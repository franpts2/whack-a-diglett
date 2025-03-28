#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL){
    return 1;
  }
  *lsb = val &0xff;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb==NULL) {
    return 1;
  }

  *msb = val >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t n;
  if (sys_inb(port,&n)!=OK || value==NULL) {
    return 1;
  }
  *value = n &0xff;
  return 0;
}
