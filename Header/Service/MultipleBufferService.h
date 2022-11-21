#pragma once
#include <esp_heap_caps.h>
#include "../Interface/IBuffer.h"
class MultipleBufferService : IMultipleBuffer
{
    uint8_t *_buffer[15];
    uint _size;
    uint _line_count;

public:
    MultipleBufferService(uint size, uint line_count, uint32_t type)
    {
        _size = size;
        _line_count = line_count;
        assert(line_count <= 16);
        for (size_t i = 0; i < line_count; i++)
        {
            _buffer[i] =(uint8_t*) heap_caps_malloc(size, type);
            assert(_buffer[i] != nullptr);
        }
        Serial.println("MultipleBufferService 初始化成功");
    };
    bool ReadBuffer(uint line_index, void *out_buffer, uint read_size, uint offset) override
    {
        if (read_size + offset > _size)
            return false;
        memcpy(out_buffer,_buffer[line_index] + offset, read_size);
        return true;
    };
    bool WriteBuffer(uint line_index, void *data, uint write_size, uint offset) override
    {
        if (write_size + offset > _size)
            return false;
        memcpy(_buffer[line_index] + offset, data, write_size);
        return true;
    };
    bool FillData(uint line_index, uint8_t data, uint length, uint offset)
    {
        if (line_index > _line_count - 1)
            return false;
        if ((length + offset) > _size)
            return false;
        memset(_buffer[line_index] + offset, data, length);
        return true;
    }
    void *GetAddress(uint line_index)
    {
        return _buffer[line_index];
    }

    uint GetLineCount() override
    {
        return _line_count;
    };
    uint GetLineBufferSize() override
    {
        return _size;
    };
};