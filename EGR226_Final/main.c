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
int note = 0;
int breath = 0;
float music_note_sequence[][2] = {
                                 //chorus, 38 notes
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

//Prototypes
void SetupTimer32s();

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//SetupLCD();
	SetupTimer32s();
	__enable_interrupt();

	while(1){}
}

void T32_INT2_IRQHandler()
{
    TIMER32_2->INTCLR = 1;                                      //Clear interrupt flag so it does not interrupt again immediately.
    if(breath) {                                                //Provides separation between notes
        TIMER_A2->CCR[0] = 0;                                   //Set output of TimerA to 0
        TIMER_A2->CCR[1] = 0;
        TIMER_A2->CCR[2] = 0;
        TIMER32_2->LOAD = BREATH_TIME;                          //Load in breath time to interrupt again
        breath = 0;                                             //Next Timer32 interrupt is no longer a breath, but is a note
    }
    else {                                                      //If not a breath (a note)
        TIMER32_2->LOAD = music_note_sequence[note][1] - 1;     //Load into interrupt count down the length of this note
        if(music_note_sequence[note][0] == REST) {              //If note is actually a rest, load in nothing to TimerA
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
        if(note >= MAX_NOTE) {                                          //Go back to the beginning if at the end
            note = 0;
            TIMER32_2->CONTROL = 0b11000011;                //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode.  See 589 of the reference manual
        }
        breath = 1;                                             //Next time through should be a breath for separation.
    }
}

void TA0_N_IRQHandler()
{
    //If CCTL1 is the reason for the interrupt (BIT0 holds the flag)
    if(TIMER_A2->CCTL[1] & BIT0){}
}

void SetupTimer32s()
{
    P5->SEL0 |=  (BIT6);  //setup pwm pin
    P5->SEL1 &=~ (BIT6);  //setup pwm pin
    P5->DIR  |=  (BIT6);  //pwd pin as output

    TIMER32_2->CONTROL = 0b11100011;                //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode.  See 589 of the reference manual
    NVIC_EnableIRQ(T32_INT2_IRQn);                  //Enable Timer32_2 interrupt.  Look at msp.h if you want to see what all these are called.
    TIMER32_2->LOAD = 3000000 - 1;                  //Set to a count down of 1 second on 3 MHz clock

    TIMER_A2->CCR[0] = 0;                           // Turn off timerA to start
    TIMER_A2->CCTL[1] = 0b0000000011110100;         // Setup Timer A0_1 Reset/Set, Interrupt, No Output
    TIMER_A2->CCR[1] = 0;                           // Turn off timerA to start
    TIMER_A2->CCR[2] = 0;                           // Turn off timerA to start
    TIMER_A2->CTL = 0b0000001000010100;             // Count Up mode using SMCLK, Clears, Clear Interrupt Flag

    NVIC_EnableIRQ(TA0_N_IRQn);                     // Enable interrupts for CCTL1-6 (if on)
}

