#ifndef TOUCHMIND_UTIL_TIMEUTIL_H_
#define TOUCHMIND_UTIL_TIMEUTIL_H_

namespace touchmind
{
namespace util
{
class TimeUtil
{
private:
    static SYSTEMTIME zero;
    TimeUtil(void);
    ~TimeUtil(void);

public:
    static std::wostream& PrintSystemTime(std::wostream& out, SYSTEMTIME systemTime);
    static std::wostream& PrintLocalSystemTime(std::wostream& out, SYSTEMTIME systemTime);
    static std::wostream& PrintUnixTime(std::wostream& out, const time_t _time_t);
    static void UnixTimeToFileTime(time_t t, FILETIME* fileTime);
    static bool UnixTimeToSystemTime(time_t t, SYSTEMTIME* systemTime);
    static void FileTimeToUnixTime(FILETIME fileTime, time_t* t);
    static bool SystemTimeToUnixTime(SYSTEMTIME systemTime, time_t* t);
    static void JavaTimeToUnixTime(LONGLONG javaTime, time_t* t, WORD* milliseconds);
    static void UnixTimeToJavaTime(time_t t, WORD milliseconds, LONGLONG* javaTime);
    static bool JavaTimeToSystemTime(LONGLONG javaTime, SYSTEMTIME* systemTime);
    static bool SystemTimeToJavaTime(SYSTEMTIME systemTime, LONGLONG* javaTime);

    static bool SystemTimeToLocalSystemTime(SYSTEMTIME systemTime, SYSTEMTIME* localSystemTime);
    static bool LocalSystemTime(SYSTEMTIME localSystemTime, SYSTEMTIME* systemTime);

    static void InitializeSystemTime(SYSTEMTIME* systemTime);
    static int CompareSystemTimes(const SYSTEMTIME* systemTime1, const SYSTEMTIME* systemTime2);
};
} // util
} // touchmind

#endif // TOUCHMIND_UTIL_TIMEUTIL_H_