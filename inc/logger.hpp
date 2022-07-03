#pragma once

#ifndef __H_LOGGER_CPPCONFIG__
#define __H_LOGGER_CPPCONFIG__

#include "./ICPPLogger.hpp"

static __N_CPPLOGGER__::ICPPLogger* g_pLogger{ nullptr };

#define CREATELOGGER        if (!g_pLogger) g_pLogger = __N_CPPLOGGER__::CreateBoostLogger();
#define INITLOGGER(init)    if (!g_pLogger) CREATELOGGER g_pLogger->Initialize(init);
#define CHECKLOGGER	        if (g_pLogger) if (g_pLogger->IsInitialized())

#define LOG_TRC(...)    CHECKLOGGER { g_pLogger->log(__N_CPPLOGGER__::SEVERITY_TRACE     , __VA_ARGS__); }
#define LOG_INF(...)    CHECKLOGGER { g_pLogger->log(__N_CPPLOGGER__::SEVERITY_INFO      , __VA_ARGS__); }
#define LOG_DBG(...)    CHECKLOGGER { g_pLogger->log(__N_CPPLOGGER__::SEVERITY_DEBUG     , __VA_ARGS__); }
#define LOG_WRN(...)    CHECKLOGGER { g_pLogger->log(__N_CPPLOGGER__::SEVERITY_WARNING   , __VA_ARGS__); }
#define LOG_ERR(...)    CHECKLOGGER { g_pLogger->log(__N_CPPLOGGER__::SEVERITY_ERROR     , __VA_ARGS__); }
#define LOG_FAT(...)    CHECKLOGGER { g_pLogger->log(__N_CPPLOGGER__::SEVERITY_FATAL     , __VA_ARGS__); }

#endif // !__H_LOGGER_CPPCONFIG__
