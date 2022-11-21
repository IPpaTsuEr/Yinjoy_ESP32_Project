#pragma once

class IFile {
public:
	virtual FILE* Open(String file) = 0;
	virtual FILE* Append(String file) = 0;
	virtual FILE* Create(String file) = 0;

	virtual bool Close() = 0;
	virtual bool Delete() = 0;
	virtual bool Exists() = 0;
	virtual bool Rename(String new_name) = 0;

	virtual size_t Write(void* buffer, size_t size, size_t offset) = 0;
	virtual size_t Read(void* buffer, size_t size, size_t offset) = 0;
	virtual size_t WriteChar(String data) = 0;
	virtual size_t WriteChar(const char* data) = 0;
	virtual char ReadChar() = 0;
};