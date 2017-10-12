#include <msp430.h>
#include <msp430fr6989.h>

unsigned int x = 500;
unsigned int y = 0;
unsigned int count = 0;
void initializeTimer();
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;
    P9DIR |= BIT7;

    P9OUT &= ~BIT7;

    P1DIR &=~(BIT2);
    P1REN|=BIT2;    //enable pull-up resistor
    P1OUT|=BIT2;
    P1IE |=BIT2;    //enable interrupt on P1.2
    P1IES |= BIT2;
    P1IFG &=~(BIT2);    //clear flag

    initializeTimer();

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE); //Enable interrupts
}

void initializeTimer(void)
{
    TA0CCTL1 = CCIE;    //Enable interrupt
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC_1;   //SMCLK/up
    TA0CCR0 = 1000 - 1;
    TA0CCR1 = 500 - 1;

    TA1CCR0 = 4000;
    TA1CCTL0 |= CCIE;
    TA1CTL |= TASSEL_2 + MC_1;  //SMCLK/up
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

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_1(void)
{
    if (x != 1)
        P1OUT |= BIT0;  //LED on
    TA0CCTL0 &= ~BIT0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER_0(void)
{
     if (TA0CCR1 != 1000)
         P1OUT &= ~BIT0;    //LED off
     TA0CCTL1 &= ~BIT0;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P9OUT ^= BIT7;
    if (!(P1IN & BIT2))
    {
        if (y == 0)
        {
            y = 1;
            if (x >= 1000)
            {
                x = 1;
            }

            else
            {
                x += 100;
            }
            TA0CCR1 = x - 1;
        }
    }
    P1IES ^= BIT2;
    P1IFG &= ~BIT2;
}
