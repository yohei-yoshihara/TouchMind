#ifndef TOUCHMIND_UTIL_H_
#define TOUCHMIND_UTIL_H_

namespace touchmind
{
namespace util
{

extern void _WMessage(std::ios_base& os, UINT message);

inline std::_Smanip<UINT> __cdecl WMessage(UINT message)
{
    return (std::_Smanip<UINT>(&_WMessage, message));
}

/*
extern void _LastError(std::ios_base& os, wchar_t * functionName);

inline std::_Smanip<wchar_t*> __cdecl LastError(wchar_t * functionName)
{
    return (std::_Smanip<wchar_t*>(&_LastError, functionName));
}
*/

class LastErrorArgs
{
public:
    LastErrorArgs() : functionName(), lastError(0) {}
    LastErrorArgs(const std::wstring &_functionName, DWORD _lastError) :
        functionName(_functionName), lastError(_lastError) {}
    std::wstring functionName;
    DWORD lastError;
};

extern void _LastError(std::ios_base& os, LastErrorArgs args);

inline std::_Smanip<LastErrorArgs> __cdecl LastError(LastErrorArgs args)
{
    return (std::_Smanip<LastErrorArgs>(&_LastError, args));
}

inline size_t ToBlockSize(size_t original, size_t blockSize)
{
    size_t result = blockSize;
    if ( 0 != original) {
        size_t remainder = original % blockSize;
        result = original + (0 != remainder ?
                             blockSize - remainder : 0);
    }
    return result;
}


} // util
} // touchmind

#endif // TOUCHMIND_UTIL_H_