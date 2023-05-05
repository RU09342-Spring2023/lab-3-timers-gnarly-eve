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

int buttonPressDuration = 0;

void gpioInit();
void timerInit();

void main() {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    PM5CTL0 &= ~LOCKLPM5;

   __bis_SR_register(LPM3_bits | GIE);
}


void gpioInit() {

    P1DIR |= BIT0;                          // Set P1.0 to output direction
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state

    P2DIR &= ~BIT3;
    P2REN |= BIT3;                          // Enable the pull up/down resistor for Pin 2.3
    P2OUT |= BIT3;                          // Pull-up resistor
    P2IES |= BIT3;                          // P2.3 High --> Low edge
    P2IFG &= ~BIT3;
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    P4DIR &= ~BIT1;                         // Config P4.1 as input
    P4REN |= BIT1;                          // Enable the pull up/down resistor for Pin 4.1
    P4OUT |= BIT1;                          // Pull-up resistor
    P4IES |= BIT1;                          // P4.1 High --> Low edge
    P4IFG &= ~BIT1;                         // Clear P4.1 IRQ Flag
    P4IE |= BIT1;                           // P4.1 interrupt enabled
}


void timerInit() {
    TB1CCR0 = 300;  //arbitrary initial value
    TB1CTL |= TBSSEL__ACLK + MC_0 + TBCLR; // Set the clock source to ACLK, stop the timer, and clear it
    TB1CCTL0 |= CAP + CM_3; // Set Timer B0 to capture mode, set for both edges

    TB0CTL |= TBCLR;                    // Clear timer and dividers
    TB0CTL |= TBSSEL__ACLK;             // Source = ACLK
    TB0CTL |= MC__UP;                   // Mode = UP;
    TB0CCR0 = 8192;                     // Timer up to 4 Hz
    TB0CCTL0 |= CCIE;                   // Enable TB0 CCR0 Overflow IRQ
    TB0CCTL0 &= ~CCIFG;                 // Clear CCR0 Flag
}


/*
 * INTERRUPT ROUTINES
 */


// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void ISR_Port2_S2(void)
{
   if (P2IES & BIT3) {
       TB1CTL |= MC_2;
       P2IES &= ~BIT3;
   }

   else if (P2IES != BIT3) {
       TB1CTL &= ~(MC0 + MC1); //stop timer B0 to record how long the button was pressed

       //Put timer B0 value into the max timer B1 CCR0
       buttonPressDuration = TB1R; // Record the button press time
       TB0CCR0 = buttonPressDuration; // Set the max count for Timer B1 to the button press time

       TB1CTL |= TBCLR; //clear timer B0 for next button press
       P2IES |= BIT3;
   }
   P2IFG &= ~BIT3;         // Clear interrupt flag for P2.3
}



// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void ISR_Port4_S1(void)
{
    TB1CTL &= ~(MC0 + MC1); // Stop Timer B0 - just in case
    TB1CTL |= TBCLR; // Clear Timer B0
    TB0CCR0 = 8192;
    P4IFG &= ~BIT1;
}


// Timer B0 interrupt service routine
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P1OUT ^= BIT0;                  // Toggle Red LED
    TB0CCTL0 &= ~CCIFG;             // Clear CCR0 flag
}
