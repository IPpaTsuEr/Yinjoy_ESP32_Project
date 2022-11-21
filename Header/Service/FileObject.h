#pragma once

#include "../Interface/IFile.h"
enum class FileMode : int
{
    Open = 1,
    Append = 2,
};
enum class FileAccess : int
{
    Write = 16,
    Read = 32,
    BinaryWrite = 64,
    BinaryRead = 128
};
class FileObject : public IFile
{
    String mount_point;
    String file_name;
    String file_path;
    String file_extersion;
    String file_full_path;
    FILE *_file;

    FILE *Open(String file) override { return nullptr; };
    FILE *Append(String file) override { return nullptr; };
    FILE *Create(String file) override { return nullptr; };

public:
    /// <summary>
    ///     "r"：只能从文件中读数据，该文件必须先存在，否则打开失败
    ///     "w"：只能向文件写数据，若指定的文件不存在则创建它，如果存在则先删除它再重建一个新文件
    /// 	"a"：向文件增加新数据(不删除原有数据)，若文件不存在则打开失败，打开时位置指针移到文件末尾
    /// 	"r+"：可读 / 写数据，该文件必须先存在，否则打开失败
    /// 	"w+"：可读 / 写数据，用该模式打开新建一个文件，先向该文件写数据，然后可读取该文件中的数据
    /// 	"a+"：可读 / 写数据，原来的文件不被删去，位置指针移到文件末尾
    /// 	打开二进制文件的模式与打开文本文件的含义是一样的，不同的是模式名称里面多一个字母'b’，以表示以二进制形式打开文件*/
    /// </summary>
    /// <param name="mountPoint"></param>
    /// <param name="file_path_ame"></param>
    /// <param name="file_mode"></param>
    /// <param name="file_access"></param>
    FileObject(String mountPoint, String file_path_name, FileMode file_mode, int file_access) : mount_point(mountPoint)
    {
        file_full_path = mount_point + file_path_name;
        String mode_str = "";

        switch ((int)file_mode | (int)file_access)
        {
        case (int)FileMode::Open | (int)FileAccess::Write:
            mode_str += "w";
            break;
        case (int)FileMode::Open | (int)FileAccess::Read:
            mode_str += "r";
            break;
        case (int)FileMode::Open | (int)FileAccess::Write | (int)FileAccess::Read:
            mode_str += "w+";
            break;
        case (int)FileMode::Append | (int)FileAccess::Write:
            mode_str += "aw";
            break;
        case (int)FileMode::Append | (int)FileAccess::Read:
            mode_str += "ar";
            break;
        case (int)FileMode::Append | (int)FileAccess::Write | (int)FileAccess::Read:
            mode_str += "a+";
            break;
        default:
            ESP_LOGE("FileObject", "不支持的文件打开参数");
            return;
        }
        if (file_access == (int)FileAccess::BinaryRead || file_access == (int)FileAccess::BinaryWrite)
            mode_str += "b";

        _file = fopen(file_full_path.c_str(), mode_str.c_str());
    };

    bool Close() override
    {
        return fclose(_file) != 0;
    };

    bool Delete() override
    {
        if (Exists(file_full_path))
        {
            return unlink(file_full_path.c_str()) != -1;
        }
        return false;
    };

    bool Exists() override
    {
        struct stat st;
        return stat(file_full_path.c_str(), &st) != -1;
    };

    bool Exists(String path_name)
    {
        struct stat st;
        return stat(path_name.c_str(), &st) != -1;
    };

    bool Rename(String new_name) override
    {
        return rename(file_full_path.c_str(), new_name.c_str()) == 0;
    };

    size_t Write(void *buffer, size_t size, size_t offset) override
    {
        return fwrite(buffer, sizeof(byte), size, _file);
    };

    size_t Read(void *buffer, size_t size, size_t offset) override
    {
        return fread(buffer, sizeof(byte), size, _file);
    };

    size_t WriteChar(String data) override
    {
        return fputs(data.c_str(), _file);
    };
    size_t WriteChar(const char *data) override
    {
        return fputs(data, _file);
    };
    char ReadChar() override
    {
        return fgetc(_file);
    };
    char* ReadChar(char *buffer, size_t size)
    {
        return fgets(buffer, sizeof(char) * size, _file);
    };
};