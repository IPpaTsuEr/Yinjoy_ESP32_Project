// #ifdef __cplusplus
// extern "C"
// {
// #endif

// #ifdef __cplusplus
// }
// #endif
#pragma once
#include <WString.h>
class IJson
{
public:
    virtual ~IJson(){};
    virtual String GetJsonString() = 0;
};
