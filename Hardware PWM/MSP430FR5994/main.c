#include <msp430.h>
#include <msp430fr5994.h>

unsigned int x = 500;
unsigned int y = 0;
unsigned int count = 0;
void initializeTimer();
void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT1;
    P1DIR |= BIT0;          // P1.0 to output
    P1SEL0 |= BIT0;
    P1SEL1 &= ~BIT0;

    P1OUT&=~BIT1;   //Clear LED

    P5DIR &=~(BIT6);     //Set P5.6 input
    P5REN|=BIT6;    //enable pull-up resistor
    P5OUT|=BIT6;
    P5IE |=BIT6;    //enable the interrupt on P1.1
    P5IES |= BIT6;
    P5IFG &=~(BIT6);    //clear interrupt flag

    initializeTimer();

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE);  //Enable interrupts
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void debounce_timer(void)
{
    if (y == 1)
    {
        if (count == 50)
        {
            y = 0;
            count = 0;
        }

        else
        {
            count++;
        }
    }
}

void initializeTimer(void)
{
    TA0CCR0 = 1000-1;   //Set max value
    TA0CCTL1 = OUTMOD_7;    //Set output
    TA0CCR1 = 500-1;
    TA0CTL = TASSEL_2 + MC_1;   //SMCLK/up

    TA1CCR0 = 4000;
    TA1CCTL0 |= CCIE;    //Set for compare
    TA1CTL |= TASSEL_2 + MC_1;  //SMCLK/up
}

#pragma vector=PORT5_VECTOR
__interrupt void PORT_5(void)
{
    P1OUT ^= BIT1;  //toggle LED
    if (!(P5IN & BIT6))
    {
        if (y == 0)
        {
            y = 1;
            if (x >= 1000)
            {
                x = 1;  //reset
            }

            else
            {
                x += 100;
            }
            TA0CCR1 = x - 1;
        }
    }
    P5IES ^= BIT6;  //Allow for redo
    P5IFG &= ~BIT6;     // clear flag
}
