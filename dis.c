
/*Directed Independent Study: Smart Lock-and-Key LED Motion System Using MSP430 Microcontroller and GY-61 ADXL335 Accelerometer
•	Assembled and programmed circuit in which MSP430G2553 mixed-signal microcontroller received multi-directional input from 3-axis analog output accelerometer
•	Circuit records passcodes of up to three movements along negative and positive X-axis and/or Y-axis; Z-axis is used to save and enter said passcodes
    1.	Movements along axes indicated by different colored LEDs (X-axis: Blue, Y-axis: Green, Z-axis: Yellow)
    2.	Learning mode is activated via photosensor (Blue “Learn” LED turns on)
    3.	Passcode of choice is entered and saved by accelerating circuit along Z-axis (Yellow LEDs flash, Learn LED turns off)
    4.	User records passcode of up to three movements by accelerating circuit along X or Y-axes, then enters passcode by accelerating circuit along Z-axis (Yellow LEDs flash)
•	Circuit “unlocks” (Green LED flashes) if entered passcode equals saved passcode, remains “locked” (Red LED flashes) if entered passcode does not equal saved passcode
•	Circuit programmed entirely in C using Code Composer Studio
•   Study supervised by Dr. Bassem Alhalabi during Summer 2019 semester at Florida Atlantic University*/
//***********************************************************************
//******** Name: Carlos Mateo
//******** Course: COT6900 - Directed Independent Study - Embedded Systems
//******** Professor: Dr. Bassem Alhalabi



#include <msp430.h>

int i = 0 ;
int z = 0, zstart = 0;
int y = 0, ystart = 0;
int x = 0, xstart = 0;
int light = 0, lightroom = 0;
int learning = 0;
int activate = 0;
unsigned long int entercode = 0;
unsigned long int savedcode = 0;
int mask = 0x0F;
int ADCReading [5];

//Analog Read Function Prototypes
void ConfigureAdc(void);
void getanalogvalues();

//Main Program
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                   	   		  //Stop WDT
    P1OUT = 0;
    P2OUT = 0;
    P1REN = 0;
    P2REN = 0;
    P1DIR |= (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);              //Set bits 1.3, 1.4, 1.6, 1.7 as outputs
    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT4 | BIT5);			  //Set bits 2.0, 2.1, 2.2, 2.4, 2.5 as outputs

    ConfigureAdc();

       //Reading the initial axis values and light value, then assigning them to zstart, xstart, ystart, lightroom (respectively)
       __delay_cycles(250);
       getanalogvalues();
       zstart = z; xstart = x; ystart = y; lightroom = light;
       __delay_cycles(250);


for (;;)
{
        //Reading z, x, and y and light repeatedly at the beginning of the main loop
        getanalogvalues();


        //Positive X-axis readings detected by LED on 1.4, Negative X-axis readings detected by LED on 2.0
        if(x < xstart * 1.03 && x > xstart * 0.97) {P1OUT &= ~BIT4; P2OUT &= ~BIT0;__delay_cycles(5000);}		//Both LEDs off when in dead zone
        else
        {
            if(x <= xstart * 0.97) {P2OUT |=  BIT0; __delay_cycles(200000); entercode = entercode<<1; entercode |=  BIT0; P2OUT &= ~BIT0; __delay_cycles(100000);}   //Flashes LED on/off if negative, shifts entercode by 1
            if(x >= xstart * 1.03) {P1OUT |=  BIT4; __delay_cycles(200000); entercode = entercode<<1; entercode &= ~BIT0; P1OUT &= ~BIT4; __delay_cycles(100000);}	//Flashes LED on/off if positive, shifts entercode by 2
        }



        //Positive Y-axis readings detected by LED on 1.5, Negative Y-axis readings detected by LED on 1.6
        if(y < ystart * 1.03 && y > ystart * 0.97) {P1OUT &= ~BIT5; P1OUT &= ~BIT6; __delay_cycles(5000);}		//Both LEDs off when in dead zone
        else
        {
        	if(y <= ystart * 0.97) {P1OUT |=  BIT6; __delay_cycles(200000); entercode = entercode<<2; entercode |=  BIT0; P1OUT &= ~BIT6; __delay_cycles(100000);}	//Flashes LED on/off if negative, shifts entercode by 3
        	if(y >= ystart * 1.03) {P1OUT |=  BIT5; __delay_cycles(200000); entercode = entercode<<2; entercode &= ~BIT0; P1OUT &= ~BIT5; __delay_cycles(100000);}	//Flashes LED on/off if positive, shifts entercode by 4
        }

        //Z-axis readings detected by LED on 1.7 (negative) and 2.5 (positive): both flash to indicate activation
        if(z > zstart * .99 && z < zstart * 1.1) {P2OUT &= ~BIT5; P1OUT &= ~BIT7; __delay_cycles(5000);}		//Both LEDs off when in dead zone
        else
        {
            if(z >= zstart * 1.1) {P2OUT |=  BIT5; P1OUT |=  BIT7;  __delay_cycles(200000); activate = 1; P2OUT &= ~BIT5; P1OUT &= ~BIT7; __delay_cycles(100000);}	//Set activate flag to 1. Flash both Z-axis LEDs
            if(z <= zstart * 0.995){P2OUT |=  BIT5; P1OUT |=  BIT7; __delay_cycles(200000); activate = 1; P2OUT &= ~BIT5; P1OUT &= ~BIT7; __delay_cycles(100000);}	//Set activate flag to 1. Flash both Z-axis LEDs
        }

        //determine if light input is blocked, and set the learning flag to 1
        if (light > lightroom * 2) {P2OUT |= BIT2; __delay_cycles(200000); learning =1;__delay_cycles(100000);}

        //if in learning and in activate, the end of learning has been reached. Entercode is saved into savedcode and flags are cleared
        if ((learning==1) && (activate==1)) {__delay_cycles(200000); entercode &=mask; savedcode=entercode; entercode=0; learning =0; activate=0; P2OUT &= ~BIT2; __delay_cycles(100000);}
        //if not in learning and in activate, and entercode matches savedcode, then send approval signal. Flags are cleared
        if ((learning==0) && (activate==1) && (entercode == savedcode)) {P2OUT |= BIT1; __delay_cycles(2000000); entercode = 0; activate=0; P2OUT &= ~BIT1; __delay_cycles(100000);}
        //if not in learning and in activate, and entercode does not match savedcode, then send denial signal. Flags are cleared
        if ((learning==0) && (activate==1) && (entercode != savedcode)) {P2OUT |= BIT4; __delay_cycles(2000000); entercode = 0; activate=0; P2OUT &= ~BIT4; __delay_cycles(100000);}

}
}

//Configures analog values
void ConfigureAdc(void)
{
   ADC10CTL1 = INCH_3 | CONSEQ_1;             	//A3 + A2 + A1 + A0, single sequence
   ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;
   while (ADC10CTL1 & BUSY);
   ADC10DTC1 = 0x04;                          	//4 conversions
   ADC10AE0 |= (BIT3 | BIT2 | BIT1 | BIT0);          	//ADC10 option select
}

//Retrieves analog values
void getanalogvalues()
{
 i = 0; y = 0; z = 0; x = 0; light = 0;        				//Set all analog values to zero
  for(i=1; i<=5 ; i++)                     		//Read all three analog values 5 times each and average
  {
    ADC10CTL0 &= ~ENC;
    while (ADC10CTL1 & BUSY);                 	//Wait while ADC is busy
    ADC10SA = (unsigned)ADCReading;       		//RAM Address of ADC Data, must be reset every conversion
    ADC10CTL0 |= (ENC | ADC10SC);             	//Start ADC Conversion
    while (ADC10CTL1 & BUSY);                 	//Wait while ADC is busy
    light += ADCReading[0];
    x += ADCReading[1];                   		//Sum all 5 readings for the three variables
    y += ADCReading[2];
    z += ADCReading[3];
  }

 z = z/5; x = x/5; y = y/5; light = light/5;	//Average the 5 reading for the four variables

}


#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}
