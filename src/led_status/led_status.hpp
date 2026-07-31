#ifndef LED_STATUS_HPP
#define LED_STATUS_HPP

typedef enum {
    LED_STATE_SEARCH,
    LED_STATE_CONNECTING,
    LED_STATE_CONNECTED,
} led_state_t;

#ifdef __cplusplus
extern "C" {
#endif

void led_status_init(void);
void led_status_set(led_state_t new_state);

#ifdef __cplusplus
}
#endif

#endif
