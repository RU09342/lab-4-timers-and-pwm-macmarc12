#include <msp430.h> 
#include <msp430fr6989.h>

unsigned int x = 0;
unsigned int count = 1;
void timerstart(int f);
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |=BIT0;   //set P1.0 as output
    P1DIR &=~(BIT2);    //set P1.2 input
    P1REN|=BIT2;    //enable pull-up resistor
    P1OUT|=BIT2;
    P1IE |=BIT2;    //enable interrupt for P1.2
    P1IES |=BIT2;
    P1IFG &=~(BIT2);    //clear flag

    timerstart(100);    //Set timer A with 100Hz

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE);      //Enable interrupts
}

void timerstart(int f)      //Call function
{
    int n;
    TA0CCTL0 = CCIE;    //Enable interrupt
    TA0CTL = TASSEL_2 + MC_1 + ID_2;    //SMCLK
    n = 250000 / f;
    TA0CCR0 = n;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A(void)
{
    if (x == 1)
    {
        if (count < 15)
        {
            count++;
        }

        else    //reset
        {
            buttonPress = 0;
            count = 1;
        }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) //interrupt
{
    if (x == 0)
    {
        x = 1;      //set high
        P1OUT ^= BIT0;      //toggle LED
        while (!(P1IN & BIT2));
    }
    P1IFG &= ~BIT2;     //clear flag
}
