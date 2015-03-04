/*	[ifuka001]_lab4_part2.c - 2/4/2015
 *	Name & E-mail: Itsuki Fukada - ifuka001@ucr.edu
 *	CS Login: fukadai
 *	Partner(s) Name & E-mail:  - Eric Tram - etram001@ucr.edu
 *	Lab Section: 23
 *	Assignment: Lab #4  Exercise #2
 *	Exercise Description:
 *	
 *	
 *	I acknowledge all content contained herein, excluding template or example 
 *	code, is my own original work.
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "C:\Users\student\Desktop\includes\io.c"
#include "C:\Users\student\Desktop\includes\seven_seg.h"


volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks


typedef struct task{
	int state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);

}task;

const unsigned char taskNum = 2;
const unsigned char GCDperiod = 100; 
task tasks[2];




void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	// bit3bit1bit0=000: timer off
	TCCR1B = 0x00;
}

void TimerISR() {
	unsigned char i;
	for(i=0; i < taskNum;i++)
	{
		if(tasks[i].elapsedTime >= tasks[i].period)
		{
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += GCDperiod;
	}

}

ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1
	// (every 1 ms per TimerOn settings)
	// Count down to 0 rather than up to TOP (results in a more efficient comparison)
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		// Call the ISR that the user uses
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


void createChar(unsigned char loc, unsigned char *custom)
{
	LCD_WriteCommand(0x40+(loc*8));
	unsigned char i;
	for(i=0;i<8;i++)
	LCD_WriteData(custom[i]);
	LCD_WriteCommand(0x80);
	delay_ms(2);

}


unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

enum States {Init, ShiftL, ShiftR, Button_P, Button_R, Back} State;

unsigned char SS[10] = {SS_0,SS_1,SS_2,SS_3,SS_4,SS_5,SS_6,SS_7,SS_8,SS_9};

unsigned char j;
unsigned char k;
unsigned char l;
unsigned char count_done;
unsigned char timerGo;
unsigned char arrow[8] = {0x0, 0x4, 0x6, 0x1f, 0x6, 0x4, 0x0, 0x0};
unsigned char flagNote[8] =	{0x1, 0x3, 0x5, 0x9, 0x9, 0xb, 0x1b, 0x18};
unsigned char flagNote2[8] = { 0x0, 0x4, 0x6, 0x5, 0x4, 0xc, 0xc, 0x0 };
unsigned char smiley[8] = {0x0, 0x0, 0xa, 0x0, 0x11, 0xa, 0x4, 0x0};
unsigned char cliff1[8] = {0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3, 0x5};
unsigned char cliff2[8] = {0x0, 0x0, 0x10, 0x8, 0x10, 0x0, 0x0, 0x0};
unsigned char cliff3[8] = {0x9, 0xb, 0x9, 0x9, 0x7, 0x1, 0x5, 0x7};
unsigned char cliff4[8] = {0x10, 0x8, 0x8, 0x8, 0x10, 0x0, 0x0, 0x0};
unsigned char game_round;
unsigned char temp1;
unsigned char temp2;

enum GameStates {game_init,menu,round,game,end} GameState;

int Game_Fct(int state)
{

	unsigned char B0;
	B0 = 0x01 & PINB;
	B0 = !B0;
	switch(GameState)
	{
		case game_init:
			temp1 = 0;
			temp2 = 1;
			l=0;
			timerGo=0;
			GameState = menu;
			break;
		case menu:
			if(B0)
			{
				GameState = round;

			}
			break;
		case round:
			if(B0)
			{
				game_round++;
				GameState = game;
										LCD_DisplayString(1,"Game ComingSoon");
										LCD_Cursor(17);
										LCD_WriteData(0x04);
			}

			break;
		case game:
			//Game here.
			//
			l=0;
			if(count_done==1)
			{
				GameState = end;
			}
			break;
		case end:
			
			GameState = round;
			break;

	}

	switch(GameState)
	{
		case game_init:
			break;
		case menu:
			if(temp2==1)
			{
				temp2++;
			}
			if(temp2>1)
			{
				
			createChar(3,arrow);
			createChar(1,flagNote);
			createChar(2,flagNote2);	
			createChar(0x04,smiley);
			createChar(0x05,cliff1);
			createChar(0x06,cliff2);
			createChar(0x07,cliff3);
			createChar(0x00,cliff4);
			
			LCD_DisplayString(1,"Guess the Melody Start");
			LCD_Cursor(17);
			LCD_WriteData(3);
			LCD_Cursor(27);
			LCD_WriteData(2);
			LCD_WriteData(1);
			LCD_WriteData(1);
			LCD_WriteData(1);
			LCD_WriteData(2);
			LCD_Cursor(17);
			temp2 = 0;
			}
			
			break;
		case round:
			if(l<1)
			{	
				LCD_DisplayString(1,"     Round");
				LCD_Cursor(11);
				LCD_WriteData(game_round + '0');
				LCD_Cursor(15);
				LCD_WriteData(0x05);
				LCD_Cursor(16);
				LCD_WriteData(0x06);
				LCD_Cursor(31);
				LCD_WriteData(0x07);
				LCD_Cursor(32);
				LCD_WriteData(0x00);

			}
			l++;
			
			break;
		case game:
			timerGo=1;

			//game here
			break;
		case end:
			LCD_DisplayString(1,"Answer was: ");
			timerGo=0;
			break;
	}


	return GameState;
}





enum segStates {seg_init,wait,count_down } segState;


int seg_function(int state)
{
/*	if(Timer_Go == 1)
	{
		Write7Seg(SS[i]);
		j--;
		if(j < 0 || j > 10)
		{
			j = 9;
		}
	}	
	else if(Timer_Go == 0)
	{
		j=0;
	
	}
*/
	switch(segState)
	{
		
		case seg_init:
			j=9;
			k=0;
			count_done=0;
			segState=wait;
			break;
		case wait:
			count_done=0;
			if(timerGo==1)
			{
				segState=count_down;
			}
			break;
		case count_down:
			if(count_done==1)
			{
				segState=wait;
			}
	}
	
	switch(segState)
	{
		case seg_init:
			break;
		case wait:
			break;
		case count_down:
			k++;
			if(k>=10)
			{
				k=0;
				Write7Seg(SS[j]);
				j--;
				if(j < 0 || j > 10)
				{
					count_done=1;
					j = 9;
				}
			}
	}

	return segState;
}



int main(void)
{
		unsigned char i;
		DDRA = 0xFF; PORTA = 0x00;
		DDRB = 0x00; PORTB = 0xFF;
		DDRC = 0xFF; PORTC = 0x00;
		DDRD = 0xFF; PORTD = 0x00;
		TimerSet(100);
		TimerOn();
		LCD_init();
		i = 0;
		l = 0;
		game_round = 1;
		tasks[i].state = game_init;
		tasks[i].period = 200;
		tasks[i].elapsedTime = tasks[i].period;
		tasks[i].TickFct = &Game_Fct;

		i++;	

		tasks[i].state = seg_init;
		tasks[i].period = 100;
		tasks[i].elapsedTime = tasks[i].period;
		tasks[i].TickFct = &seg_function;

		while(1)
		{
		}
	
}
