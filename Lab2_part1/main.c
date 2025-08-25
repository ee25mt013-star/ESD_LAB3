#include <stdint.h>
#include "tm4c123gh6pm.h"

int main(void) {
    // 1. Enable clock to Port F
    SYSCTL_RCGCGPIO_R |= 0x20;
    while ((SYSCTL_PRGPIO_R & 0x20) == 0); // wait until Port F is ready

    // 2. Unlock PF0 if needed (for SW2), here we use SW1 so PF4 is fine
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x1F;      // allow changes to PF4-0

    // 3. Configure directions
    GPIO_PORTF_DIR_R |= 0x02;     // PF1 (Red LED) as output
    GPIO_PORTF_DIR_R &= ~0x10;    // PF4 (SW1) as input

    // 4. Enable digital function
    GPIO_PORTF_DEN_R |= 0x12;     // Enable PF1 and PF4

    // 5. Enable pull-up on PF4
    GPIO_PORTF_PUR_R |= 0x10;

    while (1) {
        if ((GPIO_PORTF_DATA_R & 0x10) == 0) {
            // SW1 pressed → turn on red LED
            GPIO_PORTF_DATA_R |= 0x02;
        } else {
            // SW1 not pressed → turn off red LED
            GPIO_PORTF_DATA_R &= ~0x02;
        }
    }
}
