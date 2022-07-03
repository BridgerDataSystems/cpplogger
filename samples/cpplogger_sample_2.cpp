
#include <logger.hpp>
#include <iostream>
#include <thread>

int main()
{
    __N_CPPLOGGER__::CPPLOGGER_INIT l_init{};
    l_init.severity = __N_CPPLOGGER__::SEVERITY_TRACE | __N_CPPLOGGER__::SEVERITY_INFO | __N_CPPLOGGER__::SEVERITY_ERROR;
    INITLOGGER(l_init)

    {
        LOG_TRC("text is: %s - %d", "text", 1);
        LOG_INF("text is: %s - %d", "text", 1);
        LOG_DBG("text is: %s - %d", "text", 1);
        LOG_WRN("text is: %s - %d", "text", 1);
        LOG_ERR("text is: %s - %d", "text", 1);
        LOG_FAT("text is: %s - %d", "text", 1);
    }

    for (int i = 0; i < 10; i++)
    {
        auto l_thread = std::thread([](){
            LOG_TRC("text is: %s - %d", "text", 1);
            LOG_INF("text is: %s - %d", "text", 1);
            LOG_DBG("text is: %s - %d", "text", 1);
            LOG_WRN("text is: %s - %d", "text", 1);
            LOG_ERR("text is: %s - %d", "text", 1);
            LOG_FAT("text is: %s - %d", "text", 1);
        });

        //system("pause");
        l_thread.join();
    }

    return 0;
}