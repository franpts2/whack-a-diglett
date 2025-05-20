#include "kbc.h"
#include <stdint.h>
#include <lcf.h>

int (read_KBC_status)(uint8_t* status) {
    if (util_sys_inb(KBC_STATUS_REG, status) != 0) {
        return 1; 
    }
    return 0;
}

int read_KBC_output(uint8_t port, uint8_t *output, uint8_t mouse) {
    uint8_t status;
    for (int attempts = 0; attempts < 10; attempts++) {
        if (read_KBC_status(&status) != 0) {
            printf("Error: Status not available!\n");
            return 1;
        }
        if (status & BIT(0)) { 
            if (util_sys_inb(port, output) != 0) {
                printf("Error: Could not read output!\n");
                return 1;
            }
            if (status & (BIT(7) | BIT(6))) { 
                printf("Error: %s error!\n", (status & BIT(7)) ? "Parity" : "Timeout");
                return 1;
            }
            if ((mouse && !(status & BIT(5))) || (!mouse && (status & BIT(5)))) {
                printf("Error: %s output not found\n", mouse ? "Mouse" : "Keyboard");
                return 1;
            }
            return 0;
        }
        tickdelay(micros_to_ticks(20000));
    }
    return 1; 
}

int (write_KBC_command)(uint8_t port, uint8_t commandByte) {
    uint8_t status;
    for (int attempts = 0; attempts < MAX_ATTEMPS; attempts++) {
        if (read_KBC_status(&status) != 0) {
            printf("Error: Status not available!\n");
            return 1;
        }

        if (!(status & FULL_IN_BUFFER)) {
            if (sys_outb(port, commandByte) != 0) {
                printf("Error: Could not write commandByte!\n");
                return 1;
            }
            return 0;
        }
        tickdelay(micros_to_ticks(WAIT_KBC));
    }
    return 1; 
}
