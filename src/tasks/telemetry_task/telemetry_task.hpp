#ifndef TELEMETRY_TASK_HPP
#define TELEMETRY_TASK_HPP

#include "esp_err.h"
#include "freertos/queue.h"
#include "transport/transport_manager.hpp"

esp_err_t telemetry_task_start(QueueHandle_t queue, TransportManager& transportManager);

#endif