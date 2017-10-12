# Software Debouncing
The objective in this section is to integrate debouncing into our code.
Debouncing is the means by which we reduce feedback after releasing a switch or button that can have side effects on our code.
We will need all previously learned resources to complete this section.

## Getting started
This can be done with several interrupts as well as timer manipulation.
```c
P1IE |=BIT2;
P1IES |=BIT2;
P1IFG &=~(BIT2);
```
These lines enable interrupt, look for falling edge, and clear the interrupt flag respectively.

### Acknowledgement
I had some trouble with this section so I used the resources of the internet and classmates to help fill in holes I had in my knowledge and code.
