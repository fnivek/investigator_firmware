/*
 * encoder.c
 *
 *  Created on: Oct 9, 2015
 *      Authors: Daniel Jensen
 *      		 Kevin French
 */
#include "encoder.h"

// Globals
int32_t encodercount1 = 0;		//these longs keep track of motor position for left and right respectively
int32_t encodercount2 = 0;
uint16_t compare1old = 0;		//These are registers used to hold the CC register value for speed calculation
uint16_t compare2old = 0;
uint16_t diff1 = 0;				//This is the number of clock ticks that have occured in 1/898 of a revolution
uint16_t diff2 = 0;
uint16_t timer1intcode;
uint8_t ovf_count_1 = 0;
uint8_t ovf_count_2 = 0;

//P2.1 is the left encoder signal
//P2.4 is the right encoder signal
void Init_Encoder ()
{

	P2DIR &= 0xED; 		//Set P2.1 and 2.4 as inputs CCI1A and CCI2A
	P2SEL |= 0x12; 		//Each pin needs function code 01
	P2SEL2 &= 0xED;
	TA1CTL = 0x02E0 | TAIE;	//Make TA1 a 16bit counter /8
	TA1CCTL1 = 0x4910;	//rising edge use ccIxA compare capture enable interrupts
	TA1CCTL2 = 0x4910;

}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void ENCODER2(void)
{
	timer1intcode = TA1IV;				//Reading this register clears the interrupt flag
	// Overflow interrupt
	if(timer1intcode == TA1IV_TAIFG)
	{
		++ovf_count_1;
		++ovf_count_2;


		// If we've overflowed twice assume the motors are stoped
		if(ovf_count_1 >= 2)
		{
			diff1 = 0xFFFF;
		}

		if(ovf_count_2 >= 2)
		{
			diff2 = 0xFFFF;
		}

		//TA1CTL &= ~TAIFG;

		return;

	}


	if(timer1intcode == TA1IV_TACCR1)
	{
		//Interrupt for left encoder
		__delay_cycles(1);
		if(ldir_active == 1)
		{
			encodercount1++; 			//Dir = 1 means forward, this conditional keeps track of "position"
		}
		else
		{
			encodercount1--;
		}

		diff1 = TA1CCR1 - compare1old; //Difference is for speed calculation

		compare1old = TA1CCR1;
		ovf_count_1 = 0;
		return;
	}

	if(timer1intcode == TA1IV_TACCR2)
	{
		//Interrupt for the right encoder
		__delay_cycles(1);
		if(rdir_active == 1)
		{
			encodercount2++; 			//Dir = 1 means  //******* USED TO BE ++ *********
		}
		else
		{
			encodercount2--;
		}

		diff2 = TA1CCR2 - compare2old; 	//This difference will be used for speed computation

		compare2old = TA1CCR2;
		ovf_count_2 = 0;
		return;
	}


}
