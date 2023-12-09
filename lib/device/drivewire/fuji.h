#ifndef FUJI_H
#define FUJI_H

#include <cstdint>
#include <cstring>

#include "bus.h"
#include "disk.h"
#include "network.h"
#include "cassette.h"

#include "fujiHost.h"
#include "fujiDisk.h"
#include "fujiCmd.h"

#define MAX_HOSTS 8
#define MAX_DISK_DEVICES 8
#define MAX_NETWORK_DEVICES 8

#define MAX_WIFI_PASS_LEN 64

#define MAX_APPKEY_LEN 64

#define READ_DEVICE_SLOTS_DISKS1 0x00
#define READ_DEVICE_SLOTS_TAPE 0x10

typedef struct
{
    char ssid[33];
    char hostname[64];
    unsigned char localIP[4];
    unsigned char gateway[4];
    unsigned char netmask[4];
    unsigned char dnsIP[4];
    unsigned char macAddress[6];
    unsigned char bssid[6];
    char fn_version[15];
} AdapterConfig;

enum appkey_mode : uint8_t
{
    APPKEYMODE_READ = 0,
    APPKEYMODE_WRITE,
    APPKEYMODE_INVALID
};

struct appkey
{
    uint16_t creator = 0;
    uint8_t app = 0;
    uint8_t key = 0;
    appkey_mode mode = APPKEYMODE_INVALID;
    uint8_t reserved = 0;
} __attribute__((packed));

class drivewireFuji : public virtualDevice
{
private:
    systemBus *_drivewire_bus = nullptr;

    fujiHost _fnHosts[MAX_HOSTS];

    fujiDisk _fnDisks[MAX_DISK_DEVICES];

    drivewireCassette _cassetteDev;

    int _current_open_directory_slot = -1;

    drivewireDisk _bootDisk; // special disk drive just for configuration

    uint8_t bootMode = 0; // Boot mode 0 = CONFIG, 1 = MINI-BOOT

    uint8_t _countScannedSSIDs = 0;

    appkey _current_appkey;

protected:
    void drivewire_reset_fujinet();          // 0xFF
    void drivewire_net_get_ssid();           // 0xFE
    void drivewire_net_scan_networks();      // 0xFD
    void drivewire_net_scan_result();        // 0xFC
    void drivewire_net_set_ssid();           // 0xFB
    void drivewire_net_get_wifi_status();    // 0xFA
    void drivewire_mount_host();             // 0xF9
    void drivewire_disk_image_mount();       // 0xF8
    void drivewire_open_directory();         // 0xF7
    void drivewire_read_directory_entry();   // 0xF6
    void drivewire_close_directory();        // 0xF5
    void drivewire_read_host_slots();        // 0xF4
    void drivewire_write_host_slots();       // 0xF3
    void drivewire_read_device_slots();      // 0xF2
    void drivewire_write_device_slots();     // 0xF1
    void drivewire_enable_udpstream();       // 0xF0
    void drivewire_net_get_wifi_enabled();   // 0xEA
    void drivewire_disk_image_umount();      // 0xE9
    void drivewire_get_adapter_config();     // 0xE8
    void drivewire_new_disk();               // 0xE7
    void drivewire_unmount_host();           // 0xE6
    void drivewire_get_directory_position(); // 0xE5
    void drivewire_set_directory_position(); // 0xE4
    void drivewire_set_hdrivewire_index();         // 0xE3
    void drivewire_set_device_filename();    // 0xE2
    void drivewire_set_host_prefix();        // 0xE1
    void drivewire_get_host_prefix();        // 0xE0
    void drivewire_set_drivewire_external_clock(); // 0xDF
    void drivewire_write_app_key();          // 0xDE
    void drivewire_read_app_key();           // 0xDD
    void drivewire_open_app_key();           // 0xDC
    void drivewire_close_app_key();          // 0xDB
    void drivewire_get_device_filename();    // 0xDA
    void drivewire_set_boot_config();        // 0xD9
    void drivewire_copy_file();              // 0xD8
    void drivewire_set_boot_mode();          // 0xD6

    void shutdown() override;

public:
    bool boot_config = true;

    bool status_wait_enabled = true;

    drivewireDisk *bootdisk();

    drivewireNetwork *network();

    drivewireCassette *cassette() { return &_cassetteDev; };
    void debug_tape();

    void insert_boot_device(uint8_t d);

    void setup(systemBus *drivewirebus);

    void image_rotate();
    int get_disk_id(int drive_slot);
    std::string get_host_prefix(int host_slot);

    fujiHost *get_hosts(int i) { return &_fnHosts[i]; }
    fujiDisk *get_disks(int i) { return &_fnDisks[i]; }

    void _populate_slots_from_config();
    void _populate_config_from_slots();

    void mount_all();              // 0xD7

    drivewireFuji();
};

extern drivewireFuji theFuji;

#endif // FUJI_H