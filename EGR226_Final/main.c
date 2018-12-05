/* Name: Kendra Marckini
 * Date: 11/11/2018
 * Class: EGR 226-902
 * Program: Final Project */

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
#define MAX_NOTE_A  76

//Global variables
int note = 0;
int breath = 0;
float music_note_sequence_A[][2]={{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},
        {C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF},{C4,DOT_HALF},{0,DOT_HALF}};

float music_note_sequence[][2] = {//Chorus, 39 notes
        {E5,EIGHTH},{G5,EIGHTH},{G5,EIGHTH},{C6,QUAR},{B5,QUAR},{G5,QUAR},{G5,QUAR},{A5,SIXTH},{G5,SIXTH},{F5,QUAR},{0,QUAR},{E5,EIGHTH},
        {G5,EIGHTH},{G5,EIGHTH},{C6,QUAR},{B5,QUAR},{G5,QUAR},{E5,EIGHTH},{D5,QUAR},{0,HALF},{E5,EIGHTH},{G5,EIGHTH},{G5,EIGHTH},{C6,QUAR},
        {B5,QUAR},{G5,QUAR},{G5,QUAR},{A5,SIXTH},{G5,SIXTH},{F5,QUAR},{0,QUAR},{E5,EIGHTH},{D5,EIGHTH},{E5,QUAR},{D5,EIGHTH},{C5,EIGHTH},{C5,DOT_HALF},{0,QUAR},
                                 //Post chorus, 35 notes
        {A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},{C4,DOT_QUAR},{A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},{D4,DOT_QUAR},{E4,EIGHTH},{G4,EIGHTH},{E4,SIXTH},
        {D4,SIXTH},{C4,EIGHTH},{D4,EIGHTH},{G3,DOT_QUAR},{D4,EIGHTH},{F3_SHARP,DOT_QUAR},{A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},{C4,DOT_QUAR},
        {A3,EIGHTH},{G3,EIGHTH},{A3,EIGHTH},{D4,DOT_QUAR},{E4,EIGHTH},{G4,EIGHTH},{E4,SIXTH},{D4,SIXTH},{C4,EIGHTH},{D4,EIGHTH},{G3,DOT_QUAR},
        {D4,EIGHTH},{F3_SHARP,DOT_QUAR},{A3,HALF},{0, QUAR}};

void Print_To_LCD(char *String1, char *String2, char *String3, char *String4, int length);
void Clock_Function(int Speed);
void Store_Alarm_Status(int Status);
void Store_Time(int Clock_Type);
void Init_ADC14(void);
void SetupTimer32ForSound();
void SetUpTimer32ForCounter();
void InitInterrupts();
void TimerALEDInit();
void setupSerial();
void readInput(char *string);
void writeOutput(char *string);
void GetTemp();

//Alarm and time defines
volatile uint32_t second=1;

//1: set clock time, 2: set alarm time, 3: snooze, 4: on/off
unsigned int Speed_Flag=0;

int Clk_Hour=10, Clk_Min=8, Clk_Sec=50;
int Alr_Hour=10, Alr_Min=15;
int Light_Time=6, Ala_Time=6, Clk_Time=6;
int PreAlr_Hour=0, PreAlr_Min=0;
int counter=0, AlarmGoingOff=0;
int Hour=0, previous=0;

//AM/PM
static char Clock_Time='P', Alarm_Time='P';
static char PreAlr_Time='A';

//0 = off, 1 = on, 2 = snooze, 3 = no alarm
unsigned int Alarm_Status=1;

//Defines strings to print to LCD
char Str1[16]="", Str2[16]="", Str3[16]="", Str4[16]="", Str5[16]="";

unsigned int b=0;
int Alarm_Flag = 0;
int Time_Diff_Flag = 0;
int Var = 0;

//Defines for setting LED brightness
float time_on=0, Brightness=0;

float previous_brightness = 0;
float Back_Light = 0;

int Previous_Button_Press = 0;

volatile int Flag=0;
volatile int Button=0;
int Sound_Flag = 0;
int Buzzer_Flag = 0;

#define BUFFER_SIZE 100
unsigned int Serial_Flag=0;
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
    INPUT_BUFFER[0]='\0';
    float NADC = 0.0;
    static volatile uint16_t Result;

    PreAlr_Hour = Alr_Hour;
    PreAlr_Min = Alr_Min;

    SetupSysTick();
    SetupPort4();
    SetupLCD();
    InitInterrupts();
    TimerALEDInit();
    Init_ADC14();
    TimerALEDInit();
    setupSerial();
    __enable_interrupt();
    SetUpTimer32ForCounter();

    if((Alarm_Status == 1) || (Alarm_Status == 2)){             //If alarm is on or snoozed
            if(Clock_Time == Alarm_Time)                            //If the alarm and clock times are in AM or PM
                Time_Diff_Flag = 1;                                 //alarm time and clock time
            else if((Clock_Time == 'A') && (Alarm_Time == 'P')){    //If the alarm and clock time differ (AM and PM)
                if((Clk_Hour == 11) && (Alr_Hour == 12))
                    Time_Diff_Flag = 2;
                else if((Clk_Hour == 12) && (Alr_Hour == 1))
                    Time_Diff_Flag = 2;
            }
            else if((Clock_Time == 'P') && (Alarm_Time == 'A')){    //If the alarm and clock time differ (AM and PM)
                if((Clk_Hour == 11) && (Alr_Hour == 12))
                    Time_Diff_Flag = 2;
                else if((Clk_Hour == 12) && (Alr_Hour == 1))
                    Time_Diff_Flag = 2;
            }
        }

        if(Time_Diff_Flag == 1)
        {
            Clk_Time = (Clk_Hour * 60) + Clk_Min;           //Calculate clock time in minutes
            Ala_Time = (Alr_Hour * 60) + Alr_Min;           //Calculate alarm time in minutes
            Light_Time = Ala_Time - Clk_Time;               //Light time is equal to the difference between
        }
        else if(Time_Diff_Flag == 2)
        {
            Clk_Time = Clk_Min;                             //Calculate clock time in minutes
            Ala_Time = 60 + Alr_Min;                        //Calculate alarm time in minutes
            Light_Time = Ala_Time - Clk_Time;               //Light time is equal to the difference between
        }

        if((Light_Time <= 5) && (Light_Time > 0)) {           //If light time is equal to 6 = Pre alarm
            SetupTimer32ForSound();
            Sound_Flag = 1;
            }

	while(1) {
	    if(Serial_Flag) {               //check serial flag here
	        readInput(string);          //Read the input up to "\n" and store in string
	        if(string[0] != '\0') {     //If statement will continue to run until it reaches the end
	            if(string[0] == 'S') {
	                if(string[3] == 'T') {
	                    Clk_Hour = ((string[8] - 48) * 10) + (string[9] - 48);
	                    Clk_Min  = ((string[11] - 48) * 10) + (string[12] - 48);
	                    Clk_Sec  = ((string[14] - 48) * 10) + (string[15] - 48);
	                    if(Clk_Hour == 12)
	                        Clock_Time = 'P';
	                    if((Clk_Hour > 12) && (Clk_Hour <= 23)) {
	                        Clock_Time = 'P';
	                        Clk_Hour = Clk_Hour - 12;
	                    }
	                    else if((Clk_Hour < 12) && (Clk_Hour >= 0)) {
	                        Clock_Time = 'A';
	                        if (Clk_Hour == 0)
	                            Clk_Hour = 12;
	                    }
	                    Store_Time(1);
	                    //Printing to LCD
	                    ComWrite(0x02);                  //Home cursor
	                    PrintString(Str1);               //Print first string
	                    delay_micro(100);
	                    writeOutput("\n");
	                }
	                else if(string[3] == 'A') {
	                    Alr_Hour = ((string[9] - 48) * 10) + (string[10] - 48);
	                    Alr_Min  = ((string[12] - 48) * 10) + (string[13] - 48);
	                    if(Alr_Hour == 12)
	                        Alarm_Time = 'P';
	                    if((Alr_Hour > 12) && (Alr_Hour <= 23)) {
	                        Alarm_Time = 'P';
	                        Alr_Hour = Alr_Hour - 12;
	                    }
	                    else if((Alr_Hour < 12) && (Alr_Hour >= 0)) {
	                        Alarm_Time = 'A';
	                        if (Alr_Hour == 0)
	                            Alr_Hour = 12;
	                    }
	                    Store_Time(0);
	                    ComWrite(0x90);                  //Moves cursor to third line on LCD
	                    PrintString(Str3);               //Print third string
	                    delay_micro(100);
	                    writeOutput("\n");
	                }
	                else {
	                    writeOutput("Invalid -> Not a set command ");
	                    writeOutput(string);
	                    writeOutput("\n\n");
	                }
	            }
	            else if(string[0] == 'R') {
	                if(string[4] == 'T') {
	                    Read_Clk_Hour = Clk_Hour;
	                    Read_Clk_Min = Clk_Min;
	                    Read_Clk_Sec = Clk_Sec;
	                    if((Clock_Time == 'P') && (Read_Clk_Hour != 12))
	                        Read_Clk_Hour = Read_Clk_Hour + 12;
	                    if((Read_Clk_Hour == 12) && (Clock_Time == 'A'))
	                        Read_Clk_Hour = 0;
	                    if((Read_Clk_Hour == 12) && (Clock_Time == 'P'))
	                        Read_Clk_Hour = 12;

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
	                else if(string[4] == 'A') {
	                    Read_Alr_Hour = Alr_Hour;
	                    Read_Alr_Min = Alr_Min;
	                    if((Alarm_Time == 'P') && (Read_Alr_Hour != 12))
	                        Read_Alr_Hour = Read_Alr_Hour + 12;
	                    if((Read_Alr_Hour == 12) && (Alarm_Time == 'A'))
	                        Read_Alr_Hour = 0;
                        if((Read_Alr_Hour == 12) && (Alarm_Time == 'P'))
                            Read_Alr_Hour = 12;
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
	                else {
	                    writeOutput("Invalid ->  Not a Read Command: ");
	                    writeOutput(string);
	                    writeOutput("\n\n");
	                }
	            }
	            else {
	                writeOutput("Invalid -> Not a command: ");
	                writeOutput(string);
	                writeOutput("\n\n");
	            }
	        }
	        Serial_Flag = 0;
	    }

	    if(AlarmGoingOff == 0)
	    {
	        ADC14->CTL0 |= 1;                   //Start the conversation
	        while((!ADC14 -> IFGR0) &BIT0);             //Wait for conversation to be complete
	        Result = ADC14 -> MEM[2];           //Result is set equal to the value from the ADC
	        NADC = (Result * 3.3) / 16384.0;    //nADC is set equal to the conversion
	        Back_Light = (NADC/3.3) * 18750.0;
	        TIMER_A0 -> CCR[1] = Back_Light;
	        ADC14->CTL0 &=~ 2;
	        ADC14->CTL0 |=  2;
	        previous_brightness = Back_Light;
	    }

	    if(second)                          //do main clock stuff here
	        Clock_Function(Speed_Flag);

	    switch (Button) {
	        //Set time
	        case 1:
	            if(Flag == 1) {
	                Previous_Button_Press = 1;
	                //flash hours
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
	                ComWrite(0x02);                 //Moves cursor to first line on LCD
                    PrintString(Str1);              //Print third string
                    delay_milli(500);
                    Store_Time(1);
                    ComWrite(0x02);                 //Moves cursor to first line on LCD
                    PrintString(Str1);              //Print third string
                    delay_milli(500);
	            }
	            if(Flag == 2) {
	                Previous_Button_Press = 1;
	                //Flash minutes
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
                    Store_Time(1);
                    ComWrite(0x02);                  //Moves cursor to first line on LCD
                    PrintString(Str1);               //Print third string
                    delay_milli(500);
	            }
	            if(Flag == 3) {
	                Store_Time(1);
	                Flag = 0;
	                Button = 0;
	                Previous_Button_Press = 0;
	            }
	            break;


	        //Set alarm
	        case 2:
                if(Flag == 1) {
                    Previous_Button_Press = 2;
                    //flash hours
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
                    Store_Time(0);
                    ComWrite(0x90);                  //Moves cursor to third line on LCD
                    PrintString(Str3);               //Print third string
                    delay_milli(500);
                }
                if(Flag == 2) {
                    Previous_Button_Press = 2;
                    //Flash minutes
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
                    Store_Time(0);
                    ComWrite(0x90);                  //Moves cursor to third line on LCD
                    PrintString(Str3);               //Print third string
                    delay_milli(500);
                }
                if(Flag == 3) {
                    Store_Time(0);
                    PreAlr_Hour = Alr_Hour;
                    PreAlr_Min = Alr_Min;
                    PreAlr_Time = Alarm_Time;

                    Flag = 0;
                    Button = 0;
                    Previous_Button_Press = 0;
                    Alarm_Flag = 0;
                    Sound_Flag = 0;
                    Light_Time = 0;
                }
                break;

	        //Snooze 0 = off, 1 = on, 2 = snooze, 3 = no alarm
	        case 3:
                if((Alarm_Status == 1) || (Alarm_Status == 2)) {    //If the alarm is on or is snoozed
                    //Alarm must be going off
                    if(AlarmGoingOff == 1) {
                        //Store previous alarm
                        PreAlr_Hour = Alr_Hour;                     //Reset alarm hour
                        PreAlr_Min = Alr_Min;                       //Reset alarm minute
                        PreAlr_Time = Alarm_Time;                   //Reset alarm time

                        counter = 0;
                        Brightness = 0;
                        Alarm_Status = 2;                           //Alarm status is snooze
                        AlarmGoingOff = 0;
                        counter = 0;
                        TIMER_A0  -> CCR[3] = 0;                    //Turn off LEDs
                        TIMER_A0  -> CCR[4] = 0;
                        TIMER32_2 -> CONTROL = 0;                   //Turn off sound
                        TIMER32_2 -> LOAD    = 0;
                        TIMER_A2 -> CCR[0] = 0;
                        TIMER_A2 -> CCTL[1] = 0;
                        TIMER_A2 -> CCR[1] = 0;
                        TIMER_A2 -> CTL = 0;
                        Brightness = 0;
                        Sound_Flag = 0;
                        TIMER_A0 -> CCR[1] = previous_brightness;  //Reset brightness

                        Alr_Hour = Clk_Hour;
                        Alarm_Time = Clock_Time;
                        Alr_Min = Clk_Min + 10;                     //Set new alarm for 10 minutes

                        if(Alr_Min > 59) {
                            Alr_Min = Alr_Min - 60;
                            Alr_Hour++;
                        }
                        if(Alr_Hour == 13)
                            Alr_Hour = 1;
                        if((Alr_Hour == 12) && (Alr_Min < 11)) {
                            if(Alarm_Time == 'A')
                                Alarm_Time = 'P';
                            else if(Alarm_Time == 'P')
                                Alarm_Time = 'A';
                        }
                    }
                }
                Flag = 0;
                Button = 0;
                break;

	        //On/Off
                //0 0ff, 1 on, 2 snooze
            case 4:
                if(Alarm_Status == 0) {
                    Alarm_Status = 1;
                    AlarmGoingOff = 0;
                    Alarm_Flag = 0;
                    Sound_Flag = 0;
                    Buzzer_Flag = 0;
                }

                else if((Alarm_Status == 1) || (Alarm_Status == 2)) {
                    //pre-alarm
                    if((Light_Time <= 5) && (Light_Time >= 0) && (AlarmGoingOff == 0))
                    {
                        Alarm_Status = 1;                           //Set alarm to on

                        counter = 0;

                        TIMER_A0  -> CCR[3] = 0;                    //Turn off LEDs
                        TIMER_A0  -> CCR[4] = 0;
                        TIMER_A0  -> CCR[3] = 0;                    //Turn off LEDs
                        TIMER_A0  -> CCR[4] = 0;
                        TIMER32_2 -> CONTROL = 0;                   //Turn off sound
                        TIMER32_2 -> LOAD    = 0;
                        TIMER_A2 -> CCR[0] = 0;
                        TIMER_A2 -> CCTL[1] = 0;
                        TIMER_A2 -> CCR[1] = 0;
                        TIMER_A2 -> CTL = 0;
                        Sound_Flag = 0;
                        Brightness = 0;
                        counter = 0;

                        Alr_Hour = PreAlr_Hour;                     //Reset alarm hour
                        Alr_Min = PreAlr_Min;                       //Reset alarm minute
                        Alarm_Time = PreAlr_Time;                   //Reset alarm time

                        Alarm_Flag = 1;                             //Set flag to not make alarm go off
                    }

                    //sounding alarm
                    else if(AlarmGoingOff == 1)
                    {
                        counter = 0;
                        Alarm_Status = 1;                           //Alarm status is on
                        AlarmGoingOff = 0;
                        Sound_Flag = 0;
                        Brightness = 0;

                        TIMER_A0  -> CCR[3] = 0;                    //Turn off LEDs
                        TIMER_A0  -> CCR[4] = 0;
                        TIMER32_2 -> CONTROL = 0;                   //Turn off sound
                        TIMER32_2 -> LOAD    = 0;
                        TIMER_A2 -> CCR[0] = 0;
                        TIMER_A2 -> CCTL[1] = 0;
                        TIMER_A2 -> CCR[1] = 0;
                        TIMER_A2 -> CTL = 0;

                        TIMER_A0 -> CCR[1] = previous_brightness;  //Reset brightness

                        Alr_Hour = PreAlr_Hour;                     //Reset alarm hour
                        Alr_Min = PreAlr_Min;                       //Reset alarm minute
                        Alarm_Time = PreAlr_Time;                   //Reset alarm time

                        Alarm_Flag = 0;                             //Set flag to not make alarm go off
                    }
                    else if((AlarmGoingOff == 0) && ((Light_Time > 5) || (Light_Time < 0)))
                         {
                        Alarm_Flag = 0;                             //Set flag to not make alarm go off
                        Alarm_Status = 0;
                        Alr_Hour = PreAlr_Hour;                     //Reset alarm hour
                        Alr_Min = PreAlr_Min;                       //Reset alarm minute
                        Alarm_Time = PreAlr_Time;                   //Reset alarm time
                        counter = 0;
                        Brightness = 0;
                    }
                }
                Flag = 0;
                Button = 0;
                break;

                }

        Store_Time(1);
        Store_Alarm_Status(Alarm_Status);
        Store_Time(0);
        GetTemp();
        Print_To_LCD(Str1, Str2, Str3, Str4, b);
	}
}

void Clock_Function(int Speed)
{
    second = 0;
    //fast
    if(Speed == 1)
    {
        Clk_Min++;
        if(Clk_Min == 60) {                 //If clock minute is 60
            Clk_Hour++;                     //Increment clock hour
            Clk_Min = 0;                    //Set clock minute to zero
        }
        if(Clk_Hour == 13)                  //If clock hour is 13
            Clk_Hour = 1;                   //Set clock hour to zero
        if((Clk_Hour == 12) && (Clk_Min == 0) && (Clk_Sec == 0)) {  //If the time is 12:00:00
            if(Clock_Time == 'A')           //If clock time was AM
                Clock_Time = 'P';           //Set clock time to PM
            else if(Clock_Time == 'P')      //If clock time was PM
                Clock_Time = 'A';           //Set clock time to AM
        }
    }
    else if(Speed == 0) {
        //Clock time
        Clk_Sec++;                          //Increment clock second
        if(Clk_Sec == 60) {                 //If clock second is 60
            Clk_Min++;                      //Increment clock minute
            Clk_Sec = 0;                    //Set clock second to zero
        }
        if(Clk_Min == 60) {                 //If clock minute is 60
            Clk_Hour++;                     //Increment clock hour
            Clk_Min = 0;                    //Set clock minute to zero
        }
        if(Clk_Hour == 13)                  //If clock hour is 13
            Clk_Hour = 1;                   //Set clock hour to zero
        if((Clk_Hour == 12) && (Clk_Min == 0) && (Clk_Sec == 0)) {  //If the time is 12:00:00
            if(Clock_Time == 'A')           //If clock time was AM
                Clock_Time = 'P';           //Set clock time to PM
            else if(Clock_Time == 'P')      //If clock time was PM
                Clock_Time = 'A';           //Set clock time to AM
        }
    }

    if((Alarm_Status == 1) || (Alarm_Status == 2)){             //If alarm is on or snoozed
        if(Clock_Time == Alarm_Time)                            //If the alarm and clock times are in AM or PM
            Time_Diff_Flag = 1;                                 //alarm time and clock time
        else if((Clock_Time == 'A') && (Alarm_Time == 'P')){    //If the alarm and clock time differ (AM and PM)
            if((Clk_Hour == 11) && (Alr_Hour == 12))
                Time_Diff_Flag = 2;
            else if((Clk_Hour == 12) && (Alr_Hour == 1))
                Time_Diff_Flag = 2;
        }
        else if((Clock_Time == 'P') && (Alarm_Time == 'A')){    //If the alarm and clock time differ (AM and PM)
            if((Clk_Hour == 11) && (Alr_Hour == 12))
                Time_Diff_Flag = 2;
            else if((Clk_Hour == 12) && (Alr_Hour == 1))
                Time_Diff_Flag = 2;
        }
    }

    if(Time_Diff_Flag == 1)
    {
        Clk_Time = (Clk_Hour * 60) + Clk_Min;           //Calculate clock time in minutes
        Ala_Time = (Alr_Hour * 60) + Alr_Min;           //Calculate alarm time in minutes
        Light_Time = Ala_Time - Clk_Time;               //Light time is equal to the difference between
    }
    else if(Time_Diff_Flag == 2)
    {
        Clk_Time = Clk_Min;                             //Calculate clock time in minutes
        Ala_Time = 60 + Alr_Min;                        //Calculate alarm time in minutes
        Light_Time = Ala_Time - Clk_Time;               //Light time is equal to the difference between
    }

    if((Light_Time == 5) && (Clk_Sec == 0)) {           //If light time is equal to 6 = Pre alarm
        counter--;
        Sound_Flag = 1;
        SetupTimer32ForSound();
    }

    if((Sound_Flag == 0) && (Light_Time <= 5) && (Alarm_Flag == 0))
    {
        Sound_Flag = 1;
        SetupTimer32ForSound();
    }

    if((Speed == 0) && (Light_Time <= 5) && (Alarm_Flag == 0)) {    //If light time is within 5 minutes before the alarm

        counter++;                                                  //Increment counter
        if(Light_Time > 0)
        {
            if(counter == 3) {                              //If counter is 3
                Brightness++;                               //Increment brightness
                time_on = (Brightness /100.0) * 18750.0;    //Get PWM duty cycle for brightness of LED
                TIMER_A0 -> CCR[3]  = time_on;              //Set brightness for Green LED
                TIMER_A0 -> CCR[4]  = time_on;              //Set brightness for Blue LED
                counter = 0;                                //Set counter back to zero
            }
        }
    }
    else if((Speed == 1) && (Light_Time <= 5) && (Alarm_Flag == 0)) {          //If light time is within 5 minutes before the alarm
        counter++;
        if(Light_Time > 0)
        {
            if((counter <= 5) && (counter > 0)) {
                Brightness = Brightness + 20;               //Increment brightness
                time_on = (Brightness /100.0) * 18750.0;    //Get PWM duty cycle for brightness of LED
                TIMER_A0 -> CCR[3]  = time_on;              //Set brightness for Green LED
                TIMER_A0 -> CCR[4]  = time_on;              //Set brightness for Blue LED
                counter = 0;                                //Set counter back to zero
            }
        }
    }

    if((Speed == 1) && (Clk_Min == Alr_Min) && (Clk_Hour == Alr_Hour))
    {
        AlarmGoingOff = 1;
    }


    //if alarm is going off
    if((Alr_Min == Clk_Min) && (Alr_Hour == Clk_Hour) && (Clk_Sec == 0) && (Alarm_Flag == 0) && (Light_Time == 0))
    {
        AlarmGoingOff = 1;                              //Set alarm to on

        TIMER32_2->CONTROL = 0;        //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
        TIMER32_2->LOAD = 0;          //Set to a count down of 1 second on 3 MHz clock
        TIMER_A2->CCR[0] = 0;                   // Turn off to start
        TIMER_A2->CCTL[1] = 0; // Setup Timer A0_1 Reset/Set, Interrupt, No Output
        TIMER_A2->CCR[1] = 0;                   // Turn off timerA to start
        TIMER_A2->CTL = 0;     // Count Up mode using SMCLK, Clears, Clear Interrupt Flag

        previous_brightness = Back_Light;
        TIMER_A0 -> CCR[1]  = 18750;
        TIMER_A0 -> CCR[3]  = 18750;                    //Set brightness for Green LED to 100%
        TIMER_A0 -> CCR[4]  = 18750;                    //Set brightness for Blue LED to 100%

        Light_Time = 6;                                //Set light time equal to 10
        counter = 0;
        Buzzer_Flag = 1;
        SetupTimer32ForSound();                         //Begin alarm sound
    }

    if((AlarmGoingOff == 0) && ((Light_Time > 5) || (Light_Time < 0)))
    {
        TIMER32_2->CONTROL = 0;        //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
        TIMER32_2->LOAD = 0;          //Set to a count down of 1 second on 3 MHz clock
        TIMER_A2->CCR[0] = 0;                   // Turn off to start
        TIMER_A2->CCTL[1] = 0; // Setup Timer A0_1 Reset/Set, Interrupt, No Output
        TIMER_A2->CCR[1] = 0;                   // Turn off timerA to start
        TIMER_A2->CTL = 0;     // Count Up mode using SMCLK, Clears, Clear Interrupt Flag
    }

    if((AlarmGoingOff == 1) && (Buzzer_Flag == 0))
    {
        TIMER32_2->CONTROL = 0;        //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
        TIMER32_2->LOAD = 0;          //Set to a count down of 1 second on 3 MHz clock
        TIMER_A2->CCR[0] = 0;                   // Turn off to start
        TIMER_A2->CCTL[1] = 0; // Setup Timer A0_1 Reset/Set, Interrupt, No Output
        TIMER_A2->CCR[1] = 0;                   // Turn off timerA to start
        TIMER_A2->CTL = 0;     // Count Up mode using SMCLK, Clears, Clear Interrupt Flag

        previous_brightness = Back_Light;

        Light_Time = 6;                                //Set light time equal to 10
        counter = 0;

        Buzzer_Flag = 1;
        SetupTimer32ForSound();                         //Begin alarm sound
    }

    //if Alarm was supposed to go off
    else if((Alr_Min == Clk_Min) && (Alr_Hour == Clk_Hour) && (Clk_Sec == 0) && (Alarm_Flag == 1))
    {
        AlarmGoingOff = 0;
        Alarm_Flag = 0;
    }
}

void Init_ADC14(void)               //This function initializes the ADC
{
    P5 -> SEL0 |= (BIT2|BIT4);      //P5.2 for A3 for temp
    P5 -> SEL1 |= (BIT2|BIT4);      //P5.4 for A1 for brightness

    ADC14 -> CTL0 &=~ ADC14_CTL0_ENC;
    ADC14-> CTL0 = 0b10000100001000100000001110010000;
    ADC14 -> CTL1    = 0x00000030; //14 bit resolution
    ADC14 -> CTL1   |= 0x00000000;  //convert for mem reg 2
    ADC14 -> MCTL[2] = 1;           //A1 input, single-ended
    ADC14 -> MCTL[0] = 3;           //A3 input, single-ended
    ADC14 -> CTL0 |= ADC14_CTL0_ENC;
}

void GetTemp()
{
    float nADC = 0.0, Temp_F = 0.0, Temp_C = 0.0;
    static volatile uint16_t result;

    ADC14->CTL0 |= 1;                       //Start the conversation
    while(!ADC14 -> IFGR0);                 //Wait for conversation to be complete
    result = ADC14 -> MEM[0];               //Result is set equal to the value from the ADC
    nADC = (result * 3.3) / 16384.0;        //nADC is set equal to the conversion
    Temp_C = (nADC * 1000.0) / 10.0;        //Convert voltage to celsius
    Temp_F = (Temp_C * (9.0/5.0)) + 32.0;   //Celsius to fahrenheit
    sprintf(Str4, "%0.1f", Temp_F);         //Moves cursor to the second line on LCD//place fahrenheit into array
    b = strlen(Str4);                       //Calculate length of array
}

void Print_To_LCD(char *String1, char *String2, char *String3, char *String4, int length)
{
    int i=0;
    ComWrite(0x02);                  //Home cursor
    PrintString(String1);            //Print first string
    delay_micro(100);
    ComWrite(0xC0);                  //Moves cursor to second line on LCD
    PrintString(String2);            //Print second string
    delay_micro(100);
    ComWrite(0x90);                  //Moves cursor to third line on LCD
    PrintString(String3);            //Print third string
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

void Store_Alarm_Status(int Status)
{
    if(Status == 0)                         //If alarm is off
        sprintf(Str2, "      Off       ");
    else if(Status == 1)                    //If alarm is on
        sprintf(Str2, "       On       ");
    else if(Status == 2)                    //If alarm is snoozed
        sprintf(Str2, "     Snooze     ");
}

void Store_Time(int Clock_Type)
{
    if(Clock_Type == 1) { //Stores clock time in string
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

    else {    //Store alarm time in string
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
    P2->SEL0 |=  (BIT4|BIT6|BIT7);  //Backlight 2.4, Blue led P2.6, red led P2.7
    P2->SEL1 &=~ (BIT4|BIT6|BIT7);
    P2->DIR  |=  (BIT4|BIT6|BIT7);

    TIMER_A0->CCR[0]  = 18750 - 1;
    TIMER_A0->CTL     = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR | TIMER_A_CTL_ID_1;
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; //backlight
}

void PORT5_IRQHandler(void) //Sets time and alarm interrupt
{
    while (!((P5->IN & BIT1) == BIT1)) {}
    while (!((P5->IN & BIT7) == BIT7)) {}
    if(P5->IFG & BIT7) {  //Set time
        delay_micro(100);
        if((Previous_Button_Press == 0) | (Previous_Button_Press == 1)) {
            Flag++;
            Button = 1;
        }
        P5 -> IFG &=~ BIT7;
    }
    if(P5->IFG & BIT1) {    //Set alarm
        delay_micro(100);
        if((Previous_Button_Press == 0) | (Previous_Button_Press == 2)) {
            Flag++;
            Button = 2;
        }
        P5 -> IFG &=~ BIT1;
    }
}

void PORT2_IRQHandler(void)     //Snooze and on/off interrupts
{
    while (!((P2->IN & BIT3) == BIT3)) {}
    while (!((P2->IN & BIT5) == BIT5)) {}
    if(P2->IFG & BIT3) { //Snooze
        delay_micro(100);
        if(Button == 1) {
            if(Flag == 1) {
                previous = Clk_Hour;
                Clk_Hour--;
                if(Clk_Hour == 0)   //If hours reaches 1 am/pm, go to 12 pm/am
                    Clk_Hour = 12;
                if((Clk_Hour == 11) && (previous == 12)) {
                    if(Clock_Time == 'A')
                        Clock_Time = 'P';
                    else if(Clock_Time == 'P')
                        Clock_Time = 'A';
                }
            }
            else if(Flag == 2) {
                Clk_Min--;
                if(Clk_Min == -1)   //If minutes reaches -1, go to 59
                    Clk_Min = 59;
            }
        }
        else if(Button == 2) {
            if(Flag == 1) {
                previous = Alr_Hour;
                Alr_Hour--;
                if(Alr_Hour == 0)   //If hours reaches 1 am/pm, go to 12 pm/am
                    Alr_Hour = 12;
                if((Alr_Hour == 11) && (previous == 12)) {
                    if(Alarm_Time == 'A')
                        Alarm_Time = 'P';
                    else if(Alarm_Time == 'P')
                        Alarm_Time = 'A';
                }
            }
            else if(Flag == 2) {
                Alr_Min--;
                if(Alr_Min == -1)   //If minutes reaches -1, go to 69 and increment hour
                    Alr_Min = 59;
            }
        }
        else if (Button == 0) {
            Button = 3;
        }
        P2 -> IFG &=~ BIT3;
    }
    if(P2->IFG & BIT5) {  //On/off
        delay_micro(100);
        if(Button == 1) {
            if(Flag == 1) {
                previous = Clk_Hour;
                Clk_Hour++;
                if(Clk_Hour == 13)  //If hours reaches 12 am/pm, go to 1 pm/am
                    Clk_Hour = 1;

                if((Clk_Hour == 12) && (previous == 11)) {
                    if(Clock_Time == 'A')
                        Clock_Time = 'P';
                    else if(Clock_Time == 'P')
                        Clock_Time = 'A';
                }
             }
            else if(Flag == 2) {
                Clk_Min++;
                if(Clk_Min == 60)   //If minutes reaches 60, go to 0
                    Clk_Min = 0;
            }
        }
        else if(Button == 2)
        {
            if(Flag == 1) {
                previous = Alr_Hour;
                Alr_Hour++;
                if(Alr_Hour == 13)  //If hours reaches 12 am/pm, go to 1 pm/am
                    Alr_Hour = 1;
                if((Alr_Hour == 12) && (previous == 11)) {
                    if(Alarm_Time == 'A')
                        Alarm_Time = 'P';
                    else if(Alarm_Time == 'P')
                        Alarm_Time = 'A';
                }
            }
            else if(Flag == 2) {
                Alr_Min++;
                if(Alr_Min == 60)   //If minutes reaches 60, go to 0 and increment hour
                    Alr_Min = 0;
            }
        }
        else if(Button == 0) {
            Button = 4;
        }
        P2 -> IFG &=~ BIT5;
    }
}

void PORT1_IRQHandler(void)
{
    if(P1->IFG & BIT6) {     //Normal Speed
        Speed_Flag = 0;
        P1 -> IFG &=~ BIT6;
    }
    if(P1->IFG & BIT7) {     //Fast Speed
        Speed_Flag = 1;
        P1 -> IFG &=~ BIT7;
    }
}

void InitInterrupts()
{
    //Speed button interrupts
    P1->SEL0 &=~ (BIT6|BIT7);
    P1->SEL1 &=~ (BIT6|BIT7);
    P1->DIR  &=~ (BIT6|BIT7);
    P1->REN  |=  (BIT6|BIT7);
    P1->OUT  |=  (BIT6|BIT7);
    P1->IES  |=  (BIT6|BIT7);
    P1->IE   |=  (BIT6|BIT7);
    P1->IFG   =  0;
    NVIC ->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    //Button interrupt for setting alarm and time
    P5->SEL0 &=~ (BIT1|BIT7);
    P5->SEL1 &=~ (BIT1|BIT7);
    P5->DIR  &=~ (BIT1|BIT7);
    P5->REN  |=  (BIT1|BIT7);
    P5->OUT  |=  (BIT1|BIT7);
    P5->IES  |=  (BIT1|BIT7);
    P5->IE   |=  (BIT1|BIT7);
    P5->IFG   =  0;
    NVIC ->ISER[1] = 1 << ((PORT5_IRQn) & 31);
    //Button interrupt for snooze/down & on/off/up
    P2->SEL0 &=~ (BIT3|BIT5);
    P2->SEL1 &=~ (BIT3|BIT5);
    P2->DIR  &=~ (BIT3|BIT5);
    P2->REN  |=  (BIT3|BIT5);
    P2->OUT  |=  (BIT3|BIT5);
    P2->IES  |=  (BIT3|BIT5);
    P2->IE   |=  (BIT3|BIT5);
    P2->IFG   =  0;
    NVIC ->ISER[1] = 1 << ((PORT2_IRQn) & 31);
}

void T32_INT1_IRQHandler()
{
    TIMER32_1->INTCLR = 1;          //Clear interrupt flag so it does not interrupt again immediately
    if(Button != 1)
        second = 1;
    else if(Button == 1)
        second = 0;
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
        TIMER32_2->INTCLR = 1;                                              //Clear interrupt flag so it does not interrupt again immediately.
        if(breath) {                                                        //Provides separation between notes
            TIMER_A2->CCR[0] = 0;                                           //Set output of TimerA to 0
            TIMER_A2->CCR[1] = 0;
            TIMER_A2->CCR[2] = 0;
            TIMER32_2->LOAD = BREATH_TIME;                                  //Load in breath time to interrupt again
            breath = 0;                                                     //Next Timer32 interrupt is no longer a breath, but is a note
        }
        else {                                                              //If not a breath (a note)
            TIMER32_2->LOAD = music_note_sequence_A[note][1] - 1;             //Load into interrupt count down the length of this note
            if(music_note_sequence_A[note][0] == REST) {                      //If note is actually a rest, load in nothing to TimerA
                TIMER_A2->CCR[0] = 0;
                TIMER_A2->CCR[1] = 0;
                TIMER_A2->CCR[2] = 0;
            }
            else {
                TIMER_A2->CCR[0] = 3000000 / music_note_sequence_A[note][0];  //Math in an interrupt is bad behavior, but shows how things are happening.  This takes our clock and divides by the frequency of this note to get the period.
                TIMER_A2->CCR[1] = 1500000 / music_note_sequence_A[note][0];  //50% duty cycle
                TIMER_A2->CCR[2] = TIMER_A2->CCR[0];                        //Had this in here for fun with interrupts.  Not used right now
            }
            note = note + 1;                                                //Next note
            if(note >= MAX_NOTE_A) {                                        //Go back to the beginning if at the end
                note = 0;
            }
            breath = 1;                                                     //Next time through should be a breath for separation.
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
                TIMER_A2->CCR[2] = TIMER_A2->CCR[0];                        //Had this in here for fun with interrupts.  Not used right now
            }
            note = note + 1;                                                //Next note
            if(note >= MAX_NOTE_S) {                                        //Go back to the beginning if at the end
                note = 0;
            }
            breath = 1;                                                     //Next time through should be a breath for separation.
        }
    }
}

void TA2_N_IRQHandler()
{
    if(TIMER_A2->CCTL[1] & BIT0){}          //If CCTL1 is the reason for the interrupt (BIT0 holds the flag)
}

void SetupTimer32ForSound()
{
    P5->SEL0 |=  (BIT6);
    P5->SEL1 &=~ (BIT6);
    P5->DIR  |=  (BIT6);

    TIMER32_2->CONTROL = 0b11100011;        //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode
    TIMER32_2->LOAD = 3000000 - 1;          //Set to a count down of 1 second on 3 MHz clock
    NVIC_EnableIRQ(T32_INT2_IRQn);          //Enable Timer32_2 interrupt
    TIMER_A2->CCR[0] = 0;                   // Turn off to start
    TIMER_A2->CCTL[1] = 0b0000000011110100; // Setup Timer A0_1 Reset/Set, Interrupt, No Output
    TIMER_A2->CCR[1] = 0;                   // Turn off timerA to start
    TIMER_A2->CTL = 0b0000001000010100;     // Count Up mode using SMCLK, Clears, Clear Interrupt Flag
    NVIC_EnableIRQ(TA0_N_IRQn);             // Enable interrupts for CCTL1-6 (if on)
}

void writeOutput(char *string)              //Function prints input string to user (via Tera Term)
{
    int i = 0;                              //Determines the location in the char array "string" that is being written to
    while(string[i] != '\0') {
        EUSCI_A0->TXBUF = string[i];
        i++;
        while(!(EUSCI_A0->IFG & BIT1));
    }
}

void readInput(char *string)                //Functions reads the input from the user (via Tera Term)
{
    int i=0;                                //Determines the location in the char array "string" that is being written to
    do {
        while(read_location == storage_location && INPUT_BUFFER[read_location] != '\n');    //If a new line hasn't been found yet,
                                                                                            //but the program is caught up to what
                                                                                            //has been received, wait here for new data
        string[i] = INPUT_BUFFER[read_location];    //Manual copy of valid character into "string"
        INPUT_BUFFER[read_location] = '\0';
        i++;                                        //Increment the location in "string" for next piece of data
        read_location++;                            //Increment location in INPUT_BUFFER that has been read
        if(read_location == BUFFER_SIZE)            //If the end of INPUT_BUFFER has been reached, loop back to 0
            read_location = 0;
    }
    while(string[i-1] != '\n');                     //If a line break was just read, break out of the do-while loop
    string[i-1] = '\0';                             //Replace the \n with a \0 to end the string when returning this function
}

void EUSCIA0_IRQHandler(void)                               //Interrupt handler function
{
    if (EUSCI_A0->IFG & BIT0){                               //Interrupt on the receive line
        INPUT_BUFFER[storage_location] = EUSCI_A0->RXBUF;   //Store the new piece of data at the present location in the buffer
        EUSCI_A0->IFG &= ~BIT0;                             //Clear the interrupt flag right away in case new data is ready
        storage_location++;                                 //Update to the next position in the buffer
        if(storage_location == BUFFER_SIZE)                 //If the end of the buffer was reached, loop back to the start
            storage_location = 0;
        if(EUSCI_A0->RXBUF == '\n')
            Serial_Flag = 1;
        else
            Serial_Flag = 0;
    }
}

void setupSerial()
{
    P1->SEL0 |=  (BIT2 | BIT3);     //P1.2 and P1.3 are EUSCI_A0 RX and TX respectively
    P1->SEL1 &= ~(BIT2 | BIT3);
    EUSCI_A0->CTLW0  = BIT0;        //Disables EUSCI. Default configuration is 8N1
    EUSCI_A0->CTLW0 |= BIT7;        //Connects to SMCLK BIT[7:6] = 10
    EUSCI_A0->BRW = 19;             //UCBR Value from above
    EUSCI_A0->MCTLW = 0xAA81;       //UCBRS (Bits 15-8) & UCBRF (Bits 7-4) & UCOS16 (Bit 0)
    EUSCI_A0->CTLW0 &= ~BIT0;       //Enable EUSCI
    EUSCI_A0->IFG &= ~BIT0;         //Clear interrupt
    EUSCI_A0->IE |= BIT0;           //Enable interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}
