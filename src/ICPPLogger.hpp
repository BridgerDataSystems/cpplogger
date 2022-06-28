#pragma once

#ifndef __H_ICPPLOGGER__
#define __H_ICPPLOGGER__

#ifdef __cplusplus
    #define EXTERNC extern "C"
#else
    #define EXTERNC
#endif // !__cplusplus

#ifdef CPPLOGGER_EXPORTS
    #define  CPPLOGGER_API  EXTERNC __declspec(dllexport)
#else
    #define  CPPLOGGER_API  EXTERNC __declspec(dllimport)
    #pragma comment(lib, "cpplogger")
#endif // !CPPLOGGER_EXPORTS


#include <string>
#include <deque>

namespace __N_CPPLOGGER__
{
    using CPPLOGGER_SEVERITY = unsigned short int;

    constexpr CPPLOGGER_SEVERITY SEVERITY_TRACE   = 0x01; // b00000001,
    constexpr CPPLOGGER_SEVERITY SEVERITY_INFO    = 0x02; // b00000010,
    constexpr CPPLOGGER_SEVERITY SEVERITY_DEBUG   = 0x04; // b00000100,
    constexpr CPPLOGGER_SEVERITY SEVERITY_WARNING = 0x08; // b00001000,
    constexpr CPPLOGGER_SEVERITY SEVERITY_ERROR   = 0x10; // b00010000,
    constexpr CPPLOGGER_SEVERITY SEVERITY_FATAL   = 0x20; // b00100000

    typedef struct __cpplogger_init
    {
        CPPLOGGER_SEVERITY severity =
            SEVERITY_TRACE   |
            SEVERITY_INFO    |
            SEVERITY_DEBUG   |
            SEVERITY_WARNING |
            SEVERITY_ERROR   |
            SEVERITY_FATAL;
        std::string folder_name{ "log" };
        std::string file_prefix{ "log_"};
        std::string file_extension{ "log" };
    } CPPLOGGER_INIT, * LP_CPPLOGGER_INIT;

    class ICPPLogger
    {
    private:
    protected:
        bool m_bInitialized{ false };
        std::string m_strLastError{};

    public:
        ICPPLogger() = default;
        ICPPLogger(const ICPPLogger&) = default;
        ICPPLogger(ICPPLogger&&) = default;
        ICPPLogger& operator = (const ICPPLogger&) = default;
        ICPPLogger& operator = (ICPPLogger&&) = default;
        virtual ~ICPPLogger() = default;

        virtual bool Initialize(const CPPLOGGER_INIT&) noexcept = 0;
        virtual bool UnInitialize() noexcept = 0;
        bool IsInitialized() const noexcept { return this->m_bInitialized; }

        virtual void log(const CPPLOGGER_SEVERITY, const char*, ...) noexcept = 0;

        std::string getLastError() const noexcept { return this->m_strLastError; }
    };

    CPPLOGGER_API ICPPLogger* CreateBoostLogger() noexcept;
    CPPLOGGER_API void DestroyBoostLogger(ICPPLogger** ppObj) noexcept;
} // !__N_CPPLOGGER__

#endif // !__H_ICPPLOGGER__