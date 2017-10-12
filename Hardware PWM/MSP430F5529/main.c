#include <msp430.h>
#include <msp430f5529.h>

unsigned int x = 500;
unsigned int y = 0;
unsigned int count = 0;
void initializeTimer();
void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  //Stop watchdog timer

    P1DIR |= BIT2;
    P4DIR |= BIT7;       //P4.7 to output
    P1SEL |= BIT2;
    P4OUT &= ~BIT7;

    P1OUT&=~BIT0;
    P2DIR &=~(BIT1);    //set P2.1 input
    P2REN|=BIT1;    //enable pull-up resistor
    P2OUT|=BIT1;
    P2IE |=BIT1;    //enable interrupt on P2.1
    P2IES |= BIT1;
    P2IFG &=~(BIT1);    //clear flag

    initializeTimer();

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE); //Enable interrupts
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void debounce_timer(void)
{
    if (y == 1)
    {
        if (count == 75)
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
    TA0CCR0 = 1000 - 1;     //Set max value
    TA0CCTL1 = OUTMOD_7;    //Set output
    TA0CCR1 = 500 - 1;
    TA0CTL = TASSEL_2 + MC_1;   //SMCLK/up

    TA1CCR0 = 4000;
    TA1CCTL0 |= CCIE;   //Set for compare
    TA1CTL |= TASSEL_2 + MC_1;  //SMCLK/up
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void)
{
    P4OUT ^= BIT7;      //toggle LED
    if (!(P2IN & BIT1))
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
    P2IES ^= BIT1;
    P2IFG &= ~BIT1; //Clear flag
}
