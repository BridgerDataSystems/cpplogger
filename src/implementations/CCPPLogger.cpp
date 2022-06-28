
#include "CCPPLogger.hpp"

#include <boost/core/null_deleter.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>

namespace __N_CPPLOGGER__
{    
    void write_header(boost::log::sinks::text_file_backend::stream_type& file)
    {
        file << "<?xml version=\"1.0\"?>\n<log>\n";
    }

    void write_footer(boost::log::sinks::text_file_backend::stream_type& file)
    {
        file << "</log>\n";
    }

    std::string getSeverity(const CPPLOGGER_SEVERITY severity) noexcept
    {
        switch (severity)
        {
        case SEVERITY_TRACE   : return std::string{ "TRC" }; break;
        case SEVERITY_INFO    : return std::string{ "INF" }; break;
        case SEVERITY_DEBUG   : return std::string{ "DBG" }; break;
        case SEVERITY_WARNING : return std::string{ "WRN" }; break;
        case SEVERITY_ERROR   : return std::string{ "ERR" }; break;
        case SEVERITY_FATAL   : return std::string{ "FAT" }; break;
        default               : return std::string{ "UNK" }; break;
        }
    }

    bool CCPPLogger::Initialize(const CPPLOGGER_INIT& init) noexcept
    {
        // check id initialized
        if (this->m_bInitialized)
        {
            this->m_strLastError.assign("CCPPLogger is already initialized");
            return false;
        }

        // store init data
        this->m_init = init;

        // sink filter lambda
        auto l_filter_lambda = [init](const boost::log::attribute_value_set &set) -> bool {
            return *set["Severity"].extract<CPPLOGGER_SEVERITY>() & init.severity;
        };

        // sink formatter lambda
        auto l_formatter_lambda = [](const boost::log::record_view &view, boost::log::formatting_ostream &os) -> void {
            os  << "["      << view.attribute_values()["Timestamp"].extract<boost::posix_time::ptime>()                         << "]"
                << "["      << std::setw(5) << std::setfill('0') << view.attribute_values()["ThreadID"].extract<std::string>()  << "]"
                << "["      << getSeverity(*view.attribute_values()["Severity"].extract<CPPLOGGER_SEVERITY>())                  << "]"
                << " - "    << view.attribute_values()["Message"].extract<std::string>();
        };

        std::string l_strFilePath{};
        if (!init.folder_name.empty())
            l_strFilePath.assign(std::string{ "./" }.append(init.folder_name).append("/"));
        else
            l_strFilePath.assign("/logs/");

        this->m_pSink = boost::make_shared<TEXT_SINK>(
            boost::log::keywords::file_name = std::string{ l_strFilePath.append("file_%Y-%m-%d.%2N.log") },
            boost::log::keywords::rotation_size = 5 * 1024 * 1024,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::open_mode = std::ios_base::app
        );
        
        this->m_pSink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
            boost::log::keywords::target = "collected_logs",
            boost::log::keywords::max_size = 16 * 1024 * 1024,
            boost::log::keywords::min_free_space = 100 * 1024 * 1024
        ));

        this->m_pSink->set_filter(l_filter_lambda);
        this->m_pSink->set_formatter(l_formatter_lambda);
        this->m_pSink->locked_backend()->set_open_handler(&write_header);
        this->m_pSink->locked_backend()->set_close_handler(&write_footer);
        boost::log::core::get()->add_sink(this->m_pSink);

        this->m_bInitialized = true;
        
        return true;
    }

    bool CCPPLogger::UnInitialize() noexcept
    {
        // check id initialized
        if (!this->m_bInitialized)
        {
            this->m_strLastError.assign("CCPPLogger is NOT initialized");
            return false;
        }

        this->m_bInitialized = false;
        
        return true;
    }

    void CCPPLogger::log(const CPPLOGGER_SEVERITY severity, const char* fmt, ...) noexcept
    {
        // check id initialized
        if (!this->m_bInitialized)
        {
            this->m_strLastError.assign("CCPPLogger is NOT initialized");
            return;
        }

        std::stringstream l_stream_threadid{};
        l_stream_threadid << boost::this_thread::get_id();

        BOOST_LOG_SCOPED_LOGGER_ATTR(this->m_logger, "ThreadID", boost::log::attributes::constant<std::string>(l_stream_threadid.str()))
        BOOST_LOG_SCOPED_LOGGER_ATTR(this->m_logger, "Timestamp", boost::log::attributes::local_clock{})

        std::stringstream l_stream_text{};
        std::va_list l_args{};
        va_start(l_args, fmt);
        char* l_buffer = (char*)malloc(1024);
        std::vsprintf(l_buffer, fmt, l_args);
        l_stream_text << l_buffer;
        va_end(l_args);
        
        BOOST_LOG_SEV(this->m_logger, (int)severity) << l_stream_text.str();
        //this->m_pSink->flush();
    }
    
    ICPPLogger* CreateBoostLogger() noexcept
    {
        return new CCPPLogger();
    }
    
    CPPLOGGER_API void DestroyBoostLogger(ICPPLogger** ppObj) noexcept
    {
        if(!ppObj)
            return;

        if(!*ppObj)
            return;

        delete *ppObj;
        *ppObj = nullptr;
    }
}