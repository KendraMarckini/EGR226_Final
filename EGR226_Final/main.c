/* Name: Kendra Marckini
 * Date: 11/11/2018
 * Class: EGR 226-902
 * Program: Final Project */

//Preprocessor directives
#include "msp.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "LCD_Code.h"
//#include "Timer_Codes.h"

//Macros
#define DOT_HALF    3000000
#define HALF        2400000
#define DOT_QUAR    1800000
#define QUAR        1200000
#define EIGHTH      600000
#define SIXTH       300000
#define BREATH_TIME 50000
#define REST        0
#define C6          1046.502
#define B5          987.7666
#define A5          880.0000
#define G5          783.9909
#define F5          698.4565
#define E5          659.2551
#define D5          587.3295
#define C5          523.2511
#define G4          391.9954
#define E4          329.6276
#define D4          293.6648
#define C4          261.6256
#define B3          246.9417
#define A3          220.0000
#define G3          195.9977
#define F3_SHARP    184.9972
#define MAX_NOTE    74

//Global variables
volatile uint32_t second;
int note = 0;
int breath = 0;
float music_note_sequence[][2] = {
                                 //Chorus, 38 notes
                                 {E5,EIGHTH},{G5,EIGHTH},{G5,EIGHTH},{C6,QUAR},{B5,QUAR},
                                 {G5,QUAR},{G5,QUAR},{A5,SIXTH},{G5,SIXTH},{F5,QUAR},
                                 {0,QUAR},{E5,EIGHTH},{G5,EIGHTH},{G5,EIGHTH},{C6,QUAR},
                                 {B5,QUAR},{G5,QUAR},{E5,EIGHTH},{D5,QUAR},{0,HALF},
                                 {E5,EIGHTH},{G5,EIGHTH},{G5,EIGHTH},{C6,QUAR},{B5,QUAR},
                                 {G5,QUAR},{G5,QUAR},{A5,SIXTH},{G5,SIXTH},{F5,QUAR},
                                 {0,QUAR},{E5,EIGHTH},{D5,EIGHTH},{E5,QUAR},{D5,EIGHTH},
                                 {C5,EIGHTH},{C5,DOT_HALF},{0,QUAR},
                                 //Post chorus, 35 notes
                                 {A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},{C4,DOT_QUAR},
                                 {A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},{D4,DOT_QUAR},
                                 {E4,EIGHTH},{G4,EIGHTH},{E4,SIXTH},{D4,SIXTH},
                                 {C4,EIGHTH},{D4,EIGHTH},{G3,DOT_QUAR},{D4,EIGHTH},
                                 {F3_SHARP,DOT_QUAR},{A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},
                                 {C4,DOT_QUAR},{A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},
                                 {D4,DOT_QUAR},{E4,EIGHTH},{G4,EIGHTH},{E4,SIXTH},
                                 {D4,SIXTH},{C4,EIGHTH},{D4,EIGHTH},{G3,DOT_QUAR},
                                 {D4,EIGHTH},{F3_SHARP,DOT_QUAR},{A3,HALF},
                                 {0, QUAR}};

char Str1[16] = "";
char Str2[16] = "";
char Str3[16] = "";

//0 = off, 1 = on, 2 = snooze
int Alarm_Status = 1;

float time_on=0, Brightness = 0;

//Clocks
int Clk_Hour=10, Clk_Min=10, Clk_Min09=0, Clk_Sec=55;
int Alr_Hour=11, Alr_Min=15, Alr_Min09=0;
int Light_Time=6, Ala_Time=6, Clk_Time=6;
int counter = 0;
int AlarmGoingOff = 0;

//AM/PM
static char Clock_Time = 'P';
static char Alarm_Time = 'A';

//Prototypes
void Init_ADC14(void);
void SetupTimer32ForAlarm();
void SetUpTimer32ForCounter();
void CurrentDisplay();
void InitInterrupts();
void TimerALED();
void TimerALEDInit();
void SetupAlarmBuzzer();

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	SetupSysTick();
	SetupPort4();
	SetupLCD();
	InitInterrupts();
	TimerALEDInit();
	Init_ADC14();
	TimerALEDInit();

    //Temp
    char Str4[] = "";
    int b = 0, i = 0;
    float nADC = 0, Temp_F = 0, Temp_C = 0;
    static volatile uint16_t result;

	__enable_interrupt();
	SetUpTimer32ForCounter();

	while(1)
	{
	    if(second)
	    {
	        second = 0;
	        //Temperature
	        ADC14->CTL0 |= 1;                       //Start the conversation
	        while(!ADC14 -> IFGR0);                 //Wait for conversation to be complete
	        result = ADC14 -> MEM[2];               //Result is set equal to the value from the ADC
	        nADC = (result * 3.3) / 16384;          //nADC is set equal to the conversion
	        Temp_C = (nADC * 1000.0) / 10.0;        //Convert voltage to celsius
	        Temp_F = (Temp_C * (9.0/5.0)) + 32.0;   //Celsius to fahrenheit
	        sprintf(Str4, "%0.1f", Temp_F);         //Moves cursor to the second line on LCD//place fahrenheit into array
	        b = strlen(Str4);                       //Calculate length of array

	        //Clock time
	        Clk_Sec++;
	        if(Clk_Sec == 60)
	        {
	            Clk_Min++;
	            Clk_Sec = 0;
	        }
	        if(Clk_Min == 60)
	        {
	            Clk_Min = 0;
	            Clk_Hour++;
	        }
	        if(Clk_Hour == 13)
	            Clk_Hour = 1;

	       if((Clk_Hour == 12) && (Clk_Min == 0) && (Clk_Sec == 0))
	       {
	           if(Clock_Time == 'A')
	               Clock_Time = 'P';
	           else if(Clock_Time == 'P')
	               Clock_Time = 'A';
	       }

	       //Prints clock time
	       if(Clk_Min < 10)
	           Clk_Min09 = Clk_Min;
	       if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
	           sprintf(Str1, "   %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
	       else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
	           sprintf(Str1, "   %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	       else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
	           sprintf(Str1, "   %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
	       else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
	           sprintf(Str1, "   %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	       else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
	           sprintf(Str1, "  %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
	       else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
	           sprintf(Str1, "  %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	       else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
	           sprintf(Str1, "  %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
	       else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
	           sprintf(Str1, "  %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);

	       //Print alarm status
	       if(Alarm_Status == 0)
	           sprintf(Str2, "      Off       ");
	       else if(Alarm_Status == 1)
	           sprintf(Str2, "       On       ");
	       else if(Alarm_Status == 2)
	           sprintf(Str2, "     Snooze     ");

	       //If the alarm is on
	       if(Alarm_Status == 1)
	       {
	           if(Clock_Time == Alarm_Time)                             //If the alarm and clock times are in AM or PM
	           {
	               Clk_Time = (Clk_Hour * 60) + Clk_Min;
	               Ala_Time = (Alr_Hour * 60) + Alr_Min;
	               Light_Time = Ala_Time - Clk_Time;
	           }
	           else if(Clock_Time != Alarm_Time)                        //If the alarm and clock time differ (AM and PM)
	           {
	               Clk_Time = Clk_Min;
	               Ala_Time = 60 + Alr_Min;
	               Light_Time = Ala_Time - Clk_Time;
	           }

	           if((Alr_Min == Clk_Min) && (Alr_Hour == Clk_Hour))      //If the alarm time is equal to the clock time
	           {
	              AlarmGoingOff = 1;
	              //Make ^^ the flag for buzzer interrupt
	              SetupAlarmBuzzer();
	           }

	           if(Light_Time == 5)
	           {
	               while(Light_Time !=0 )
	               {
	                   SetupTimer32ForAlarm();
	               }
	           }

	           if(Light_Time <= 5)                                     //If the clock is within five minutes of the alarm
	           {
	               counter++;
	               if(counter == 3)
	               {
	                   TimerALED();
	                   counter = 0;
	               }
	           }
	       }

	       //Print alarm time
	       if(Alr_Min < 10)
	           Alr_Min09 = Alr_Min;
	       if((Alr_Min>9) && (Alr_Hour>9))
	           sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
	       else if((Alr_Min>9) && (Alr_Hour<10))
	           sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
	       else if((Alr_Min<10) && (Alr_Hour>9))
	           sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);
	       else if((Alr_Min<10) && (Alr_Hour<10))
	           sprintf(Str3, "     %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);

	       //Printing to LCD
	       ComWrite(0x02);                  //Home cursor
	       PrintString(Str1);               //Print first string
	       delay_micro(100);
	       ComWrite(0xC0);                  //Moves cursor to second line on LCD
	       PrintString(Str2);               //Print second string
	       delay_micro(100);
	       ComWrite(0x90);                  //Moves cursor to third line on LCD
	       PrintString(Str3);               //Print third string
	       delay_micro(100);
	       ComWrite(0xD0);                  //Move cursor to fourth line on LCD
	       for(i=0;i<5;i++)
	       {
	           ComWrite(0x14);              //Shift cursor one space to the right five times
	       }
	       PrintStringWithLength(Str4, b);  //Print temperature string
	       delay_micro(100);
	       DataWrite(0xDF);                 //Print degree symbol
	       delay_micro(100);
	       DataWrite('F');                  //Print units
	    }
	}
}


void SetupAlarmBuzzer()
{
//TA2.4


}

void TimerALED()
{
    Brightness++;
    time_on = (Brightness /100.0) * 18750.0;
    TIMER_A0 -> CCR[3]  = time_on;
    TIMER_A0 -> CCR[4]  = time_on;
}

void TimerALEDInit()
{
    //Blue P2.6, Green P2.7
    P2->SEL0 |=  (BIT6|BIT7);
    P2->SEL1 &=~ (BIT6|BIT7);
    P2->DIR  |=  (BIT6|BIT7);
    P2->OUT  &=~ (BIT6|BIT7);
    TIMER_A0->CCR[0]  = 18750.0 - 1;
    TIMER_A0->CTL     = TIMER_A_CTL_SSEL__SMCLK |
                        TIMER_A_CTL_MC__UP |
                        TIMER_A_CTL_CLR |
                        TIMER_A_CTL_ID_1;
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;
}

//Snooze and on/off interrupts
void PORT2_IRQHandler(void)
{
    //Snooze
    if (P2->IFG & BIT4)
    {
        while (!(P2->IN & BIT4)) {}
        __disable_interrupt();

        //set new alarm for 10 minutes
        Alr_Min = Alr_Min + 10;

        if(Alr_Min > 59)
        {
            Alr_Min = Alr_Min - 60;
            Alr_Hour++;
        }

        if(Alr_Hour == 13)
            Alr_Hour = 1;

         if((Alr_Hour == 12) && (Alr_Min < 11))
         {
             if(Alarm_Time == 'A')
                 Alarm_Time = 'P';
             else if(Alarm_Time == 'P')
                 Alarm_Time = 'A';
         }

        //Change status of alarm on LCD to "Snooze"
        Alarm_Status = 2;
        sprintf(Str2, "     Snooze     ");

        //Change the alarm time by 10 minutes on LCD
        if(Alr_Min < 10)
            Alr_Min09 = Alr_Min;
        if((Alr_Min>9) && (Alr_Hour>9))
            sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
        else if((Alr_Min>9) && (Alr_Hour<10))
            sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
        else if((Alr_Min<10) && (Alr_Hour>9))
            sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);
        else if((Alr_Min<10) && (Alr_Hour<10))
            sprintf(Str3, "     %d:0%d %cM   ", Alr_Hour, Alr_Min09, Alarm_Time);

        __enable_interrupt();
        //turn off alarm soundS
        TIMER_A2->CCR[0] = 0;
        TIMER32_2->LOAD = 0;
        TIMER32_2->CONTROL = 0;
    }
    P2 -> IFG  &=~ (BIT4);

    //__________________________________________________________________________________________

    //On/off
    if (P2->IFG & BIT5)
    {
        while (!(P2->IN & BIT5)) {}
        __disable_interrupt();

        //if alarm is enabled
        if(Alarm_Status == 1)
        {
            if(Light_Time < 6)
            {
                if(AlarmGoingOff == 1)
                {
                    //turn off buzzer
                    //turn off lights
                    //alarm: on
                }


                else
                {
                    //stop song
                    //turn off alarm
                    //turn off lights
                }
            }

            if(Light_Time > 5)
            {
                //disable
                Alarm_Status = 0;
            }

        }

        //if the alarm is disabled
        else if(Alarm_Status == 0)
            //enable
            Alarm_Status = 1;

        //if the alarm is on snooze
        else if(Alarm_Status == 2)
        {
            Alarm_Status = 1;
            //turn off lights


        }

        //Print alarm status
        if(Alarm_Status == 0)
            sprintf(Str2, "      Off       ");
        else if(Alarm_Status == 1)
            sprintf(Str2, "       On       ");
        else if(Alarm_Status == 2)
            sprintf(Str2, "     Snooze     ");

        __enable_interrupt();
    }
    P2 -> IFG  &=~ (BIT5);
}

//Sets time and alarm interrupt
void PORT5_IRQHandler(void)
{
    //Set time
    if (P5->IFG & BIT7)
    {
        __disable_interrupt();
        //flash hours
        //blink at certain spot until "set time" is pressed again
        while(!((P5->IN & BIT7) == BIT7))
        {
            if(Clk_Min < 10)
                   Clk_Min09 = Clk_Min;
            if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                sprintf(Str1, "   %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                sprintf(Str1, "   %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                sprintf(Str1, "   %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                sprintf(Str1, "   %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
            else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                sprintf(Str1, "  %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                sprintf(Str1, "  %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                sprintf(Str1, "  %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                sprintf(Str1, "  %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);

            ComWrite(0x90);                  //Moves cursor to third line on LCD
            PrintString(Str3);               //Print third string

            delay_milli(1000);

            if(Clk_Min < 10)
                   Clk_Min09 = Clk_Min;
            if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                sprintf(Str1, "    :0%d:0%d %cM   ", Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                sprintf(Str1, "    :%d:%d %cM   ", Clk_Min, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                sprintf(Str1, "    :0%d:%d %cM   ", Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                sprintf(Str1, "    :%d:0%d %cM   ", Clk_Min, Clk_Sec, Clock_Time);
            else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                sprintf(Str1, "    :0%d:0%d %cM   ", Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                sprintf(Str1, "    :%d:%d %cM   ", Clk_Min, Clk_Sec, Clock_Time);
            else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                sprintf(Str1, "    :0%d:%d %cM   ", Clk_Min09, Clk_Sec, Clock_Time);
            else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                sprintf(Str1, "    :%d:0%d %cM   ", Clk_Min, Clk_Sec, Clock_Time);

            ComWrite(0x90);                  //Moves cursor to third line on LCD
            PrintString(Str3);               //Print third string


            //user presses "on/off/up" button to increment hours
            if(P2->OUT & BIT5)
            {
                Clk_Hour++;
                //when hours reaches 12 am/pm, go to 1 pm/am
                if(Clk_Hour == 13)
                {
                    Clk_Hour = 1;
                    if(Clock_Time == 'A')
                        Clock_Time = 'P';
                    if(Clock_Time == 'P')
                        Clock_Time = 'A';
                }
            }

            //user presses "snooze/down" button to decrement hours
            if(P2->OUT & BIT4)
            {
                Clk_Hour--;
                //when hours reaches 1 am/pm, go to 12 pm/am
                if(Clk_Hour == 0)
                {
                    Clk_Hour = 12;
                    if(Clock_Time == 'A')
                        Clock_Time = 'P';
                    if(Clock_Time == 'P')
                        Clock_Time = 'A';
                }
            }
        }


        //user presses set-alarm button to go to minutes
        if(P5->OUT & BIT7)
        {
            //hours stop flashing and minutes start flashing
            //blink at certain spot until "set time" is pressed again
            while(!((P5->IN & BIT7)==BIT7))
            {
                if(Clk_Min < 10)
                       Clk_Min09 = Clk_Min;
                if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);

                //delay

                if(Clk_Min < 10)
                       Clk_Min09 = Clk_Min;
                if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:  :0%d %cM   " , Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:  :%d %cM   ", Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:  :%d %cM   ", Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:  :0%d %cM   ", Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:  :0%d %cM   " , Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:  :%d %cM   ", Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:  :%d %cM   ", Clk_Hour, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:  :0%d %cM   ", Clk_Hour, Clk_Sec, Clock_Time);


                //user presses "on/off/up" button to increment minutes
                if(P2->OUT & BIT5)
                {
                    Clk_Min++;
                    //when minutes reaches 60, go to 0 and increment hour
                    if(Clk_Min == 60)
                    {
                        Clk_Min = 0;
                        Clk_Hour++;
                        //when hours reaches 12 am/pm, go to 1 pm/am
                        if(Clk_Hour == 13)
                        {
                            Clk_Hour = 1;
                            if(Clock_Time == 'A')
                                Clock_Time = 'P';
                            if(Clock_Time == 'P')
                                Clock_Time = 'A';
                        }
                    }
                }
                //user presses "snooze/down" button to decrement minutes
                if(P2->OUT & BIT4)
                {
                    Clk_Min--;
                    //when minutes reaches -1, go to 69 and increment hour
                    if(Clk_Min == -1)
                    {
                        Clk_Min = 59;
                        Clk_Hour--;
                        //when hours reaches 12 am/pm, go to 1 pm/am
                        if(Clk_Hour == 0)
                        {
                            Clk_Hour = 12;
                            if(Clock_Time == 'A')
                                Clock_Time = 'P';
                            if(Clock_Time == 'P')
                                Clock_Time = 'A';
                        }
                    }
                }
            }

            //user presses set-alarm button again
            if(P5->OUT & BIT7)
            {
                //flashing stops and time is now set
                if(Clk_Min < 10)
                       Clk_Min09 = Clk_Min;
                if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                    sprintf(Str1, "   %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:0%d:%d %cM   ", Clk_Hour, Clk_Min09, Clk_Sec, Clock_Time);
                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                    sprintf(Str1, "  %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);

                //clear interrupt
                P5 -> IFG  &=~ (BIT7);
            }

        }
    }
    __enable_interrupt();

 //__________________________________________________________________________________________________________

    //Set alarm
    if (P5->IFG & BIT0)
    {
        __disable_interrupt();
        //flash hours
        while(!((P5->IN & BIT0)==BIT0))
        {
            if(Alr_Min < 10)
                Alr_Min09 = Alr_Min;
            if((Alr_Min>9) && (Alr_Hour>9))
                sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
            else if((Alr_Min>9) && (Alr_Hour<10))
                sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
            else if((Alr_Min<10) && (Alr_Hour>9))
                sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);
            else if((Alr_Min<10) && (Alr_Hour<10))
                sprintf(Str3, "     %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);

            //delay

            if(Alr_Min < 10)
                Alr_Min09 = Alr_Min;
            if((Alr_Min>9) && (Alr_Hour>9))
                sprintf(Str3, "      :%d %cM    ", Alr_Min, Alarm_Time);
            else if((Alr_Min>9) && (Alr_Hour<10))
                sprintf(Str3, "      :%d %cM    ", Alr_Min, Alarm_Time);
            else if((Alr_Min<10) && (Alr_Hour>9))
                sprintf(Str3, "      :0%d %cM    ", Alr_Min09, Alarm_Time);
            else if((Alr_Min<10) && (Alr_Hour<10))
                sprintf(Str3, "      :0%d %cM    ", Alr_Min09, Alarm_Time);

            //user presses "on/off/up" button to increment hours
            if(P2->OUT & BIT5)
            {
                Alr_Hour++;
                //when hours reaches 12 am/pm, go to 1 pm/am
                if(Alr_Hour == 13)
                {
                    Alr_Hour = 1;
                    if(Alarm_Time == 'A')
                        Alarm_Time = 'P';
                    if(Alarm_Time == 'P')
                        Alarm_Time = 'A';
                }
            }

            //user presses "snooze/down" button to decrement hours
            if(P2->OUT & BIT4)
            {
                Alr_Hour--;
                //when hours reaches 1 am/pm, go to 12 pm/am
                if(Alr_Hour == 0)
                {
                    Alr_Hour = 12;
                    if(Alarm_Time == 'A')
                        Alarm_Time = 'P';
                    if(Alarm_Time == 'P')
                        Alarm_Time = 'A';
                }
            }
        }

        //user presses set-alarm button to go to minutes
        if(P5->OUT & BIT0)
        {
            //hours stop flashing and minutes start flashing
            while(!((P5->IN & BIT0)==BIT0))
            {
                if(Alr_Min < 10)
                    Alr_Min09 = Alr_Min;
                if((Alr_Min>9) && (Alr_Hour>9))
                    sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
                else if((Alr_Min>9) && (Alr_Hour<10))
                    sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
                else if((Alr_Min<10) && (Alr_Hour>9))
                    sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);
                else if((Alr_Min<10) && (Alr_Hour<10))
                    sprintf(Str3, "     %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);

                //delay

                if(Alr_Min < 10)
                    Alr_Min09 = Alr_Min;
                if((Alr_Min>9) && (Alr_Hour>9))
                    sprintf(Str3, "    %d:   %cM    ", Alr_Hour, Alarm_Time);
                else if((Alr_Min>9) && (Alr_Hour<10))
                    sprintf(Str3, "     %d:   %cM    ", Alr_Hour, Alarm_Time);
                else if((Alr_Min<10) && (Alr_Hour>9))
                    sprintf(Str3, "    %d:0  %cM    ", Alr_Hour, Alarm_Time);
                else if((Alr_Min<10) && (Alr_Hour<10))
                    sprintf(Str3, "     %d:0  %cM    ", Alr_Hour, Alarm_Time);

                //user presses "on/off/up" button to increment minutes
                if(P2->OUT & BIT5)
                {
                    Alr_Min++;
                    //when minutes reaches 60, go to 0 and increment hour
                    if(Alr_Min == 60)
                    {
                        Alr_Min = 0;
                        Alr_Hour++;
                        //when hours reaches 12 am/pm, go to 1 pm/am
                        if(Alr_Hour == 13)
                        {
                            Alr_Hour = 1;
                            if(Alarm_Time == 'A')
                                Alarm_Time = 'P';
                            if(Alarm_Time == 'P')
                                Alarm_Time = 'A';
                        }
                    }
                }
                //user presses "snooze/down" button to decrement minutes
                if(P2->OUT & BIT4)
                {
                    Alr_Min--;
                    //when minutes reaches -1, go to 69 and increment hour
                    if(Alr_Min == -1)
                    {
                        Alr_Min = 59;
                        Alr_Hour--;
                        //when hours reaches 12 am/pm, go to 1 pm/am
                        if(Alr_Hour == 0)
                        {
                            Alr_Hour = 12;
                            if(Alarm_Time == 'A')
                                Alarm_Time = 'P';
                            if(Alarm_Time == 'P')
                                Alarm_Time = 'A';
                        }
                    }
                }
            }

            //user presses set-alarm button again
            if(P5->OUT & BIT0)
            {
                //flashing stops and time is now set
                if(Alr_Min < 10)
                    Alr_Min09 = Alr_Min;
                if((Alr_Min>9) && (Alr_Hour>9))
                    sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
                else if((Alr_Min>9) && (Alr_Hour<10))
                    sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
                else if((Alr_Min<10) && (Alr_Hour>9))
                    sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);
                else if((Alr_Min<10) && (Alr_Hour<10))
                    sprintf(Str3, "     %d:0%d %cM    ", Alr_Hour, Alr_Min09, Alarm_Time);

                //Clear interrupt
                P5 -> IFG  &=~ (BIT0);
            }
        }
    }
    __enable_interrupt();
}

void InitInterrupts()
{
    //Button interrupt for setting the time
    P5->SEL0 &=~ BIT7;
    P5->SEL1 &=~ BIT7;
    P5->DIR  &=~ BIT7;
    P5->REN  |=  BIT7;
    P5->OUT  |=  BIT7;
    P5->IES  |=  BIT7;
    P5->IE   |=  BIT7;
    P5->IFG   =  0;
    //Button interrupt for setting alarm
    P5->SEL0 &=~ BIT0;
    P5->SEL1 &=~ BIT0;
    P5->DIR  |=  BIT0;
    P5->REN  |=  BIT0;
    P5->OUT  |=  BIT0;
    P5->IES  |=  BIT0;
    P5->IE   |=  BIT0;
    P5->IFG   =  0;
    NVIC ->ISER[1] = 1 << ((PORT5_IRQn) & 31);

    //Button interrupt for snooze/down
    P2->SEL0 &=~ BIT4;
    P2->SEL1 &=~ BIT4;
    P2->DIR  &=~ BIT4;
    P2->REN  |=  BIT4;
    P2->OUT  |=  BIT4;
    P2->IES  |=  BIT4;
    P2->IE   |=  BIT4;
    P2->IFG   =  0;

    //Button interrupt for on/off/up
    P2->SEL0 &=~ BIT5;
    P2->SEL1 &=~ BIT5;
    P2->DIR  &=~ BIT5;
    P2->REN  |=  BIT5;
    P2->OUT  |=  BIT5;
    P2->IES  |=  BIT5;
    P2->IE   |=  BIT5;
    P2->IFG   =  0;
    NVIC ->ISER[1] = 1 << ((PORT2_IRQn) & 31);
}

//This function initializes the ADC
void Init_ADC14(void)
{
    //configure P5.4 for A1
    P5 -> SEL0 |= BIT2;
    P5 -> SEL1 |= BIT2;

    ADC14 -> CTL0    = 0x00000010;  //Power on and disable
    ADC14 -> CTL0   |= 0x04080300;  //S/H pulse mode, Sysclk, 32 sample clocks
    ADC14 -> CTL1   |= 0x00000030;  //14 bit resolution
    ADC14 -> MCTL[2] = 3;           //A1 input, single-ended
    ADC14 -> CTL1   |= 0x00020000;  //Convert for mem reg 2
    ADC14 -> CTL0   |= 2;           //Enable ADC
}

void T32_INT1_IRQHandler()
{
    TIMER32_1->INTCLR = 1;          //Clear interrupt flag so it does not interrupt again immediately
    second = 1;
    TIMER32_1->LOAD = 3000000 - 1;  //Set to a count down of 1 second on 3 MHz clock
}

void SetUpTimer32ForCounter()
{
    TIMER32_1->CONTROL = 0b11100011;    //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
    NVIC_EnableIRQ(T32_INT1_IRQn);      //Enable Timer32_1 interrupt
    TIMER32_1->LOAD = 3000000 - 1;      //Set to a count down of 1 second on 3 MHz clock
}

void T32_INT2_IRQHandler()
{
    TIMER32_2->INTCLR = 1;              //Clear interrupt flag so it does not interrupt again immediately.
    if(breath) {                        //Provides separation between notes
        TIMER_A2->CCR[0] = 0;           //Set output of TimerA to 0
        TIMER_A2->CCR[1] = 0;
        TIMER_A2->CCR[2] = 0;
        TIMER32_2->LOAD = BREATH_TIME;  //Load in breath time to interrupt again
        breath = 0;                     //Next Timer32 interrupt is no longer a breath, but is a note
    }
    else {                                                  //If not a breath (a note)
        TIMER32_2->LOAD = music_note_sequence[note][1] - 1; //Load into interrupt count down the length of this note
        if(music_note_sequence[note][0] == REST) {          //If note is actually a rest, load in nothing to TimerA
            TIMER_A2->CCR[0] = 0;
            TIMER_A2->CCR[1] = 0;
            TIMER_A2->CCR[2] = 0;
        }
        else {
            TIMER_A2->CCR[0] = 3000000 / music_note_sequence[note][0];  //Divide by the frequency of current note to get the period
            TIMER_A2->CCR[1] = 1500000 / music_note_sequence[note][0];  //50% duty cycle
            TIMER_A2->CCR[2] = TIMER_A0->CCR[0];                        //Had this in here for fun with interrupts.  Not used right now
        }
        note = note + 1;                        //Next note
        if(note >= MAX_NOTE) {                  //Go back to the beginning if at the end
            note = 0;
            TIMER32_2->CONTROL = 0b11000011;    //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
        }
        breath = 1;                             //Next time through should be a breath for separation.
    }
}

void TA0_N_IRQHandler()
{
    if(TIMER_A2->CCTL[1] & BIT0){}          //If CCTL1 is the reason for the interrupt (BIT0 holds the flag)
}

void SetupTimer32ForAlarm()
{
    //Setup pwm pin
    P5->SEL0 |=  (BIT6);
    P5->SEL1 &=~ (BIT6);
    P5->DIR  |=  (BIT6);

    TIMER32_2->CONTROL = 0b11100011;        //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
    NVIC_EnableIRQ(T32_INT2_IRQn);          //Enable Timer32_2 interrupt.
    TIMER32_2->LOAD = 3000000 - 1;          //Set to a count down of 1 second on 3 MHz clock

    TIMER_A2->CCR[0] = 0;                   // Turn off to start
    TIMER_A2->CCTL[1] = 0b0000000011110100; // Setup Timer A0_1 Reset/Set, Interrupt, No Output
    TIMER_A2->CCR[1] = 0;                   // Turn off timerA to start
    TIMER_A2->CTL = 0b0000001000010100;     // Count Up mode using SMCLK, Clears, Clear Interrupt Flag
    NVIC_EnableIRQ(TA0_N_IRQn);             // Enable interrupts for CCTL1-6 (if on)
}
