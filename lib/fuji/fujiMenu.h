#ifndef _FUJI_MENU_
#define _FUJI_MENU_

#include <stdio.h>
#include "fnFS.h"

#define MAX_MENU_SIZE 65535
#define MAX_MENU_NAME_LEN 80
#define MAX_MENU_ITEM_LEN 168
#define MAX_MENU_LINE_LEN MAX_PATHLEN
#define MAX_MENU_LINES 4096

#define MENU_TYPE_TEXT 0
#define MENU_TYPE_FOLDER 1
#define MENU_TYPE_BINARY 2
#define MENU_TYPE_LINK 3
#define MENU_TYPE_SUBMENU 4

class fujiMenu
{
private:

    FILE * _menu_file = nullptr;
    uint16_t _current_offset = 0;
    uint16_t _current_pos = 0;
    fsdir_entry _direntry;

    uint16_t _type = MENU_TYPE_TEXT;
    uint8_t _name_len = 0;
    char _name[MAX_MENU_NAME_LEN];
    uint8_t _item_len = 0;
    char _item[MAX_MENU_ITEM_LEN];
    uint16_t decode_menutype(const char * buf);

public:

    fujiMenu() {};
    ~fujiMenu() {};

    uint16_t get_menu_entry_type() { return _type; };
    uint8_t get_name_len() { return _name_len; };
    uint8_t get_item_len() { return _item_len; };
    uint8_t get_name(char * p);
    uint8_t get_item(char * p);
    bool init(const char *path, FILE * mf);
    void release();
    bool get_initialized() { return (_menu_file != nullptr); };
    uint16_t get_pos() { return _current_offset; };
    bool set_pos(uint16_t newPos);
    fsdir_entry_t *  next_menu_entry();

};

#endif // _FUJI_MENU_
