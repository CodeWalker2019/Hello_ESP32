#pragma once

#include "esp_err.h"
#include "freertos/queue.h"
#include <functional>
#include <cstdint>
#include <cstddef>

using SendTelemetryFn = std::function<void(const uint8_t*, size_t)>;

esp_err_t telemetry_task_start(QueueHandle_t queue,
                                SendTelemetryFn sendTelemetry,
                                const char* taskName = "telemetry_task");