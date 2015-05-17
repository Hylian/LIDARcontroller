#include <msp430.h> 

/*
 * LIDARcontroller:
 * interfaces with the serial port of an XV-11 LIDAR module
 * controls the motor speed via PWM and reports data as an SPI slave
 *
 * Author: Edward Shin
 */

const unsigned long smclk_freq = 16000000;
const unsigned long bps = 115200;


//Serial Debug Variables
volatile char line[11];
volatile char counter = 0;


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /* Use Calibration values  for 16MHz    Clock   DCO*/
    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    /*
     *  UART Configuration
     */

    /* Configure Pin Muxes for P1.1 RX; P2.2 TX */
    P1DIR = (~BIT1) & BIT2;


    P1SEL = BIT1 | BIT2;
    P1SEL2 = BIT1 | BIT2;

    UCA0CTL1 = UCSWRST; // Put UCA0 in reset for configuration

    UCA0CTL1 |= UCSSEL_2; // Use SMCLK

    UCA0CTL0 = 0; // No parity, LSB first, 8 bits, one stop bit, UART (async)


    /* Configure for 115200 baud @ 16 MHz */

	/*
    const unsigned long brd = (smclk_freq + (bps >> 1)) / bps; // Bit rate divisor
    UCA0BR0 = (brd >> 4) & 0xFF; // Low byte of whole divisor
    UCA0BR1 = (brd >> 12) & 0xFF; // High byte of whole divisor
    UCA0MCTL = ((brd << 4) & 0xF0) | UCOS16;
	 */

    /* Configuration Values from Table 15-4 of Family Datasheet */

    UCA0BR0 = 138;
    UCA0BR1 = 0;
    UCA0MCTL = (7 << 1) | UCOS16;

    UCA0CTL1 &= ~UCSWRST; // Take UCA0 out of reset


    /*
     * PWM Configuration
     */

    P2DIR |= BIT6; // Pin 2.6 Output

    P2SEL = BIT6; // Pin 2.6 to TA0.1 for PWM

    TA0CCR0 = 1000; // PWM Period
    TA0CCTL1 = OUTMOD_7; // CCR1 reset/set
    TA0CCR1 = 450; // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1; // SMCLK, up mode

    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

    for(;;){}

	return 0;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	char temp;

	temp = UCA0RXBUF;

	if(temp == '\n' || counter > 10)
	{
		counter = 0;
	}

	if(temp == 0xFA || 1)
	{
		TA0CCR1 = 0;
	}


	line[counter++] = temp;
}




