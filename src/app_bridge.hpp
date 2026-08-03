#ifndef APP_BRIDGE_HPP
#define APP_BRIDGE_HPP

#ifdef __cplusplus
extern "C" {
#endif

void app_bridge_init(void);
void app_bridge_connection_manager_worker(void);

#ifdef __cplusplus
}
#endif

#endif