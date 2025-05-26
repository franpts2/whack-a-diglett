#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include "kbc.h"
#include "i8042.h"

extern uint8_t scancode;
int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();
void (kbc_ih)();
int keyboard_restore();


#endif
