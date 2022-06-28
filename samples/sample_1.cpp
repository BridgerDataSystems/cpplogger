
#include <ICPPLogger.hpp>

#include <iostream>
#include <thread>
#include <chrono>

void func(__N_CPPLOGGER__::ICPPLogger* pLogger)
{
    pLogger->log(__N_CPPLOGGER__::SEVERITY_TRACE  , "Start");
    pLogger->log(__N_CPPLOGGER__::SEVERITY_TRACE  , "End");
}

int main() {
    
    auto l_pLogger = __N_CPPLOGGER__::CreateBoostLogger();

    __N_CPPLOGGER__::CPPLOGGER_INIT l_init{};
    l_init.severity = __N_CPPLOGGER__::SEVERITY_TRACE | __N_CPPLOGGER__::SEVERITY_INFO | __N_CPPLOGGER__::SEVERITY_ERROR;
    if (!l_pLogger->Initialize(l_init))
    {
        std::cout << l_pLogger->getLastError() << std::endl;
        return 1;
    }

    func(l_pLogger);

    {
        l_pLogger->log(__N_CPPLOGGER__::SEVERITY_TRACE  , "text is: %s - %d", "text", 1);
        l_pLogger->log(__N_CPPLOGGER__::SEVERITY_INFO   , "text is: %s - %d", "text", 1);
        l_pLogger->log(__N_CPPLOGGER__::SEVERITY_DEBUG  , "text is: %s - %d", "text", 1);
        l_pLogger->log(__N_CPPLOGGER__::SEVERITY_WARNING, "text is: %s - %d", "text", 1);
        l_pLogger->log(__N_CPPLOGGER__::SEVERITY_ERROR  , "text is: %s - %d", "text", 1);
        l_pLogger->log(__N_CPPLOGGER__::SEVERITY_FATAL  , "text is: %s - %d", "text", 1);
    }

    for (int i = 0; i < 10; i++)
    {
        auto l_thread = std::thread([l_pLogger](){
            l_pLogger->log(__N_CPPLOGGER__::SEVERITY_TRACE  , "text is: %s - %d", "text", 1);
            l_pLogger->log(__N_CPPLOGGER__::SEVERITY_INFO   , "text is: %s - %d", "text", 1);
            l_pLogger->log(__N_CPPLOGGER__::SEVERITY_DEBUG  , "text is: %s - %d", "text", 1);
            l_pLogger->log(__N_CPPLOGGER__::SEVERITY_WARNING, "text is: %s - %d", "text", 1);
            l_pLogger->log(__N_CPPLOGGER__::SEVERITY_ERROR  , "text is: %s - %d", "text", 1);
            l_pLogger->log(__N_CPPLOGGER__::SEVERITY_FATAL  , "text is: %s - %d", "text", 1);
        });

        //system("pause");
        l_thread.join();
    }

    __N_CPPLOGGER__::DestroyBoostLogger(&l_pLogger);

    return 0;
}