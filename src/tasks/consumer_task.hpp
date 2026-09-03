#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_log.h>
#include <functional>
#include <new>

template <typename T>
esp_err_t consumer_task_start(QueueHandle_t queue,
                               std::function<void(const T&)> onItem,
                               const char* taskName,
                               uint32_t stackSize = 2048,
                               UBaseType_t priority = 2) {
    if (queue == nullptr) {
        ESP_LOGE("ConsumerTask", "consumer_task_start called with null queue");
        return ESP_ERR_INVALID_ARG;
    }
    if (!onItem) {
        ESP_LOGE("ConsumerTask", "consumer_task_start called with null onItem");
        return ESP_ERR_INVALID_ARG;
    }
    if (taskName == nullptr) {
        ESP_LOGE("ConsumerTask", "consumer_task_start called with null taskName");
        return ESP_ERR_INVALID_ARG;
    }

    struct Args {
        QueueHandle_t queue;
        std::function<void(const T&)> onItem;
    };

    auto* args = new (std::nothrow) Args{queue, std::move(onItem)};

    if (args == nullptr) return ESP_ERR_NO_MEM;

    auto taskFn = [](void* raw) {
        auto* args = static_cast<Args*>(raw);
        T item;
        while (true) {
            if (xQueueReceive(args->queue, &item, portMAX_DELAY) == pdTRUE) {
                args->onItem(item);
            }
        }
    };

    BaseType_t res = xTaskCreate(taskFn, taskName, stackSize, args, priority, nullptr);
    if (res != pdPASS) {
        delete args;
        return ESP_FAIL;
    }
    return ESP_OK;
}
