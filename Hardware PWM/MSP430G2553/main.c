#include <msp430.h>
#include <msp430g2553.h>

unsigned int x = 500;
unsigned int y = 0;
unsigned int count = 0;
void initializeTimer();
void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;   //Stop watchdog timer

    P1DIR |= BIT0;
    P1DIR |= BIT6;           // P1.6 to output
    P1SEL |= BIT6;
    P1SEL2 &= ~BIT6;

    P1OUT&=~BIT0;
    P1DIR &=~(BIT3);    //set P1.3 input
    P1REN|=BIT3;    //enable pull-up resistor
    P1OUT|=BIT3;
    P1IE |=BIT3;    //enable the interrupt on P1.1
    P1IES |= BIT3;
    P1IFG &=~(BIT3);    //clear flag

    initializeTimer();

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE); //Enable interrupt
}

#pragma vector = TIMER1_A1_VECTOR
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
    TA1CCTL0 |= CCIE;   //set for compare
    TA1CTL |= TASSEL_2 + MC_1;  //SMCLK/up

}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P1OUT ^= BIT0;  //toggle LED
    if (!(P1IN & BIT3))
    {
        if (y == 0)
        {
            y = 1;
            if (x >= 1000)
            {
                x = 1; //reset
            }

            else
            {
                x += 100;
            }
            TA0CCR1 = x - 1;
        }
    }
    P1IES ^= BIT3; //allow for redo
    P1IFG &= ~BIT3; // clear flag
}
