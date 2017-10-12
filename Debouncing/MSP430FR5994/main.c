#include <msp430.h> 
#include <msp430fr5994.h>

unsigned int x = 0;
unsigned int count = 1;
void timerstart(int f);
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |=BIT0;   //set P1.0 as output
    P5DIR &=~(BIT6);     //set P1.1 input
    P5REN|=BIT6;    //enable pull-up resistor
    P5OUT|=BIT6;
    P5IE |=BIT6;    //enable interrupt for P1.1
    P5IES |=BIT6;
    P5IFG &=~(BIT6);    //clear flag

    timerstart(100);    //initialize timer A with 100Hz freq

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE);   //Enable interrupts
}

void timerstart(int f)  //Call function
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
        if (count < 25)
        {
            count++;
        }

        else    //reset
        {
            x = 0;
            count = 1;
        }
    }
}

#pragma vector=PORT5_VECTOR
__interrupt void PORT_5(void)
{
    if (x == 0)
    {
        x = 1;    //set high
        P1OUT ^= BIT0;  //toggle LED
    }
    P5IFG &= ~BIT6;     //clear flag
}
