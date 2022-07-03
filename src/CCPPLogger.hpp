#pragma once

#ifndef __H_CCPPLOGGER__
#define __H_CCPPLOGGER__

#include <ICPPLogger.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks.hpp>

namespace __N_CPPLOGGER__
{
    using TEXT_SINK = boost::log::sinks::asynchronous_sink<boost::log::sinks::text_file_backend>;

    class CCPPLogger : public ICPPLogger
    {
    private:
    CPPLOGGER_INIT m_init{};
    boost::shared_ptr<TEXT_SINK> m_pSink{};
    boost::log::sources::severity_logger<CPPLOGGER_SEVERITY> m_logger{};

    protected:
    public:
        CCPPLogger() = default;
        CCPPLogger(const CCPPLogger&) = default;
        CCPPLogger(CCPPLogger&&) = default;
        CCPPLogger& operator = (const CCPPLogger&) = default;
        CCPPLogger& operator = (CCPPLogger&&) = default;
        virtual ~CCPPLogger() { this->UnInitialize(); }

        bool Initialize(const CPPLOGGER_INIT&) noexcept final;
        bool UnInitialize() noexcept final;

        void log(const CPPLOGGER_SEVERITY, const char*, ...) noexcept final;
    };
} // !__N_CPPLOGGER__

#endif // !__H_CCPPLOGGER__