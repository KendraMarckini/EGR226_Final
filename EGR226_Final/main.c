//create interrupt for serial


#include "msp.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "LCD_Code.h"

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
#define MAX_NOTE_S  74
#define MAX_NOTE_A  3

//Global variables
int note = 0;
int breath = 0;
float music_note_sequence_A[][2] = {{0,QUAR},{C4,QUAR},{0,QUAR}};
float music_note_sequence[][2] = {//Chorus, 39 notes
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


void Print_To_LCD(char *String1, char *String2, char *String3, char *String4, int length);
void Clock_Function(int Speed);
void Store_Alarm_Status(int Status);
void Store_Time(int Clock_Type);
void Init_ADC14(void);
void SetupTimer32ForAlarm();
void SetUpTimer32ForCounter();
void InitInterrupts();
void TimerALEDInit();
void setupSerial();
void readInput(char *string);
void writeOutput(char *string);
void GetTemp();
void GetBacklight();

//Alarm and time defines
volatile uint32_t second=1;

//1: set clock time, 2: set alarm time, 3: snooze, 4: on/off
int Speed_Flag=0;

int Clk_Hour=10, Clk_Min=9, Clk_Sec=55;
int Alr_Hour=10, Alr_Min=15;
int Light_Time=6, Ala_Time=6, Clk_Time=6, Light_Time_Flag=1;
int PreAlr_Hour=0, PreAlr_Min=0;
int counter=0, AlarmGoingOff=0;
int Hour=0, previous=0;

//AM/PM
static char Clock_Time='P', Alarm_Time='P';
static char PreAlr_Time= 'P';

//0 = off, 1 = on, 2 = snooze, 3 = no alarm
int Alarm_Status=1;

//Defines strings to print to LCD
char Str1[16]="", Str2[16]="", Str3[16]="", Str4[16]="", Str5[16]="";

int b=0;

//Defines for setting LED brightness
float time_on=0, Brightness=0;

volatile int Flag=0;
volatile int Button=0;

#define BUFFER_SIZE 100
int Serial_Var=0;
char string[BUFFER_SIZE];
char INPUT_BUFFER[BUFFER_SIZE];
uint8_t storage_location=0;
uint8_t read_location=0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    __disable_interrupt();

    int Read_Clk_Hour=0, Read_Clk_Min=0, Read_Clk_Sec=0;
    int Read_Alr_Hour=0, Read_Alr_Min=0;

    SetupSysTick();
    SetupPort4();
    SetupLCD();
    InitInterrupts();
    TimerALEDInit();
    Init_ADC14();
    TimerALEDInit();
    setupSerial();
    INPUT_BUFFER[0]='\0';
    __enable_interrupt();
    SetUpTimer32ForCounter();

	while(1)
	{
	    //check serial flag here
	    if(Serial_Var)
	    {
	        //Read the input up to "\n" and store in string
	        readInput(string);
	        //If statement will continue to run until it reaches the end
	        if(string[0] != '\0')
	        {
	            if(string[0] == 'S')
	            {
	                if(string[3] == 'T')
	                {
	                    Clk_Hour = ((string[8] - 48) * 10) + (string[9] - 48);
	                    Clk_Min  = ((string[11] - 48) * 10) + (string[12] - 48);
	                    Clk_Sec  = ((string[14] - 48) * 10) + (string[15] - 48);

	                    if(Clk_Hour == 12)
	                        Clock_Time = 'P';

	                    if((Clk_Hour > 12) && (Clk_Hour <= 23))
	                    {
	                        Clock_Time = 'P';
	                        Clk_Hour = Clk_Hour - 12;
	                    }
	                    else if((Clk_Hour < 12) && (Clk_Hour >= 0))
	                    {
	                        Clock_Time = 'A';
	                        if (Clk_Hour == 0)
	                            Clk_Hour = 12;
	                    }

	                    //Prints clock time
	                    if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
	                        sprintf(Str1, "   %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
	                        sprintf(Str1, "   %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
	                        sprintf(Str1, "   %d:0%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
	                        sprintf(Str1, "   %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
	                        sprintf(Str1, "  %d:0%d:0%d %cM   " , Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
	                        sprintf(Str1, "  %d:%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
	                        sprintf(Str1, "  %d:0%d:%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
	                        sprintf(Str1, "  %d:%d:0%d %cM   ", Clk_Hour, Clk_Min, Clk_Sec, Clock_Time);
	                    //Printing to LCD
	                    ComWrite(0x02);                  //Home cursor
	                    PrintString(Str1);               //Print first string
	                    delay_micro(100);
	                    writeOutput("\n");
	                }

	                else if(string[3] == 'A')
	                {
	                    Alr_Hour = ((string[9] - 48) * 10) + (string[10] - 48);
	                    Alr_Min  = ((string[12] - 48) * 10) + (string[13] - 48);

	                    if(Alr_Hour == 12)
	                        Alarm_Time = 'P';

	                    if((Alr_Hour > 12) && (Alr_Hour <= 23))
	                    {
	                        Alarm_Time = 'P';
	                        Alr_Hour = Alr_Hour - 12;
	                    }
	                    else if((Alr_Hour < 12) && (Alr_Hour >= 0))
	                    {
	                        Alarm_Time = 'A';
	                        if (Alr_Hour == 0)
	                            Alr_Hour = 12;
	                    }

	                    //Print alarm time
	                    if((Alr_Min>9) && (Alr_Hour>9))
	                        sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
	                    else if((Alr_Min>9) && (Alr_Hour<10))
	                        sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
	                    else if((Alr_Min<10) && (Alr_Hour>9))
	                        sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
	                    else if((Alr_Min<10) && (Alr_Hour<10))
	                        sprintf(Str3, "     %d:0%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
	                    ComWrite(0x90);                  //Moves cursor to third line on LCD
	                    PrintString(Str3);               //Print third string
	                    delay_micro(100);
	                    writeOutput("\n");
	                }
	                else
	                {
	                    writeOutput("Invalid -> Not a set command ");
	                    writeOutput(string);
	                    writeOutput("\n\n");
	                }
	            }
	            //Checks if the first character in the string is a G
	            else if(string[0] == 'R')
	            {
	                if(string[4] == 'T')
	                {
	                    Read_Clk_Hour = Clk_Hour;
	                    Read_Clk_Min = Clk_Min;
	                    Read_Clk_Sec = Clk_Sec;

	                    if(Clock_Time == 'P')
	                        Read_Clk_Hour = Read_Clk_Hour + 12;
	                    if((Read_Clk_Hour == 12) && (Clock_Time == 'A'))
	                        Read_Clk_Hour = 0;

	                    if((Read_Clk_Hour < 10) && (Read_Clk_Min < 10) && (Read_Clk_Sec < 10))
	                        sprintf(Str5, "0%d:0%d:0%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour >= 10) && (Read_Clk_Min < 10) && (Read_Clk_Sec < 10))
	                        sprintf(Str5, "%d:0%d:0%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour < 10) && (Read_Clk_Min >= 10) && (Read_Clk_Sec < 10))
	                        sprintf(Str5, "0%d:%d:0%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour >= 10) && (Read_Clk_Min >= 10) && (Read_Clk_Sec < 10))
	                        sprintf(Str5, "%d:%d:0%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour < 10) && (Read_Clk_Min < 10) && (Read_Clk_Sec >= 10))
	                        sprintf(Str5, "0%d:0%d:%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour >= 10) && (Read_Clk_Min < 10) && (Read_Clk_Sec >= 10))
	                        sprintf(Str5, "%d:0%d:%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour < 10) && (Read_Clk_Min >= 10) && (Read_Clk_Sec >= 10))
	                        sprintf(Str5, "0%d:%d:%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);
	                    if((Read_Clk_Hour >= 10) && (Read_Clk_Min >= 10) && (Read_Clk_Sec >= 10))
	                        sprintf(Str5, "%d:%d:%d\n" , Read_Clk_Hour, Read_Clk_Min, Read_Clk_Sec);

	                    writeOutput(Str5);
	                    writeOutput("\n");
	                }

	                else if(string[4] == 'A')
	                {
	                    Read_Alr_Hour = Alr_Hour;
	                    Read_Alr_Min = Alr_Min;

	                    if(Alarm_Time == 'P')
	                        Read_Alr_Hour = Read_Alr_Hour + 12;
	                    if((Read_Alr_Hour == 12) && (Alarm_Time == 'A'))
	                        Read_Alr_Hour = 0;

	                    if((Read_Alr_Hour < 10) && (Read_Alr_Min < 10))
	                        sprintf(Str5, "0%d:0%d\n" , Read_Alr_Hour, Read_Alr_Min);
	                    if((Read_Alr_Hour >= 10) && (Read_Alr_Min < 10))
	                        sprintf(Str5, "%d:0%d\n" , Read_Alr_Hour, Read_Alr_Min);
	                    if((Read_Alr_Hour < 10) && (Read_Alr_Min >= 10))
	                        sprintf(Str5, "0%d:%d\n" , Read_Alr_Hour, Read_Alr_Min);
	                    if((Read_Alr_Hour >= 10) && (Read_Alr_Min >= 10))
	                        sprintf(Str5, "%d:%d\n" , Read_Alr_Hour, Read_Alr_Min);

	                    writeOutput(Str5);
	                    writeOutput("\n");
	                }
	                else
	                {
	                    writeOutput("Invalid ->  Not a Read Command: ");
	                    writeOutput(string);
	                    writeOutput("\n\n");
	                }
	            }
	            else
	            {
	                writeOutput("Invalid -> Not a command: ");
	                writeOutput(string);
	                writeOutput("\n\n");
	            }
	        }
	        Serial_Var = 0;
	    }

	    //do main clock stuff here
	    if(second)
	        Clock_Function(Speed_Flag);

	    PreAlr_Hour = Alr_Hour;
        PreAlr_Min = Alr_Min;
        PreAlr_Time = Alarm_Time;

	    switch (Button)
	    {
	        //Set time
	        case 1:
                P5->IE &=~ BIT7;
	            if(Flag == 1)
	            {
	                //flash hours
                    if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    ComWrite(0x02);                  //Moves cursor to first line on LCD
                    PrintString(Str1);               //Print third string
                    delay_milli(500);
                    if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"    :0%d:0%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"    :%d:%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"    :0%d:%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"    :%d:0%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"    :0%d:0%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"    :%d:%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"    :0%d:%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"    :%d:0%d %cM   ",Clk_Min,Clk_Sec,Clock_Time);
                    ComWrite(0x02);                  //Moves cursor to first line on LCD
                    PrintString(Str1);               //Print third string
                    delay_milli(500);
	            }
	            if(Flag == 2)
	            {
	                //Flash minutes
                    if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
                    ComWrite(0x02);                  //Moves cursor to first line on LCD
                    PrintString(Str1);               //Print third string
                    delay_milli(500);
                    if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:  :0%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:  :%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:  :%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
                        sprintf(Str1,"   %d:  :0%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:  :0%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:  :%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:  :%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
                        sprintf(Str1,"  %d:  :0%d %cM   ",Clk_Hour,Clk_Sec,Clock_Time);
                    ComWrite(0x02);                  //Moves cursor to first line on LCD
                    PrintString(Str1);               //Print third string
                    delay_milli(500);
	            }
	            if(Flag == 3)
	            {
	                if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
	                    sprintf(Str1,"   %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
	                    sprintf(Str1,"   %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
	                    sprintf(Str1,"   %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
	                    sprintf(Str1,"   %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
	                    sprintf(Str1,"  %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
	                    sprintf(Str1,"  %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
	                    sprintf(Str1,"  %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
	                    sprintf(Str1,"  %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
	                Flag = 0;
	                Button = 0;
	            }
                P5->IE |= BIT7;
	            break;

//_____________________________________________________________________________________________________________________________

	        //Set alarm
	        case 2:
                P5->IE &=~ BIT0;
                if(Flag == 1)
                {
                    //flash hours
                    if((Alr_Min>9) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min>9) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:0%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:0%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    ComWrite(0x90);                  //Moves cursor to third line on LCD
                    PrintString(Str3);               //Print third string
                    delay_milli(500);
                    if((Alr_Min>9) && (Alr_Hour>9))
                        sprintf(Str3,"      :%d %cM    ",Alr_Min,Alarm_Time);
                    else if((Alr_Min>9) && (Alr_Hour<10))
                        sprintf(Str3,"      :%d %cM    ",Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour>9))
                        sprintf(Str3,"      :0%d %cM    ",Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour<10))
                        sprintf(Str3,"      :0%d %cM    ",Alr_Min,Alarm_Time);
                    ComWrite(0x90);                  //Moves cursor to third line on LCD
                    PrintString(Str3);               //Print third string
                    delay_milli(500);
                }
                if(Flag == 2)
                {
                    //Flash minutes
                    if((Alr_Min>9) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min>9) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:0%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:0%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    ComWrite(0x90);                  //Moves cursor to third line on LCD
                    PrintString(Str3);               //Print third string
                    delay_milli(500);
                    if((Alr_Min>9) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:   %cM    ",Alr_Hour,Alarm_Time);
                    else if((Alr_Min>9) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:   %cM    ",Alr_Hour,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:0  %cM    ",Alr_Hour,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:0  %cM    ",Alr_Hour,Alarm_Time);
                    ComWrite(0x90);                  //Moves cursor to third line on LCD
                    PrintString(Str3);               //Print third string
                    delay_milli(500);
                }
                if(Flag == 3)
                {
                    if((Alr_Min>9) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min>9) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour>9))
                        sprintf(Str3,"    %d:0%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);
                    else if((Alr_Min<10) && (Alr_Hour<10))
                        sprintf(Str3,"     %d:0%d %cM    ",Alr_Hour,Alr_Min,Alarm_Time);

                    PreAlr_Hour = Alr_Hour;
                    PreAlr_Min = Alr_Min;
                    PreAlr_Time = Alarm_Time;

                    Flag = 0;
                    Button = 0;
                }
                Light_Time_Flag = 1;
                P5->IE |= BIT0;
	            break;

//_____________________________________________________________________________________________________________________________

	        //Snooze
	        case 3:
                P2->IE &=~  BIT4;

                if(Alarm_Status == 0)                               //If the alarm is off
                    Alarm_Status = 0;

                else if((Alarm_Status == 1) | (Alarm_Status == 2))  //If the alarm is on or is snoozed
                {
                    if((Light_Time <= 5) && (Light_Time >=0))       //If it's pre-alarm time
                    {
                        Alarm_Status = 2;                          //Alarm status is snooze

                        TIMER_A0 -> CCR[3]  = 0;                    //Turn off LEDs
                        TIMER_A0 -> CCR[4]  = 0;
                        TIMER32_2->CONTROL = 0;                     //Turn off song
                        TIMER32_2->LOAD = 0;
                        TIMER_A2->CCR[0] = 0;
                        TIMER_A2->CCTL[1] = 0;
                        TIMER_A2->CCR[1] = 0;
                        TIMER_A2->CTL = 0;

                        Alr_Min = Alr_Min + 10;                     //Set new alarm for 10 minutes
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
                    }
                    else if(AlarmGoingOff == 1)                     //If the alarm is going off
                    {
                        Alarm_Status = 2;                          //Alarm status is snooze

                        TIMER_A0 -> CCR[3]  = 0;                    //Turn off LEDs
                        TIMER_A0 -> CCR[4]  = 0;
                        TIMER32_2->CONTROL = 0;                     //Turn off buzzard
                        TIMER32_2->LOAD = 0;
                        TIMER_A2->CCR[0] = 0;
                        TIMER_A2->CCTL[1] = 0;
                        TIMER_A2->CCR[1] = 0;
                        TIMER_A2->CTL = 0;

                        Alr_Min = Alr_Min + 10;                     //Set new alarm for 10 minutes
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
                    }
                    else if((AlarmGoingOff==0)&&((Light_Time>5)|(Light_Time<0)))   //If the alarm is not going off nor is in pre-alarm
                        Alarm_Status = 2;
                }

                Flag = 0;
                Button = 0;
                P2->IE |= BIT4;
                break;

//_____________________________________________________________________________________________________________________________

	        //On/Off
                //0 0ff, 1 on, 2 snooze
            case 4:
                P2->IE &=~ BIT5;

                if(Alarm_Status == 0)
                {
                    Alarm_Status = 1;

                    //Light_Time = 10;                                //Set light time equal to 10
                    //Light_Time_Flag = 0;
                }

                else if(Alarm_Status == 1)
                {
                    if((Light_Time < 6) && (Light_Time >=0) && (Light_Time_Flag != 0))       //If it's pre-alarm time
                    {
                        Alarm_Status = 1;
                        TIMER_A0 -> CCR[3]  = 0;                    //Turn off LEDs
                        TIMER_A0 -> CCR[4]  = 0;
                        TIMER32_2->CONTROL = 0;                     //Turn off sounds
                        TIMER32_2->LOAD = 0;
                        TIMER_A2->CCR[0] = 0;
                        TIMER_A2->CCTL[1] = 0;
                        TIMER_A2->CCR[1] = 0;
                        TIMER_A2->CTL = 0;

                        Light_Time = 10;                                //Set light time equal to 10
                        Light_Time_Flag = 0;
                    }
                    else if(AlarmGoingOff == 1)                     //If the alarm is going off
                    {
                        Alarm_Status = 1;
                        TIMER_A0 -> CCR[3]  = 0;                    //Turn off LEDs
                        TIMER_A0 -> CCR[4]  = 0;
                        TIMER32_2->CONTROL = 0;                     //Turn off sounds
                        TIMER32_2->LOAD = 0;
                        TIMER_A2->CCR[0] = 0;
                        TIMER_A2->CCTL[1] = 0;
                        TIMER_A2->CCR[1] = 0;
                        TIMER_A2->CTL = 0;

                        Light_Time = 10;                                //Set light time equal to 10
                        Light_Time_Flag = 0;

                        Alr_Hour = PreAlr_Hour;     //Reset alarm hour
                        Alr_Min = PreAlr_Min;       //Reset alarm minute
                        Alarm_Time = PreAlr_Time;   //Reset alarm time
                    }
                    else if(Light_Time_Flag == 0)  //If the alarm is not going off nor is in pre-alarm
                    {
                        Alarm_Status = 0;
                        Light_Time = 10;                                //Set light time equal to 10
                    }
                }
                else if(Alarm_Status == 2)
                {
                    Alarm_Status = 1;           //Alarm status on

                    Alr_Hour = PreAlr_Hour;     //Reset alarm hour
                    Alr_Min = PreAlr_Min;       //Reset alarm minute
                    Alarm_Time = PreAlr_Time;   //Reset alarm time

                    TIMER_A0 -> CCR[3]  = 0;                    //Turn off LEDs
                    TIMER_A0 -> CCR[4]  = 0;
                    TIMER32_2->CONTROL = 0;                     //Turn off sounds
                    TIMER32_2->LOAD = 0;
                    TIMER_A2->CCR[0] = 0;
                    TIMER_A2->CCTL[1] = 0;
                    TIMER_A2->CCR[1] = 0;
                    TIMER_A2->CTL = 0;

                    Light_Time_Flag = 0;
                }

                Flag = 0;
                Button = 0;
                P2->IE |= BIT5;
                break;
	    }
	GetBacklight();
	Store_Time(1);
    Store_Alarm_Status(Alarm_Status);
    Store_Time(0);
    GetTemp();
    Print_To_LCD(Str1, Str2, Str3, Str4, b);
	}
}

void TA3_0_IRQHandler()
{
    if(Flag)
    {
        //start timer
    }
    if(Flag > 0)
    {
        //stop timer
    }
}


void GetBacklight()
{
    float nADC = 0.0;
    static volatile uint16_t result;
    float Back_Light = 0;

    ADC14->CTL0 |= 1;                       //Start the conversation
    while(!ADC14 -> IFGR0 & BIT1);          //Wait for conversation to be complete
    result = ADC14 -> MEM[1];               //Result is set equal to the value from the ADC
    nADC = (result * 3.3) / 16384;          //nADC is set equal to the conversion
    Back_Light = (nADC/3.3) * 100;
    TIMER_A1 -> CCR[1]  = Back_Light;
    ADC14->CTL0 &=~ 2;
    ADC14->CTL0 |=  2;
}

void GetTemp()
{
    float nADC = 0.0, Temp_F = 0.0, Temp_C = 0.0;
    static volatile uint16_t result;

    ADC14->CTL0 |= 1;                       //Start the conversation
    while(!ADC14 -> IFGR0);                 //Wait for conversation to be complete
    result = ADC14 -> MEM[0];               //Result is set equal to the value from the ADC
    nADC = (result * 3.3) / 16384.0;          //nADC is set equal to the conversion
    Temp_C = (nADC * 1000.0) / 10.0;        //Convert voltage to celsius
    Temp_F = (Temp_C * (9.0/5.0)) + 32.0;   //Celsius to fahrenheit
    sprintf(Str4, "%0.1f", Temp_F);         //Moves cursor to the second line on LCD//place fahrenheit into array
    b = strlen(Str4);                       //Calculate length of array
}

void Print_To_LCD(char *String1, char *String2, char *String3, char *String4, int length)
{
    int i=0;
    ComWrite(0x02);                  //Home cursor
    PrintString(String1);               //Print first string
    delay_micro(100);
    ComWrite(0xC0);                  //Moves cursor to second line on LCD
    PrintString(String2);               //Print second string
    delay_micro(100);
    ComWrite(0x90);                  //Moves cursor to third line on LCD
    PrintString(String3);               //Print third string
    delay_micro(100);
    ComWrite(0xD0);                  //Move cursor to fourth line on LCD
    for(i=0;i<5;i++)
    {
        ComWrite(0x14);              //Shift cursor one space to the right five times
    }
    PrintStringWithLength(String4, length);  //Print temperature string
    delay_micro(100);
    DataWrite(0xDF);                 //Print degree symbol
    delay_micro(100);
    DataWrite('F');                  //Print units
}

void Clock_Function(int Speed)
{
    second = 0;

    //fast
    if(Speed == 1)
    {
        Clk_Min++;
        if(Clk_Min == 60)                   //If clock minute is 60
        {
            Clk_Hour++;                     //Increment clock hour
            Clk_Min = 0;                    //Set clock minute to zero
        }
        if(Clk_Hour == 13)                  //If clock hour is 13
            Clk_Hour = 1;                   //Set clock hour to zero
        if((Clk_Hour == 12) && (Clk_Min == 0) && (Clk_Sec == 0))    //If the time is 12:00:00
        {
            if(Clock_Time == 'A')           //If clock time was AM
                Clock_Time = 'P';           //Set clock time to PM
            else if(Clock_Time == 'P')      //If clock time was PM
                Clock_Time = 'A';           //Set clock time to AM
        }

    }
    else if(Speed == 0)
    {
        //Clock time
        Clk_Sec++;                          //Increment clock second
        if(Clk_Sec == 60)                   //If clock second is 60
        {
            Clk_Min++;                      //Increment clock minute
            Clk_Sec = 0;                    //Set clock second to zero
        }
        if(Clk_Min == 60)                   //If clock minute is 60
        {
            Clk_Hour++;                     //Increment clock hour
            Clk_Min = 0;                    //Set clock minute to zero
        }
        if(Clk_Hour == 13)                  //If clock hour is 13
            Clk_Hour = 1;                   //Set clock hour to zero
        if((Clk_Hour == 12) && (Clk_Min == 0) && (Clk_Sec == 0))    //If the time is 12:00:00
        {
            if(Clock_Time == 'A')           //If clock time was AM
                Clock_Time = 'P';           //Set clock time to PM
            else if(Clock_Time == 'P')      //If clock time was PM
                Clock_Time = 'A';           //Set clock time to AM
        }

    }

    if((Alarm_Status == 1) | (Alarm_Status == 2))           //If alarm is on or snoozed
    {
        if(Clock_Time == Alarm_Time)                        //If the alarm and clock times are in AM or PM
        {
            Clk_Time = (Clk_Hour * 60) + Clk_Min;           //Calculate clock time in minutes
            Ala_Time = (Alr_Hour * 60) + Alr_Min;           //Calculate alarm time in minutes
            Light_Time = Ala_Time - Clk_Time;               //Light time is equal to the difference between
        }                                                   //alarm time and clock time
        else if(Clock_Time != Alarm_Time)                   //If the alarm and clock time differ (AM and PM)
        {
            if(Clk_Hour < Alr_Hour)
            {
                Hour = Alr_Hour - Clk_Hour;                     //Calculate the hour difference between clock and alarm time
                if(Hour == 0)
                    Hour = 4;
                Clk_Time = Clk_Min;                         //Clock time is set to clock minute
                Ala_Time = (60 * Hour) + Alr_Min;           //Alarm time is set equal to hour difference and
                //alarm minute in minutes
                Light_Time = Ala_Time - Clk_Time;           //Light time is equal to the difference between
            }
            else if(Alr_Hour <= Clk_Hour)
            {
                Hour = Clk_Hour - Alr_Hour;                     //Calculate the hour difference between clock and alarm time
                if(Hour == 0)
                    Hour = 4;
                Clk_Time = Clk_Min;                         //Clock time is set to clock minute
                Ala_Time = (60 * Hour) + Alr_Min;           //Alarm time is set equal to hour difference and
                //alarm minute in minutes
                Light_Time = Ala_Time - Clk_Time;           //Light time is equal to the difference between
            }
        }                                                   //alarm time and clock time

        //Reset flag when the alarm would have gone off
        if((Alr_Min == Clk_Min) && (Alr_Hour == Clk_Hour) && (Light_Time == 0) && (Light_Time_Flag != 0))  //If alarm time is equal to clock time
        {
            TIMER_A0 -> CCR[3]  = 18750;                    //Set brightness for Green LED to 100%
            TIMER_A0 -> CCR[4]  = 18750;                    //Set brightness for Blue LED to 100%
            AlarmGoingOff = 1;                              //Set alarm to on
            Light_Time = 10;                                //Set light time equal to 10
            SetupTimer32ForAlarm();                        //Begin alarm sound
        }

        if(Light_Time == 6)                                 //If light time is equal to 6 = Pre alarm
        {
            AlarmGoingOff = 0;                              //Set alarm to off
            SetupTimer32ForAlarm();                          //Begin wake up song
        }

        if((Alr_Min == Clk_Min) && (Alr_Hour == Clk_Hour) && (Light_Time_Flag == 0))  //If alarm time is equal to clock time
        {
            Light_Time_Flag = 1;
        }

        if((Light_Time <= 5) && (Light_Time >= 0) && (Light_Time_Flag != 0))           //If light time is within 5 minutes before the alarm
        {
            counter++;                                      //Increment counter
            if(counter == 3)                                //If counter is 3
            {
                Brightness++;                               //Increment brightness
                time_on = (Brightness /100.0) * 18750.0;    //Get PWM duty cycle for brightness of LED
                TIMER_A0 -> CCR[3]  = time_on;              //Set brightness for Green LED
                TIMER_A0 -> CCR[4]  = time_on;              //Set brightness for Blue LED
                counter = 0;                                //Set counter back to zero
            }
        }
    }
}

void Store_Alarm_Status(int Status)
{
    if(Status == 0)                                   //If alarm is off
        sprintf(Str2, "      Off       ");
    else if(Status == 1)                              //If alarm is on
        sprintf(Str2, "       On       ");
    else if(Status == 2)                              //If alarm is snoozed
        sprintf(Str2, "     Snooze     ");
}

void Store_Time(int Clock_Type)
{
    if(Clock_Type == 1)
    {
        //Stores clock time in string
        if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour<10))
            sprintf(Str1,"   %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour<10))
            sprintf(Str1,"   %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour<10))
            sprintf(Str1,"   %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour<10))
            sprintf(Str1,"   %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec<10) && (Clk_Min<10) && (Clk_Hour>9))
            sprintf(Str1,"  %d:0%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec>9) && (Clk_Min>9) && (Clk_Hour>9))
            sprintf(Str1,"  %d:%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec>9) && (Clk_Min<10) && (Clk_Hour>9))
            sprintf(Str1,"  %d:0%d:%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);
        else if((Clk_Sec<10) && (Clk_Min>9) && (Clk_Hour>9))
            sprintf(Str1,"  %d:%d:0%d %cM   ",Clk_Hour,Clk_Min,Clk_Sec,Clock_Time);

    }

    else //if(Clock_Type == 0)
    {
        //Store alarm time in string
        if((Alr_Min>9) && (Alr_Hour>9))
            sprintf(Str3, "    %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
        else if((Alr_Min>9) && (Alr_Hour<10))
            sprintf(Str3, "     %d:%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
        else if((Alr_Min<10) && (Alr_Hour>9))
            sprintf(Str3, "    %d:0%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
        else if((Alr_Min<10) && (Alr_Hour<10))
            sprintf(Str3, "     %d:0%d %cM    ", Alr_Hour, Alr_Min, Alarm_Time);
    }
}

void TimerALEDInit()
{
    //Blue P2.6, Green P2.7
    P2->SEL0 |=  (BIT6|BIT7);
    P2->SEL1 &=~ (BIT6|BIT7);
    P2->DIR  |=  (BIT6|BIT7);
    //P2->OUT  &=~ (BIT6|BIT7);
    TIMER_A0->CCR[0]  = 18750 - 1;
    TIMER_A0->CTL     = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR | TIMER_A_CTL_ID_1;
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;

    //Backlight Init
    TIMER_A1->CCR[0]  = 100;
    TIMER_A1->CTL     = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR | TIMER_A_CTL_ID_1;
    TIMER_A1->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
}


//Sets time and alarm interrupt
void PORT5_IRQHandler(void)
{
    //Set time
    if(P5->IFG & BIT7)
    {
        Flag++;
        Button = 1;
        P5 -> IFG &=~ BIT7;
    }

    //Set alarm
    if(P5->IFG & BIT0)
    {
        Flag++;
        Button = 2;
        P5 -> IFG &=~ BIT0;
    }
}

//Snooze and on/off interrupts
void PORT2_IRQHandler(void)
{
    //Snooze
    if(P2->IFG & BIT4)
    {
        if(Button == 1)
        {
            if(Flag == 1)
            {
                P2->IE &=~ BIT4;
                previous = Clk_Hour;
                Clk_Hour--;
                //when hours reaches 1 am/pm, go to 12 pm/am
                if(Clk_Hour == 0)
                    Clk_Hour = 12;

                if((Clk_Hour == 11) && (previous == 12))
                {
                    if(Clock_Time == 'A')
                        Clock_Time = 'P';
                    else if(Clock_Time == 'P')
                        Clock_Time = 'A';
                }
                P2->IE |= BIT4;
            }
            else if(Flag == 2)
            {
                P2->IE &=~ BIT4;
                Clk_Min--;
                //when minutes reaches -1, go to 59
                if(Clk_Min == -1)
                    Clk_Min = 59;
                P2->IE |= BIT4;
            }
            P2 -> IFG &=~ BIT4;
        }
        else if(Button == 2)
        {
            if(Flag == 1)
            {
                P2->IE &=~ BIT4;
                previous = Alr_Hour;
                Alr_Hour--;
                //when hours reaches 1 am/pm, go to 12 pm/am
                if(Alr_Hour == 0)
                    Alr_Hour = 12;

                if((Alr_Hour == 11) && (previous == 12))
                {
                    if(Alarm_Time == 'A')
                        Alarm_Time = 'P';
                    else if(Alarm_Time == 'P')
                        Alarm_Time = 'A';
                }
                P2->IE |= BIT4;
            }
            else if(Flag == 2)
            {
                P2->IE &=~ BIT4;
                Alr_Min--;
                //when minutes reaches -1, go to 69 and increment hour
                if(Alr_Min == -1)
                    Alr_Min = 59;
                P2->IE |= BIT4;
            }
            P2 -> IFG &=~ BIT4;
        }
        else if (Button == 0)
        {
            P2->IE &=~ BIT4;
            Button = 3;
            P2->IE |= BIT4;
        }

        P2->IE |= BIT4;
        P2 -> IFG &=~ BIT4;
    }

    //On/off
    if(P2->IFG & BIT5)
    {
        if(Button == 1)
        {
            if(Flag == 1)
            {
                P2->IE &=~ BIT5;
                previous = Clk_Hour;
                Clk_Hour++;
                //when hours reaches 12 am/pm, go to 1 pm/am
                if(Clk_Hour == 13)
                    Clk_Hour = 1;

                if((Clk_Hour == 12) && (previous == 11))
                {
                    if(Clock_Time == 'A')
                        Clock_Time = 'P';
                    else if(Clock_Time == 'P')
                        Clock_Time = 'A';
                }
                P2->IE |= BIT5;
            }
            else if(Flag == 2)
            {
                P2->IE &=~ BIT5;
                Clk_Min++;
                //when minutes reaches 60, go to 0
                if(Clk_Min == 60)
                    Clk_Min = 0;
                P2->IE |= BIT5;
            }
            P2 -> IFG &=~ BIT4;
        }
        else if(Button == 2)
        {
            if(Flag == 1)
            {
                P2->IE &=~ BIT5;
                previous = Alr_Hour;
                Alr_Hour++;
                //when hours reaches 12 am/pm, go to 1 pm/am
                if(Alr_Hour == 13)
                    Alr_Hour = 1;

                if((Alr_Hour == 12) && (previous == 11))
                {
                    if(Alarm_Time == 'A')
                        Alarm_Time = 'P';
                    else if(Alarm_Time == 'P')
                        Alarm_Time = 'A';
                }
                P2->IE |= BIT5;
            }
            else if(Flag == 2)
            {
                P2->IE &=~ BIT5;
                Alr_Min++;
                //when minutes reaches 60, go to 0 and increment hour
                if(Alr_Min == 60)
                    Alr_Min = 0;
                P2->IE |= BIT5;
            }
            P2 -> IFG &=~ BIT4;
        }
        else if(Button == 0)
        {
            P2->IE &=~ BIT5;
            Button = 4;
            P2->IE |= BIT5;
        }

        P2 -> IFG &=~ BIT5;
    }

}

void PORT1_IRQHandler(void)
{
    //Normal Speed
    if(P1->IFG & BIT1)
    {
        Speed_Flag = 0;
        P1 -> IFG &=~ BIT1;
    }

    //Fast Speed
    if(P1->IFG & BIT4)
    {
        Speed_Flag = 1;
        P1 -> IFG &=~ BIT4;
    }
}

void InitInterrupts()
{
    //Speed button interrupts
    P1->SEL0 &=~ (BIT1|BIT4);
    P1->SEL1 &=~ (BIT1|BIT4);
    P1->DIR  &=~  (BIT1|BIT4);
    P1->REN  |=  (BIT1|BIT4);
    P1->OUT  |=  (BIT1|BIT4);
    P1->IES  |=  (BIT1|BIT4);
    P1->IE   |=  (BIT1|BIT4);
    P1->IFG   =  0;
    NVIC ->ISER[1] = 1 << ((PORT1_IRQn) & 31);
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
    P5->DIR  &=~  BIT0;
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
    P5 -> SEL0 |= (BIT2);//|BIT4);
    P5 -> SEL1 |= (BIT2);//|BIT4);
    ADC14 -> CTL0    = 0x00000010;  //Power on and disable
    ADC14 -> CTL0   |= 0x04270210;  //
    ADC14 -> CTL1   |= 0x00000030;  //14 bit resolution
    ADC14 -> MCTL[0] = 3;           //A1 input, single-ended
    //ADC14 -> MCTL[1] = 1;           //A1 input, single-ended
    ADC14 -> CTL1   |= 0x00020000;  //Convert for mem reg 2
    ADC14 -> CTL0   |= 2;           //Enable ADC
}

void T32_INT1_IRQHandler()
{
    TIMER32_1->INTCLR = 1;          //Clear interrupt flag so it does not interrupt again immediately

    if(Button != 1)
    {
        second = 1;
    }
    else if(Button == 1)
    {
        second = 0;
    }

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
    if(AlarmGoingOff == 1)
    {
        TIMER_A0 -> CCR[3] = 0;
        TIMER_A0 -> CCR[4] = 0;
        TIMER32_2->INTCLR = 1;                                              //Clear interrupt flag so it does not interrupt again immediately.
        if(breath) {                                                        //Provides separation between notes
            TIMER_A2->CCR[0] = 0;                                           //Set output of TimerA to 0
            TIMER_A2->CCR[1] = 0;
            TIMER_A2->CCR[2] = 0;
            TIMER32_2->LOAD = BREATH_TIME;                                  //Load in breath time to interrupt again
            breath = 0;                                                     //Next Timer32 interrupt is no longer a breath, but is a note
        }
        else {                                                              //If not a breath (a note)
            TIMER32_2->LOAD = music_note_sequence_A[note][1] - 1;       //Load into interrupt count down the length of this note
            if(music_note_sequence_A[note][0] == REST) {                //If note is actually a rest, load in nothing to TimerA
                TIMER_A2->CCR[0] = 0;
                TIMER_A2->CCR[1] = 0;
                TIMER_A2->CCR[2] = 0;
            }
            else {
                TIMER_A2->CCR[0] = 3000000 / music_note_sequence_A[note][0];  //Math in an interrupt is bad behavior, but shows how things are happening.  This takes our clock and divides by the frequency of this note to get the period.
                TIMER_A2->CCR[1] = 1500000 / music_note_sequence_A[note][0];  //50% duty cycle
                TIMER_A2->CCR[2] = TIMER_A0->CCR[0];                          //Had this in here for fun with interrupts.  Not used right now
            }
            note = note + 1;                                                  //Next note
            if(note >= MAX_NOTE_A) {                                          //Go back to the beginning if at the end
                note = 0;
            }
            breath = 1;                                                       //Next time through should be a breath for separation.
        }
    }

    else if(AlarmGoingOff == 0)
    {
        TIMER32_2->INTCLR = 1;                                              //Clear interrupt flag so it does not interrupt again immediately.
        if(breath) {                                                        //Provides separation between notes
            TIMER_A2->CCR[0] = 0;                                           //Set output of TimerA to 0
            TIMER_A2->CCR[1] = 0;
            TIMER_A2->CCR[2] = 0;
            TIMER32_2->LOAD = BREATH_TIME;                                  //Load in breath time to interrupt again
            breath = 0;                                                     //Next Timer32 interrupt is no longer a breath, but is a note
        }
        else {                                                              //If not a breath (a note)
            TIMER32_2->LOAD = music_note_sequence[note][1] - 1;             //Load into interrupt count down the length of this note
            if(music_note_sequence[note][0] == REST) {                      //If note is actually a rest, load in nothing to TimerA
                TIMER_A2->CCR[0] = 0;
                TIMER_A2->CCR[1] = 0;
                TIMER_A2->CCR[2] = 0;
            }
            else {
                TIMER_A2->CCR[0] = 3000000 / music_note_sequence[note][0];  //Math in an interrupt is bad behavior, but shows how things are happening.  This takes our clock and divides by the frequency of this note to get the period.
                TIMER_A2->CCR[1] = 1500000 / music_note_sequence[note][0];  //50% duty cycle
                TIMER_A2->CCR[2] = TIMER_A0->CCR[0];                        //Had this in here for fun with interrupts.  Not used right now
            }
            note = note + 1;                                                //Next note
            if(note >= MAX_NOTE_S) {                                        //Go back to the beginning if at the end
                note = 0;
            }
            breath = 1;                                                     //Next time through should be a breath for separation.
        }
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
    TIMER32_2->LOAD = 3000000 - 1;          //Set to a count down of 1 second on 3 MHz clock
    NVIC_EnableIRQ(T32_INT2_IRQn);          //Enable Timer32_2 interrupt
    TIMER_A2->CCR[0] = 0;                   // Turn off to start
    TIMER_A2->CCTL[1] = 0b0000000011110100; // Setup Timer A0_1 Reset/Set, Interrupt, No Output
    TIMER_A2->CCR[1] = 0;                   // Turn off timerA to start
   // TIMER_A0->CCR[2] = 0;                   // Turn off timerA to start
    TIMER_A2->CTL = 0b0000001000010100;     // Count Up mode using SMCLK, Clears, Clear Interrupt Flag
    NVIC_EnableIRQ(TA0_N_IRQn);             // Enable interrupts for CCTL1-6 (if on)

    TIMER_A3->CCR[0]  = 18750;
    TIMER_A3->CTL     = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR | TIMER_A_CTL_ID_1;
    TIMER_A3->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    NVIC_EnableIRQ(TA3_0_IRQn);             // Enable interrupts for CCTL1-6 (if on)
}

 //Function prints input string to user (via Tera Term)
void writeOutput(char *string)
{
    //Determines the location in the char array "string" that is being written to
    int i = 0;

    while(string[i] != '\0') {
        EUSCI_A0->TXBUF = string[i];
        i++;
        while(!(EUSCI_A0->IFG & BIT1));
    }
}

//Functions reads the input from the user (via Tera Term)
void readInput(char *string)
{
    //Determines the location in the char array "string" that is being written to
    int i=0;

    do
    {
        //If a new line hasn't been found yet, but the program is caught up to what has been received,
        //wait here for new data
        while(read_location == storage_location && INPUT_BUFFER[read_location] != '\n');
        //Manual copy of valid character into "string"
        string[i] = INPUT_BUFFER[read_location];
        INPUT_BUFFER[read_location] = '\0';
        //Increment the location in "string" for next piece of data
        i++;
        //Increment location in INPUT_BUFFER that has been read
        read_location++;
        //If the end of INPUT_BUFFER has been reached, loop back to 0
        if(read_location == BUFFER_SIZE)
            read_location = 0;
    }
    //If a line break was just read, break out of the do-while loop
    while(string[i-1] != '\n');
    //Replace the \n with a \0 to end the string when returning this function
    string[i-1] = '\0';
}

//Interrupt handler function
void EUSCIA0_IRQHandler(void)
{
    if(EUSCI_A0->RXBUF == '\n')
    {
        Serial_Var = 1;
    }


    //Interrupt on the receive line
    if (EUSCI_A0->IFG & BIT0)
    {
        //Store the new piece of data at the present location in the buffer
        INPUT_BUFFER[storage_location] = EUSCI_A0->RXBUF;
        //Clear the interrupt flag right away in case new data is ready
        EUSCI_A0->IFG &= ~BIT0;
        //Update to the next position in the buffer
        storage_location++;
        //If the end of the buffer was reached, loop back to the start
        if(storage_location == BUFFER_SIZE)
            storage_location = 0;
    }
}

void setupSerial()
{
    //P1.2 and P1.3 are EUSCI_A0 RX and TX respectively
    P1->SEL0 |=  (BIT2 | BIT3);
    P1->SEL1 &= ~(BIT2 | BIT3);
    //Disables EUSCI. Default configuration is 8N1
    EUSCI_A0->CTLW0  = BIT0;
    //Connects to SMCLK BIT[7:6] = 10
    EUSCI_A0->CTLW0 |= BIT7;
    //UCBR Value from above
    EUSCI_A0->BRW = 19;
    //UCBRS (Bits 15-8) & UCBRF (Bits 7-4) & UCOS16 (Bit 0)
    EUSCI_A0->MCTLW = 0xAA81;
    //Enable EUSCI
    EUSCI_A0->CTLW0 &= ~BIT0;
    //Clear interrupt
    EUSCI_A0->IFG &= ~BIT0;
    //Enable interrupt
    EUSCI_A0->IE |= BIT0;
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}

