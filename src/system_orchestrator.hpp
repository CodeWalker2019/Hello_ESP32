#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYSTEM_ORCHESTRATOR_LOGS        "SYSTEM_ORCHESTRATOR_LOGS"

esp_err_t system_orchestrator_init(void);


#ifdef __cplusplus
}
#endif
