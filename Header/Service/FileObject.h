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
    ///     "r"��ֻ�ܴ��ļ��ж����ݣ����ļ������ȴ��ڣ������ʧ��
    ///     "w"��ֻ�����ļ�д���ݣ���ָ�����ļ��������򴴽����������������ɾ�������ؽ�һ�����ļ�
    /// 	"a"�����ļ�����������(��ɾ��ԭ������)�����ļ����������ʧ�ܣ���ʱλ��ָ���Ƶ��ļ�ĩβ
    /// 	"r+"���ɶ� / д���ݣ����ļ������ȴ��ڣ������ʧ��
    /// 	"w+"���ɶ� / д���ݣ��ø�ģʽ���½�һ���ļ���������ļ�д���ݣ�Ȼ��ɶ�ȡ���ļ��е�����
    /// 	"a+"���ɶ� / д���ݣ�ԭ�����ļ�����ɾȥ��λ��ָ���Ƶ��ļ�ĩβ
    /// 	�򿪶������ļ���ģʽ����ı��ļ��ĺ�����һ���ģ���ͬ����ģʽ���������һ����ĸ'b�����Ա�ʾ�Զ�������ʽ���ļ�*/
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
            ESP_LOGE("FileObject", "��֧�ֵ��ļ��򿪲���");
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