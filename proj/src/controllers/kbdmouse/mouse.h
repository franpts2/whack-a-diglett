#ifndef _MOUSE_H_
#define _MOUSE_H_

int(mouse_test_packet)(uint32_t cnt);

int(mouse_test_async)(uint8_t idle_time);

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance);

#endif // _MOUSE_H_