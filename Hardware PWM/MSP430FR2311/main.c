#include <msp430.h>
#include <msp430fr2311.h>

unsigned int x = 500;
unsigned int y = 0;
unsigned int count = 0;
void initializeTimer();
void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  //Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;
    P2DIR |= BIT0;         //P2.0 to output
    P2SEL0 |= BIT0;
    P2SEL1 &= ~BIT0;

    P1OUT&=~BIT0;
    P1DIR &=~(BIT1);    //set P1.3 input
    P1REN|=BIT1;    //enable pull-up resistor
    P1OUT|=BIT1;
    P1IE |=BIT1;    //enable interrupt on P1.1
    P1IES |= BIT1;
    P1IFG &=~(BIT1);    //clear flag

    initializeTimer();

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE); //Enable interrupts
}

#pragma vector = TIMER0_B0_VECTOR
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
    TB1CCR0 = 1000-1;   //Set max value
    TB1CCTL1 = OUTMOD_7;    //Set output
    TB1CCR1 = 500-1;
    TB1CTL = TBSSEL_2 + MC_1;   //SMCLK/up

    TB0CCR0 = 4000;     //
    TB0CCTL0 |= CCIE;   //Set compare
    TB0CTL |= TBSSEL_2 + MC_1;  //SMCLK/up

}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P1OUT ^= BIT0;      //toggle LED
    if (!(P1IN & BIT1))
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
            TB1CCR1 = x - 1;
        }
    }
    P1IES ^= BIT1; //Allow for redo
    P1IFG &= ~BIT1; // clear flag
}
