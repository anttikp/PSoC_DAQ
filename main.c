/* 
create: 015.4.2019
autor: Miriam Nold
PSoC DAQ
Exercise 3
*/

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include <stdlib.h>

long int stemp = 0;		// the sum of added data that a gave from the EzADC
long int ntemp = 0;			// the number of added data 
long int value = 0;		// the average of the data
int index = 0;			// the number of calculated data
char intToString[15];	//


// a function for the output on the lcd to control the reaction of the motor
void lcdOutput(int y,int x,int content){
	char intToString[15];
	itoa(intToString,content,10);
	LCD_Position(y,x);
	LCD_PrString(intToString);
}


void main(void)
{
	M8C_EnableGInt ; // enable global interrupts 
	
  //  PGA_Start(PGA_MEDPOWER);
	  
	PGA_Start(PGA_HIGHPOWER);      // Turn on the PGA
    LPF2_Start(LPF2_HIGHPOWER);    // Turn on the LPF
	
	Counter8_WritePeriod(0x07);			// set period to eight clocks
    Counter8_WriteCompareValue(0x03);   // generate a 50% duty cycle
    Counter8_EnableInt();               // ensure interrupt is enabled
    Counter8_Start();                   // start the counter!
	
	EzADC_Start(EzADC_HIGHPOWER);       // Apply power to ADC 
    EzADC_SetBufferGain(EzADC_G1_00);   // Set Gain 
    EzADC_GetSamples(0);                // Have ADC run continuously
	
	LCD_Start();                // Initialize LCD
	LCD_Position(0,4);          // Place LCD cursor at row 0, col 4
	LCD_PrCString("Celsius");   // Print "Celsius" on the LCD
	LCD_Position(1,0);          // Place LCD cursor at row 1, col 0
	LCD_PrCString("Index:");	// Print "PSoC LCD" on the LCD
	
	SleepTimer_Start();
	SleepTimer_SetInterval(SleepTimer_8_HZ);   // Set interrupt to a
	SleepTimer_EnableInt();                    // 8 Hz rate
	
	while (1){
		
		if (ntemp >= 50){			//by 50 data the average will be calculated
			
			value = stemp/ntemp;	// calculate average of 50 data
			ntemp = 0;				// set the number and addition to zero
			stemp = 0;
			index++;				// increment the number of calculated data
			
			value = (value*400)/(4096);	// calculate the temperature in celsius
										// T = (5V * Value)/(4096 * 0,01V)
										//4096 couse of the 12 bit by the ADC

			lcdOutput(0,0,(int)value);	// call the function for LCD output
			lcdOutput(1,7,index);
			SleepTimer_SyncWait(4, SleepTimer_WAIT_RELOAD);		//wait 0,5s for the next calculation
		}
	}
}


//called when ADC conversion is ready
void CalcMean(void){

	ntemp++;
	stemp += EzADC_iGetDataClearFlag();  
	
}