#ifndef TELEMETRY_TASK_HPP
#define TELEMETRY_TASK_HPP

#include "freertos/queue.h"
#include "transport/transport_manager.hpp"

void telemetry_task_start(QueueHandle_t queue, TransportManager& transportManager);

#endif