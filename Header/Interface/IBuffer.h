class IBuffer
{
public:
    virtual bool ReadBuffer(void *out_buffer, uint read_size, uint offset);
    virtual bool WriteBuffer(void *data, uint write_size, uint offsset);
    virtual uint GetBufferSize();
    virtual ~IBuffer(){};
};
class IMultipleBuffer
{
public:
    virtual bool ReadBuffer(uint line_index, void *out_buffer, uint read_size, uint offset);
    virtual bool WriteBuffer(uint line_index, void *data, uint write_size, uint offsset);
    virtual uint GetLineBufferSize(); 
    virtual uint GetLineCount();
    virtual ~IMultipleBuffer(){};
};