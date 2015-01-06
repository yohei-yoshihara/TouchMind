#include "stdafx.h"
#include "touchmind/logging/Logging.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_value_impl.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/core/null_deleter.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

#define LOG_LOCALE "English"
//#define LOG_LOCALE "Japanese"
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SEVERITY_LEVEL)

namespace touchmind
{
    namespace logging
    {
        std::wstring getTimestamp() {
            SYSTEMTIME systemTime;
            GetLocalTime(&systemTime);
            std::wstringstream ss;
            ss << systemTime.wYear << L"/"
                << std::setfill(L'0') << std::setw(2)
                << systemTime.wMonth << L"/"
                << std::setfill(L'0') << std::setw(2)
                << systemTime.wDay << L" "
                << std::setfill(L'0') << std::setw(2)
                << systemTime.wHour << L":"
                << std::setfill(L'0') << std::setw(2)
                << systemTime.wMinute << L":"
                << std::setfill(L'0') << std::setw(2)
                << systemTime.wSecond << L"."
                << std::setfill(L'0') << std::setw(3)
                << systemTime.wMilliseconds;
            return ss.str();
        }

        class SystemTimestampImpl : public boost::log::attribute::impl {
        public:
            boost::log::attribute_value get_value() {
                return attrs::make_attribute_value(getTimestamp());
            }
        };

        class SystemTimestamp : public boost::log::attribute {
        public:
            SystemTimestamp() : boost::log::attribute(new SystemTimestampImpl())
            {}
            explicit SystemTimestamp(attrs::cast_source const &source) : boost::log::attribute(source.as<SystemTimestampImpl>())
            {}
        };


        class HResultStrHelper
        {
        public:
            std::map<HRESULT, std::wstring> m;
            HResultStrHelper() {
                m.insert({ S_OK, L"S_OK" });
                m.insert({ E_UNEXPECTED, L"E_UNEXPECTED" });
                m.insert({ E_NOTIMPL, L"E_NOTIMPL" });
                m.insert({ E_OUTOFMEMORY, L"E_OUTOFMEMORY" });
                m.insert({ E_INVALIDARG, L"E_INVALIDARG" });
                m.insert({ E_NOINTERFACE, L"E_NOINTERFACE" });
                m.insert({ E_POINTER, L"E_POINTER" });
                m.insert({ E_HANDLE, L"E_HANDLE" });
                m.insert({ E_ABORT, L"E_ABORT" });
                m.insert({ E_FAIL, L"E_FAIL" });
                m.insert({ E_ACCESSDENIED, L"E_ACCESSDENIED" });
            }
        };

        HResultStrHelper hResultHelper;

        void _HResultStr(std::ios_base& os, HRESULT args)
        {
            std::wostream *pos = dynamic_cast<std::wostream*>(&os);
            if (pos) {
                if (hResultHelper.m.count(args) > 0) {
                    (*pos) << hResultHelper.m[args] << L'(' << std::hex << args << std::dec << L')';
                }
                else {
                    (*pos) << std::hex << args << std::dec;
                }
            }
        }

        void InitLogger(const wchar_t* logFileName)
        {
            boost::log::add_common_attributes();

            _wsetlocale(LC_ALL, _T(LOG_LOCALE));
            std::wofstream *wof(new std::wofstream());
            wof->imbue(std::locale(LOG_LOCALE, LC_ALL));
            wof->open(logFileName /*, std::ios::app*/);
            boost::shared_ptr<std::wofstream> strm(wof);
            if (!strm->good()) {
                throw std::runtime_error("Failed to open a text log file");
            }

            // Create a text file sink
            auto sink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::wtext_ostream_backend>>();

            sink->locked_backend()->add_stream(strm);

#ifdef _DEBUG
            sink->set_filter(severity >= SEVERITY_LEVEL_DEBUG);
            sink->locked_backend()->auto_flush(true);
#else
            sink->set_filter(severity >= SEVERITY_LEVEL_INFO);
#endif

            sink->set_formatter(
                expr::stream
                << expr::attr<std::wstring>("SystemTimestamp")
                << L" [" << expr::attr<SEVERITY_LEVEL>("Severity") << L"] "
                << expr::wmessage
                );

            // Add it to the core
            boost::log::core::get()->add_sink(sink);

            // Add some attributes too
            boost::shared_ptr< boost::log::core > core = boost::log::core::get();
            core->add_global_attribute("SystemTimestamp", SystemTimestamp());
        }
    }
}
