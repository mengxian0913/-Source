#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"
#include "Scankey.h"


int dx[] = {0, 1, 1, -1, -1, 1, -1};
int dy[] = {0, -1, 1, -1, 1, 0, 0};
// STOP, RU, RD, LU, LD, R, L


void Buzz()
{
	PB11=0; // PB11 = 0 to turn on Buzzer
	CLK_SysTickDelay(10000);     // Delay 
	PB11=1; // PB11 = 1 to turn off Buzzer    
	CLK_SysTickDelay(10000);     // Delay 
}

int32_t main (void)
{
	int i = 0, keyPressed = 0;
	int idx = 0;
	uint16_t r;
	uint16_t x, y;
	uint16_t fgColor, bgColor;
	OpenKeyPad();
	SYS_Init();
	init_LCD();  
	clear_LCD();	
	
	x = 64;    // circle center x
	y = 32;    // circle center y
	r = 3;    // circle radius
	
	bgColor = BG_COLOR;
	while(1) {
		
		int nextX = x + dx[idx];
		int nextY = y + dy[idx];
		
		if ( nextY <= 4 ) {
			if ( idx == 1 ) idx = 2;
			else if ( idx == 3 ) idx = 4;
			Buzz();
		} 
		
		if ( nextY >= 59 ) {
			if ( idx == 2 ) idx = 1;
			else if ( idx == 4 ) idx = 3;
			Buzz();
		}
		
		if ( nextX <= 4 ) {
			if ( idx == 3 ) idx = 1;
			else if ( idx == 4 ) idx = 2;
			else if ( idx == 6 ) idx = 5;
			Buzz();
		}
		
		if ( nextX >= 123 ) {
			if ( idx == 1 ) idx = 3;
			else if ( idx == 2 ) idx = 4;
			else if ( idx == 5 ) idx = 6;
			Buzz();
		}
		
		x = nextX;
		y = nextY;
		
		CLK_SysTickDelay(20000);
		i=ScanKey();
		fgColor = FG_COLOR;
		clear_LCD();
		draw_Circle(x, y, r, fgColor, bgColor); // draw a circle
		
		if( i == 0 ) {
			keyPressed = 0;
			continue;
		}
		
		if(keyPressed) {
			continue;
		}
		
		keyPressed = 1;
		
		if ( i == 5 ) idx = 0;
		
		if ( idx != 0 ) continue;
		
		if ( i == 4) idx = 6;
		else if ( i == 6 ) idx = 5;
		else if ( i == 1 ) idx = 3;
		else if ( i == 3 ) idx = 1;
		else if ( i == 7 ) idx = 4;
		else if ( i == 9 ) idx = 2;
  }
}