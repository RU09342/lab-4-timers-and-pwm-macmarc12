#include <msp430.h>
#include <msp430fr2311.h>

unsigned int x = 500;
unsigned int y = 0;
unsigned int count = 0;
void initializeTimer();
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;
    P2DIR |= BIT0;

    P1DIR &=~(BIT1);
    P1REN|=BIT1;    //enable pull-up resistor
    P1OUT|=BIT1;
    P1IE |=BIT1;    //enable interrupt on P1.1
    P1IES |= BIT1;
    P1IFG &=~(BIT1);    //clear flag

    initializeTimer();

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE);  //Enable interrupts
}

void initializeTimer(void)
{
    TB0CCTL1 = CCIE;    //Enable interrupt
    TB0CCTL0 = CCIE;
    TB0CTL = TBSSEL_2 + MC_1;   //SMCLK/up
    TB0CCR0 = 1000 - 1;
    TB0CCR1 = 500 - 1;

    TB1CCR0 = 4000;
    TB1CCTL0 |= CCIE;
    TB1CTL |= TBSSEL_2 + MC_1;
}

#pragma vector = TIMER1_B0_VECTOR
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

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER_1(void) //interrupt
{
    if (x != 1)
        P1OUT |= BIT0;  // LED on
    TB0CCTL0 &= ~BIT0;
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER_0(void)
{
     if (TB0CCR1 != 1000)
         P1OUT &= ~BIT0;    //LED off
     TB0CCTL1 &= ~BIT0;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) //interrupt
{
    P2OUT ^= BIT0;
    if (!(P1IN & BIT1))
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
            TB0CCR1 = x - 1;
        }
    }
    P1IES ^= BIT1;
    P1IFG &= ~BIT1;
}
