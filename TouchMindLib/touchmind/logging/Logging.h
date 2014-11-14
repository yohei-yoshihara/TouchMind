#ifndef TOUCHMIND_LOGGING_LOGGING_H_
#define TOUCHMIND_LOGGING_LOGGING_H_

#pragma warning(disable: 4503)

// Boost Log
#define BOOST_LOG_USE_WCHAR_T
#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)
#pragma warning(disable: 4251)
#pragma warning(disable: 4503)
#pragma warning(disable: 4634)
#pragma warning(disable: 4714)
#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#define _AFX_SECURE_NO_WARNINGS
#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#undef _AFX_SECURE_NO_WARNINGS
#undef _CRT_NONSTDC_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#pragma warning(pop)

namespace touchmind
{
namespace logging
{
// ********** HRESULT **********

extern void _HResultStr(std::ios_base& os, HRESULT args);

inline std::_Smanip<HRESULT> __cdecl HResultStr(HRESULT args)
{
    return (std::_Smanip<HRESULT>(&_HResultStr, args));
}

extern void InitLogger(const wchar_t* logFileName);
} // logging
} // touchmind

#define LOG(sev) BOOST_LOG_SEV(logger::get(),sev) << __FUNCTION__ << L"(" << __LINE__ << L") - "

enum SEVERITY_LEVEL {
    SEVERITY_LEVEL_DEBUG_L3,
    SEVERITY_LEVEL_DEBUG_L2,
    SEVERITY_LEVEL_DEBUG,
    SEVERITY_LEVEL_INFO,
    SEVERITY_LEVEL_WARN,
    SEVERITY_LEVEL_ERROR,
    SEVERITY_LEVEL_FATAL
};

inline std::wostream& operator<< (std::wostream& strm, SEVERITY_LEVEL const& val)
{
    switch (val) {
    case SEVERITY_LEVEL_DEBUG_L3:
        strm << L"DDD";
        break;
    case SEVERITY_LEVEL_DEBUG_L2:
        strm << L"DD";
        break;
    case SEVERITY_LEVEL_DEBUG:
        strm << L'D';
        break;
    case SEVERITY_LEVEL_INFO:
        strm << L'I';
        break;
    case SEVERITY_LEVEL_WARN:
        strm << L'W';
        break;
    case SEVERITY_LEVEL_ERROR:
        strm << L'E';
        break;
    case SEVERITY_LEVEL_FATAL:
        strm << L'F';
        break;
    }
    return strm;
}

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, boost::log::sources::wseverity_logger< SEVERITY_LEVEL >);

#define CHK_FATAL_HRESULT(expression)\
{\
    HRESULT __hr__ = expression;\
    if(FAILED(__hr__)) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", hr = " << std::hex << __hr__ << std::dec;\
    throw std::runtime_error(#expression);\
    }\
}

#define CHK_HR(expression) CHK_FATAL_HRESULT(expression)

/*
 * CHK_FATAL_HR_RESOURCE
 */

#if (defined(DEBUG) || defined(_DEBUG)) && defined(DEBUG_GPU_RESOURCE)

#define CHK_FATAL_HR_RESOURCE(resource, expression)\
{\
    HRESULT __hr__ = expression;\
    LOG(SEVERITY_LEVEL_DEBUG) << L"[GPU RESOURCE] " << #resource << L"[" << typeid((resource)).name() << L"] = [" << std::hex << (resource) << std::dec << L"]";\
    if(FAILED(__hr__)) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", hr = " << std::hex << __hr__ << std::dec;\
    throw std::runtime_error(#expression);\
    }\
}

#else

#define CHK_FATAL_HR_RESOURCE(resource, expression)\
{\
    HRESULT __hr__ = expression;\
    if(FAILED(__hr__)) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", hr = " << std::hex << __hr__ << std::dec;\
    throw std::runtime_error(#expression);\
    }\
}

#endif

#define CHK_RES(resource, expression) CHK_FATAL_HR_RESOURCE(resource, expression)

#if (defined(DEBUG) || defined(_DEBUG)) && defined(DEBUG_GPU_RESOURCE)
#define LOG_RES(resource) LOG(SEVERITY_LEVEL_DEBUG) << L"[RESOURCE] " << #resource << L"[" << typeid((resource)).name() << L"] = [" << std::hex << (resource) << std::dec << L"]"
#else
#define LOG_RES(resource)
#endif

#define CHK_FATAL_BOOL(expression)\
{\
    BOOL __b__ = expression;\
    if(__b__ == 0) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", last error = " << std::hex << GetLastError() << std::dec;\
    throw std::runtime_error(#expression);\
    }\
}

#define CHK_FATAL_NULL(expression)\
{\
    if((expression) == nullptr) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", last error = " << std::hex << GetLastError() << std::dec;\
    throw std::runtime_error(#expression);\
    }\
}

#define CHK_ERROR_HRESULT(retValue, expression)\
    retValue = expression;\
    if(FAILED(retValue)) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", hr = " << std::hex << retValue << std::dec;\
    }

#define CHK_ERROR_BOOL(retValue, expression)\
    retValue = expression;\
    if(retValue == 0) {\
    LOG(SEVERITY_LEVEL_FATAL) << #expression << L", last error = " << std::hex << GetLastError() << std::dec;\
    throw std::runtime_error(#expression);\
    }

#define CHK_WARN_HRESULT(expression)\
{\
    HRESULT __hr__ = expression;\
    if(FAILED(__hr__)) {\
    LOG(SEVERITY_LEVEL_WARN) << #expression << L", hr = " << std::hex << __hr__ << std::dec;\
    }\
}

#define CHK_WARN_BOOL(expression)\
{\
    BOOL __b__ = expression;\
    if(__b__ == 0) {\
    LOG(SEVERITY_LEVEL_WARN) << #expression << L", last error = " << std::hex << GetLastError() << std::dec;\
    }\
}

#if defined(DEBUG) || defined(_DEBUG)

#define LOG_ENTER BOOST_LOG_SEV(logger::get(),SEVERITY_LEVEL_DEBUG) << __FUNCTION__ << L"(" << __LINE__ << L") <ENTER>"
#define LOG_ENTER_ARG(args) BOOST_LOG_SEV(logger::get(),SEVERITY_LEVEL_DEBUG) << __FUNCTION__ << L"(" << __LINE__ << L") <ENTER> - " << args

#define LOG_LEAVE BOOST_LOG_SEV(logger::get(),SEVERITY_LEVEL_DEBUG) << __FUNCTION__ << L"(" << __LINE__ << L") <LEAVE>"
#define LOG_LEAVE_ARG(args) BOOST_LOG_SEV(logger::get(),SEVERITY_LEVEL_DEBUG) << __FUNCTION__ << L"(" << __LINE__ << L") <LEAVE> - " << args
#define LOG_LEAVE_HRESULT(hr) BOOST_LOG_SEV(logger::get(),SEVERITY_LEVEL_DEBUG) << __FUNCTION__ << L"(" << __LINE__ << L") <LEAVE> - " << L"HRESULT = " << touchmind::logging::HResultStr(hr)

#else

#define LOG_ENTER ;
#define LOG_ENTER_ARG(args) ;

#define LOG_LEAVE ;
#define LOG_LEAVE_ARG(args) ;
#define LOG_LEAVE_HRESULT(hr) ;

#endif

#endif // TOUCHMIND_LOGGING_LOGGING_H_