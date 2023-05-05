/*
 * Part3.c
 *
 *  Created on: Feb 12, 2023
 *      Author: Evelyn LL
 *       
 
 *      build a system which has the LED blinking speed 
 *      controlled by how long you hold down a button. 
 *      The system should boot up and begin blinking 
*        at a specific speed 
 */


#include <msp430.h>


#define ARMED_STATE 0
#define WARNING_STATE 1
#define ALERT_STATE 2


int main() {
    WDTCTL = WDTPW | WDTHOLD;             // Stop watchdog timer

    // Initialization:
    P6OUT &= ~BIT6;                       // Clear P6.6 output latch for a defined power-on state
    P6DIR |= BIT6;                        // Set P6.6 to output direction

    P1OUT &= ~BIT0;                       // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                        // Set P1.0 to output direction

    P2DIR &= ~BIT3;                       // Configure Pin 2.3 to an input
    P2REN |= BIT3;                        // Enable the pull up/down resistor for Pin 2.3
    P2OUT |= BIT3;                        // While configured as an input, P4OUT controls whether
                                          // the resistor is a pull up or pull down

    P4DIR &= ~BIT1;                       // Configure Pin 4.1 to an input
    P4REN |= BIT1;                        // Enable the pull up/down resistor for Pin 4.1
    P4OUT |= BIT1;                        // While configured as an input, P4OUT controls whether
                                          // the resistor is a pull up or pull down


    char state = 0;                       // Initialize state to Armed State
    int count = 0;                        // Initialize counter to 0

    PM5CTL0 &= ~LOCKLPM5;                 // Disable the GPIO power-on default high-impedance mode
                                          // to activate previously configured port settings

    // Prototypes:
    void delay(int ms);
    void blinkGreenLED();
    void blinkRedLED();


    while(1)
    {
      switch (state) {
        case 0:                           // Armed State
            (P1OUT &= BIT0) == 0;         // Toggle off P1.0
            if (!(P2IN & BIT3)) {         // If P2.3 pressed, go to Warning State
               state = 1;
               break;
            }
            else {
                blinkGreenLED();          // If P2.3 not pressed, blink Green LED
                break;
            }
        case 1:                           // Warning State
            P6OUT &= ~BIT6;               // Toggle off P6.6
            if (P2IN & BIT3) {            // If P2.3 let go, reset counter and go to Armed State
                count = 0;
                state = 0;
                break;
            }
            else if (count >= 10) {       // If P2.3 held down for more than
                state = 2;                // 10 (seconds from blink Red LED), go to Alert State
                break;
            }
            else {                        // If P2.3 still held down, blink Red LED and increment counter
                blinkRedLED();
                count++;
                break;
            }
        case 2:                           // Alert State
            P1OUT |= BIT0;                // Toggle on Red LED
            if (!(P4IN & BIT1)) {         // If P4.1 pressed, toggle off Red LED, reset counter,
                P1OUT &= ~BIT0;           // and go to Armed State
                count = 0;
                state = 0;
                break;
            }
       }
     }
}


// Definitions:
void delay(int ms) {                      // Function to convert microseconds to milliseconds
    int i;
    for (i = 0; i < ms; i++) {
        __delay_cycles(1000);             // Delay for 1000000us = 1000ms
    }
}

void blinkGreenLED() {                    // Function to blink Green LED once every 3s
    P6OUT ^= BIT6;                        // Toggle off P6.6
    delay(1500);                          // Delay for 1.5s
    P6OUT ^= BIT6;                        // Toggle on P6.6
    delay(1500);                          // Delay for 1.5s
}

void blinkRedLED() {                      // Function to blink RED LED once every 1s
    P1OUT ^= BIT0;                        // Toggle off P1.0
    delay(500);                           // Delay for 0.5s
    P1OUT ^= BIT0;                        // Toggle off P1.0
    delay(500);                           // Delay for 0.5s
}




