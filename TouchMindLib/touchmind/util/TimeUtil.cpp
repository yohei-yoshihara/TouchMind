#include "StdAfx.h"
#include "touchmind/util/TimeUtil.h"

SYSTEMTIME touchmind::util::TimeUtil::zero;

touchmind::util::TimeUtil::TimeUtil(void) {
}

touchmind::util::TimeUtil::~TimeUtil(void) {
}

std::wostream &touchmind::util::TimeUtil::PrintSystemTime(std::wostream &out, SYSTEMTIME systemTime) {
  out << systemTime.wYear << L"/" << systemTime.wMonth << L"/" << systemTime.wDay << L" " << systemTime.wHour << L":"
      << systemTime.wMinute << L":" << systemTime.wSecond << L"." << systemTime.wMilliseconds;
  return out;
}

std::wostream &touchmind::util::TimeUtil::PrintLocalSystemTime(std::wostream &out, SYSTEMTIME systemTime) {
  SYSTEMTIME localSystemTime;
  SystemTimeToLocalSystemTime(systemTime, &localSystemTime);
  return PrintSystemTime(out, localSystemTime);
}

std::wostream &touchmind::util::TimeUtil::PrintUnixTime(std::wostream &out, const time_t _time_t) {
  static const wchar_t *fmt = L"%Y/%m/%d %H:%M:%S";
  tm t;
  localtime_s(&t, &_time_t);
  const std::time_put<wchar_t> &dateWriter = std::use_facet<std::time_put<wchar_t>>(out.getloc());
  size_t n = wcslen(fmt);
  if (dateWriter.put(out, out, ' ', &t, fmt, fmt + n).failed()) {
    throw std::runtime_error("failure to format date time");
  }
  return out;
}

void touchmind::util::TimeUtil::UnixTimeToFileTime(time_t t, FILETIME *fileTime) {
  LONGLONG ll;

  ll = Int32x32To64(t, 10000000) + 116444736000000000;
  fileTime->dwLowDateTime = (DWORD)ll;
  fileTime->dwHighDateTime = ll >> 32;
}

bool touchmind::util::TimeUtil::UnixTimeToSystemTime(time_t t, SYSTEMTIME *systemTime) {
  FILETIME fileTime;

  UnixTimeToFileTime(t, &fileTime);
  return FileTimeToSystemTime(&fileTime, systemTime) != 0;
}

void touchmind::util::TimeUtil::FileTimeToUnixTime(FILETIME fileTime, time_t *t) {
  ULARGE_INTEGER ull;
  ull.LowPart = fileTime.dwLowDateTime;
  ull.HighPart = fileTime.dwHighDateTime;
  *t = (ull.QuadPart - 116444736000000000) / 10000000;
}

bool touchmind::util::TimeUtil::SystemTimeToUnixTime(SYSTEMTIME systemTime, time_t *t) {
  FILETIME fileTime;
  if (SystemTimeToFileTime(&systemTime, &fileTime) == 0) {
    return false;
  }
  FileTimeToUnixTime(fileTime, t);
  return true;
}

void touchmind::util::TimeUtil::JavaTimeToUnixTime(LONGLONG javaTime, time_t *t, WORD *milliseconds) {
  *t = javaTime / 1000;
  *milliseconds = static_cast<WORD>(javaTime - *t * 1000);
}

void touchmind::util::TimeUtil::UnixTimeToJavaTime(time_t t, WORD milliseconds, LONGLONG *javaTime) {
  *javaTime = static_cast<long>(t) * 1000 + milliseconds;
}

bool touchmind::util::TimeUtil::JavaTimeToSystemTime(LONGLONG javaTime, SYSTEMTIME *systemTime) {
  time_t t;
  WORD milliseconds;
  JavaTimeToUnixTime(javaTime, &t, &milliseconds);
  if (!UnixTimeToSystemTime(t, systemTime)) {
    return false;
  }
  systemTime->wMilliseconds = milliseconds;
  return true;
}

bool touchmind::util::TimeUtil::SystemTimeToJavaTime(SYSTEMTIME systemTime, LONGLONG *javaTime) {
  time_t t;
  if (!SystemTimeToUnixTime(systemTime, &t)) {
    return false;
  }
  UnixTimeToJavaTime(t, systemTime.wMilliseconds, javaTime);
  return true;
}

bool touchmind::util::TimeUtil::SystemTimeToLocalSystemTime(SYSTEMTIME systemTime, SYSTEMTIME *localSystemTime) {
  TIME_ZONE_INFORMATION timeZoneInfo;
  if (GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_INVALID) {
    return false;
  }

  if (SystemTimeToTzSpecificLocalTime(&timeZoneInfo, &systemTime, localSystemTime) == 0) {
    return false;
  }
  return true;
}

bool touchmind::util::TimeUtil::LocalSystemTime(SYSTEMTIME localSystemTime, SYSTEMTIME *systemTime) {
  TIME_ZONE_INFORMATION timeZoneInfo;
  if (GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_INVALID) {
    return false;
  }

  if (TzSpecificLocalTimeToSystemTime(&timeZoneInfo, &localSystemTime, systemTime) == 0) {
    return false;
  }
  return true;
}

void touchmind::util::TimeUtil::InitializeSystemTime(SYSTEMTIME *systemTime) {
  static bool flag = false;
  if (!flag) {
    FILETIME fileTime;
    ZeroMemory(&fileTime, sizeof(fileTime));
    FileTimeToSystemTime(&fileTime, &zero);
    flag = true;
  }
  *systemTime = zero;
}

int touchmind::util::TimeUtil::CompareSystemTimes(const SYSTEMTIME *systemTime1, const SYSTEMTIME *systemTime2) {
  FILETIME fileTime1;
  FILETIME fileTime2;
  SystemTimeToFileTime(systemTime1, &fileTime1);
  SystemTimeToFileTime(systemTime2, &fileTime2);

  ULARGE_INTEGER ull1;
  ull1.HighPart = fileTime1.dwHighDateTime;
  ull1.LowPart = fileTime1.dwLowDateTime;

  ULARGE_INTEGER ull2;
  ull2.HighPart = fileTime2.dwHighDateTime;
  ull2.LowPart = fileTime2.dwLowDateTime;

  if (ull1.QuadPart == ull2.QuadPart) {
    return 0;
  } else if (ull1.QuadPart < ull2.QuadPart) {
    return -1;
  } else {
    return 1;
  }
}
