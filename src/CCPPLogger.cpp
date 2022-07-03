
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

        std::string l_strLogFolder{ init.log_folder_name };
        l_strLogFolder.erase(std::remove(l_strLogFolder.begin(), l_strLogFolder.end(), '/'), l_strLogFolder.end());
        l_strLogFolder.erase(std::remove(l_strLogFolder.begin(), l_strLogFolder.end(), '\\'), l_strLogFolder.end());
        l_strLogFolder.erase(std::remove(l_strLogFolder.begin(), l_strLogFolder.end(), '.'), l_strLogFolder.end());
        std::string l_strLogFileFormat{ l_strLogFolder + "/" + init.log_file_prefix + std::string{ "%Y-%m-%d.%2N" } + init.log_file_extension };

        // sink creation
        this->m_pSink = boost::make_shared<TEXT_SINK>(
            boost::log::keywords::file_name             = l_strLogFileFormat,
            boost::log::keywords::rotation_size         = init.rotation_size,
            boost::log::keywords::time_based_rotation   = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
            boost::log::keywords::scan_method           = boost::log::sinks::file::scan_matching,
            boost::log::keywords::auto_flush            = true,
            boost::log::keywords::enable_final_rotation = init.enable_final_rotation,
            boost::log::keywords::open_mode             = std::ios_base::out | std::ios_base::app
        );
        
        // sink collector
        if (init.enable_collect)
            this->m_pSink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
                boost::log::keywords::target            = init.target_folder_name,
                boost::log::keywords::max_size          = init.collect_max_size,
                boost::log::keywords::min_free_space    = init.collect_min_free_space,
                boost::log::keywords::max_files         = init.collect_max_files
            ));

        // sink setup
        this->m_pSink->set_filter(l_filter_lambda);
        this->m_pSink->set_formatter(l_formatter_lambda);
        if (init.enable_header_footer)
        {
            this->m_pSink->locked_backend()->set_open_handler(&write_header);
            this->m_pSink->locked_backend()->set_close_handler(&write_footer);
        }

        // sink addition
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

        boost::log::core::get()->remove_sink(this->m_pSink);

        this->m_pSink->stop();
        this->m_pSink->flush();
        this->m_pSink.reset();

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
    
    void DestroyBoostLogger(ICPPLogger** ppObj) noexcept
    {
        if(!ppObj)
            return;

        if(!*ppObj)
            return;

        delete *ppObj;
        *ppObj = nullptr;
    }
}