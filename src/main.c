#include "app_bridge.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    app_bridge_init();

    while (1) {
        app_bridge_connection_manager_worker();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}