#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <minix/sysutil.h>
#include "i8042.h"
 
#include <lcom/lcf.h>

int (read_KBC_status)(uint8_t* status);

int (read_KBC_output)(uint8_t port, uint8_t *output, uint8_t mouse);

int (write_KBC_command)(uint8_t port, uint8_t commandByte);

#endif /* _LCOM_KBC_H_ */
