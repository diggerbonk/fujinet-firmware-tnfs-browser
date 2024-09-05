#include "fujiMenu.h"

#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include "../../include/debug.h"

#include "fnFsSD.h"
#include "fnFsTNFS.h"

#include "utils.h"

bool fujiMenu::init(const char *path, FILE * mf)
{
    Debug_printf("fujiMenu::init, IN");
    release();
    _menu_file = mf;
    return true;
}

void fujiMenu::release()
{
    _current_offset = 0;
    _current_pos = 0;

    if (_menu_file != nullptr) 
    {
        fclose(_menu_file);
        _menu_file = nullptr;
    }
}

bool fujiMenu::set_pos(uint16_t newPos) 
{

    char tempBuf[MAX_MENU_LINE_LEN];

    // find the offset of the new menu position.
    _current_pos = 0;
    _current_offset = 0;
    int linelen = 0;

    while (_current_pos < newPos && fgets(tempBuf, MAX_MENU_LINE_LEN, _menu_file)) 
    {

        linelen = strlen(tempBuf);

        if (linelen <= 0)
        {
            break;
        }
        else if ((_current_offset + linelen) > MAX_MENU_SIZE) {
            Debug_printf("fujiMenu::init, truncating menu because it is over MAX_MENU_SIZE");
            break;
        }
        else if ((_current_pos + 1) > MAX_MENU_LINES) {
            Debug_printf("fujiMenu::init, truncating menu because it is over MAX_MENU_LINES");
            break;
        }

        _current_pos += 1;
        _current_offset += linelen;
    }

    return true;
}

uint16_t fujiMenu::decode_menutype(const char * buf)
{
    return (uint16_t)strtol(buf, nullptr, 16);
}

fsdir_entry_t * fujiMenu::next_menu_entry() 
{
    char tempBuf[MAX_MENU_LINE_LEN];
    _type = MENU_TYPE_TEXT;
    _name_len = 0;
    _item_len = 0;
    memset(_name, 0, MAX_MENU_NAME_LEN);
    memset(_item, 0, MAX_MENU_ITEM_LEN);
    uint8_t nameStart = 0;
    uint8_t itemStart = 0;

    // if we have an offset, skip to it. 
    if (_current_offset > 0) 
    {
        if (fseek(_menu_file, _current_offset, 0) != 0)
        {
            Debug_printf("fujiMenu::get_next_menu_entry, cannot seek to current offset.");
            return nullptr;
        }
    }

    if (fgets(tempBuf, MAX_MENU_LINE_LEN, _menu_file)) 
    {
        _current_pos += 1;
        _current_offset += strlen(tempBuf);

        // menu format: [<type>|]<name>[|<item>]

        int len = strlen(tempBuf);

        if (len>0 && tempBuf[len-1] == '\n') {
            tempBuf[len-1] = 0;
            len--;
        }
        else return nullptr;

        char * pt = strchr(tempBuf, '|');
        if (pt && (pt - tempBuf) < 5)
        {
            nameStart = pt-tempBuf+1;
            _name_len = len+1;
            _type = decode_menutype(tempBuf);

            pt = strchr(&tempBuf[nameStart], '|');
            if (pt)
            {
                _name_len = (pt - (tempBuf + nameStart));
                itemStart = 4 + _name_len;
                _item_len = len - itemStart;
            }
            else
            {
                itemStart = nameStart;
                _name_len = len - nameStart;
                _item_len = _name_len;
            }
        }
        else
        {
            _name_len = len;
            _item_len = _name_len;
        }

        if (_name_len >= MAX_MENU_NAME_LEN) _name_len = MAX_MENU_NAME_LEN-1;
        if (_item_len >= MAX_MENU_ITEM_LEN) _item_len = MAX_MENU_ITEM_LEN-1;

        strncpy(_name, &tempBuf[nameStart], _name_len);
        strncpy(_item, &tempBuf[itemStart], _item_len);

        // populate _direntry;
        strncpy(_direntry.filename, _item, MAX_PATHLEN);
        _direntry.isDir = (_type == MENU_TYPE_FOLDER);
        _direntry.size = 0;
        _direntry.modified_time = 0;
        _direntry.menu_type = _type;
        return &_direntry;
    }
    else return nullptr;
}

uint8_t fujiMenu::get_name(char * p)
{
    strcpy(p, _name);
    return _name_len;
}

uint8_t fujiMenu::get_item(char *p)
{
    strcpy(p, _item);
    return _item_len;
}
