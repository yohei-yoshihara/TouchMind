#include "stdafx.h"
#include "touchmind/logging/Logging.h"

#define LOG_LOCALE "English"
//#define LOG_LOCALE "Japanese"

namespace touchmind
{
namespace logging
{

class HResultStrHelper
{
public:
    std::map<HRESULT, std::wstring> m;
    HResultStrHelper() {
        m.insert(std::make_pair<HRESULT, std::wstring>(S_OK, L"S_OK"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_UNEXPECTED, L"E_UNEXPECTED"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_NOTIMPL, L"E_NOTIMPL"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_OUTOFMEMORY, L"E_OUTOFMEMORY"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_INVALIDARG, L"E_INVALIDARG"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_NOINTERFACE, L"E_NOINTERFACE"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_POINTER, L"E_POINTER"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_HANDLE, L"E_HANDLE"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_ABORT, L"E_ABORT"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_FAIL, L"E_FAIL"));
        m.insert(std::make_pair<HRESULT, std::wstring>(E_ACCESSDENIED, L"E_ACCESSDENIED"));
    }
};

HResultStrHelper hResultHelper;

void _HResultStr(std::ios_base& os, HRESULT args)
{
    std::wostream *pos = dynamic_cast<std::wostream*>(&os);
    if (pos) {
        if (hResultHelper.m.count(args) > 0) {
            (*pos) << hResultHelper.m[args] << L'(' << std::hex <<  args << std::dec << L')';
        } else {
            (*pos) <<  std::hex <<  args << std::dec;
        }
    }
}


void InitLogger(const wchar_t* logFileName)
{
    _wsetlocale(LC_ALL, _T(LOG_LOCALE));
    std::wofstream *wof(new std::wofstream());
    wof->imbue(std::locale(LOG_LOCALE, LC_ALL));
    wof->open(logFileName /*, std::ios::app*/);
    boost::shared_ptr< std::wofstream > strm(wof);
    if (!strm->good()) {
        throw std::runtime_error("Failed to open a text log file");
    }

    // Create a text file sink
    boost::shared_ptr< boost::log::sinks::synchronous_sink< boost::log::sinks::wtext_ostream_backend > > sink(
        new boost::log::sinks::synchronous_sink< boost::log::sinks::wtext_ostream_backend >);

    sink->locked_backend()->add_stream(strm);

#ifdef _DEBUG
    sink->set_filter(
        boost::log::filters::attr< SEVERITY_LEVEL >(L"Severity", std::nothrow) >= SEVERITY_LEVEL_DEBUG
    );
    sink->locked_backend()->auto_flush(true);
#else
    sink->set_filter(
        boost::log::filters::attr< SEVERITY_LEVEL >(L"Severity", std::nothrow) >= SEVERITY_LEVEL_INFO
    );
#endif
    sink->locked_backend()->set_formatter(
        boost::log::formatters::format(L"%1% [%2%] <%3%> %4%")
        % boost::log::formatters::date_time(L"TimeStamp", std::nothrow)
        % boost::log::formatters::time_duration(L"Uptime", std::nothrow)
        % boost::log::formatters::attr< SEVERITY_LEVEL >(L"Severity", std::nothrow)
        % boost::log::formatters::wmessage()
    );

    // Add it to the core
    boost::log::wcore::get()->add_sink(sink);

    // Add some attributes too
    boost::shared_ptr< boost::log::attribute > attr(new boost::log::attributes::local_clock);
    boost::log::wcore::get()->add_global_attribute(L"TimeStamp", attr);
    boost::log::wcore::get()->add_global_attribute(L"Uptime", boost::make_shared< boost::log::attributes::timer >());
}
}
}
