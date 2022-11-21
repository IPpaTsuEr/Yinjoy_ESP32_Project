#include <driver/sdmmc_host.h>
#include <esp_vfs_fat.h>
#include "../SPIDevice/SPISDDevice.h"
#include "../Service/FileObject.h"

class SDCardService : protected SPISDDevice
{
    String _MountPoint;
    esp_vfs_fat_mount_config_t _Mount_Cofig;
    sdmmc_card_t *_SD_Card;
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    bool _is_sdmmc_mount = false;

public:
    SDCardService(
        String mountPoint,
        spi_host_device_t Host_Slot,
        gpio_num_t In_Port,
        gpio_num_t Out_Port,
        gpio_num_t CS_Port,
        gpio_num_t SCLK_Port) : SPISDDevice(Host_Slot, In_Port, Out_Port, CS_Port, SCLK_Port),
                                _MountPoint(mountPoint)
    {
        _Mount_Cofig.format_if_mount_failed = false;
        _Mount_Cofig.max_files = 5;
        _Mount_Cofig.allocation_unit_size = CONFIG_WL_SECTOR_SIZE; // 16 * 1024;

        auto res = esp_vfs_fat_sdspi_mount(_MountPoint.c_str(), &_SD_Host, &_Device_Cofig, &_Mount_Cofig, &_SD_Card);

        if (res == ESP_FAIL)
        {
            ESP_LOGE("SDCardService", "SPI挂载失败，尝试格式化并挂载...");

            _Mount_Cofig.format_if_mount_failed = true;
            res = esp_vfs_fat_sdspi_mount(_MountPoint.c_str(), &_SD_Host, &_Device_Cofig, &_Mount_Cofig, &_SD_Card);
        }

        if (res != ESP_OK)
        {
            ESP_LOGE("SDCardService", "Failed to initialize the card (%s). "
                                      "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));

            // ESP_LOGE("SDCardService", "挂载失败， 尝试MMC挂载...");
            // _Mount_Cofig.format_if_mount_failed = false;
            // res = esp_vfs_fat_sdmmc_mount(_MountPoint.c_str(), &_SD_Host, &slot_config, &_Mount_Cofig, &_SD_Card);

            // if (res == ESP_FAIL)
            // {
            //     ESP_LOGE("SDCardService", "MMC挂载失败，尝试格式化并挂载...");
            //     _Mount_Cofig.format_if_mount_failed = true;
            //     res = esp_vfs_fat_sdmmc_mount(_MountPoint.c_str(), &_SD_Host, &slot_config, &_Mount_Cofig, &_SD_Card);
            //     _is_sdmmc_mount = true;
            // }
            // else if (res != ESP_OK)
            // {
            //     ESP_LOGE("SDCardService", "MMC挂载失败");
            // }
        }
        if (res == ESP_OK)
        {
            ESP_LOGE("SDCardService", "挂载成功...");
            sdmmc_card_print_info(stdout, _SD_Card);
            // get_fatfs_usage();
        }
        else
        {
            ESP_LOGE("SDCardService", "挂载失败...");
        }
    };

    ~SDCardService()
    {
        if (_is_sdmmc_mount)
            esp_vfs_fat_sdmmc_unmount();
        else
            esp_vfs_fat_sdcard_unmount(_MountPoint.c_str(), _SD_Card);
    };

    /*FILE* Open(String File_Path_Name, String Mode)
    {
        auto Full_Path = _MountPoint + File_Path_Name;
        return fopen(Full_Path.c_str(), Mode.c_str());
    };
    FILE* Open(String File_Path_Name)
    {
        return Open(File_Path_Name, "r+");
    };
    FILE* AppendFile(String File_Path_Name)
    {
        return Open(File_Path_Name, "a+");
    };

    FILE* Create(String File_Path_Name)
    {
        return Open(File_Path_Name, "w+");
    };

    bool Close(FILE* file)
    {
        return fclose(file) != 0;
    };
    bool Delete(String File_Path_Name)
    {
        if (Exists(File_Path_Name))
        {
            auto Full_Path = _MountPoint + File_Path_Name;
            return unlink(Full_Path.c_str()) != -1;
        }
        return false;
    };
    bool Exists(String File_Path_Name)
    {
        auto Full_Path = _MountPoint + File_Path_Name;
        struct stat st;
        return stat(Full_Path.c_str(), &st) != -1;
    };

    bool Rename(String file, String new_name) {
        return rename(file.c_str(), new_name.c_str()) == 0;
    };

    size_t Read(FILE* file, byte* buffer, size_t size, size_t offset)
    {
        return fread(buffer, sizeof(byte), size, file);
    };

    size_t Write(FILE* file, byte* buffer, size_t size, size_t offset)
    {
        return fwrite(buffer, sizeof(byte), size, file);
    };*/
};