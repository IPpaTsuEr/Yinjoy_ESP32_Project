#pragma once
#include <esp_heap_caps.h>
#include "../Interface/IBuffer.h"

class BufferService : IBuffer
{
    void *_buffer;
    uint _size;

public:
    BufferService(uint size, uint32_t type)
    {
        _size = size;
        _buffer = heap_caps_malloc(size, type);
        assert(_buffer != nullptr);
    };
    bool ReadBuffer(void *out_buffer, uint read_size, uint offset) override
    {
        if (read_size + offset > _size)
            return false;
        memcpy(out_buffer, (uint8_t*)_buffer + offset, read_size);
        return true;
    };
    bool WriteBuffer(void *data, uint write_size, uint offset) override
    {
        if (read_size + offset > _size)
            return false;
        memcpy((uint8_t*)_buffer + offset, data, read_size);
        return true;
    };
    uint GetBufferSize() override
    {
        return _size;
    }
}