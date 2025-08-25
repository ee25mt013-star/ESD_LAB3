#include <stdint.h>
#include "tm4c123gh6pm.h"

void delayMs(int n) {
    volatile int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 3180; j++) {}  // ~1ms delay at 16 MHz
    }
}

int main(void) {
    // Enable clock to Port F
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0);

    // Unlock PF0 if needed (not used here, we’ll use SW1 on PF4)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x1F;

    // PF1, PF2, PF3 = output (LEDs), PF4 = input (SW1)
    GPIO_PORTF_DIR_R |= 0x0E;   // 0000 1110 → PF1–3 output
    GPIO_PORTF_DIR_R &= ~0x10;  // clear PF4 input
    GPIO_PORTF_DEN_R |= 0x1E;   // enable digital for PF1–4
    GPIO_PORTF_PUR_R |= 0x10;   // enable pull-up on PF4

    int state = -1;  // 0=Red, 1=Green, 2=Blue

    while(1) {
        // Wait for button press (active low)
        if ((GPIO_PORTF_DATA_R & 0x10) == 0) {
            // Debounce delay
            delayMs(20);
            if ((GPIO_PORTF_DATA_R & 0x10) == 0) {
                // Advance state
                state = (state + 1) % 3;

                // Update LED output
                if (state == 0) {
                    GPIO_PORTF_DATA_R = 0x02; // Red
                } else if (state == 1) {
                    GPIO_PORTF_DATA_R = 0x08; // Green
                } else {
                    GPIO_PORTF_DATA_R = 0x04; // Blue
                }

                // Wait until button released (so only 1 toggle per press)
                while ((GPIO_PORTF_DATA_R & 0x10) == 0);
                delayMs(20); // debounce release
            }
        }
    }
}
