#include <msp430.h> 
#include <msp430f5529.h>

unsigned int x = 0;
unsigned int count = 1;
void timerstart(int f);
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |=BIT0;   //set P1.0 as output
    P1DIR &=~(BIT1);    //set P1.1 input

    P1REN|=BIT1;    //enable pull-up resistor
    P1OUT|=BIT1;

    P1IE |=BIT1;    //enable interrupt for P1.1
    P1IES |=BIT1;
    P1IFG &=~(BIT1);    //clear flag

    timerstart(100);    //set timer A with 100Hz

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE);  //Enable interrupts

}

void timerstart(int f) //Call function
{
    int n;
    TA0CCTL0 = CCIE;    //Enable interrupt
    TA0CTL = TASSEL_2 + MC_1 + ID_2;     //SMCLK
    n = 250000 / f;
    TA0CCR0 = n;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A(void) //interrupt
{
    if (x == 1)
    {
        if (count < 15)
        {
            count++;
        }

        else
        {
            x = 0;
            count = 1;
        }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    if (x == 0)
    {
        x = 1;      //set high
        P1OUT ^= BIT0;   //toggle LED
        while (!(P1IN & BIT1));
    }
    P1IFG &= ~BIT1; //clear flag
}
