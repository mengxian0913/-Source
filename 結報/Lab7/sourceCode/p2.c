#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"
#include "Scankey.h"

uint16_t fgColor, bgColor;
int seed = 0;
const int INF = 0x3f3f3f;
int dx[] = {0, 1, 1, -1, -1, 1, -1};
int dy[] = {0, -1, 1, -1, 1, 0, 0};
int idx = 0;
// STOP, RU, RD, LU, LD, R, L


int posX = 0, posY = 0;

void Buzz()
{
	PB11=0; // PB11 = 0 to turn on Buzzer
	CLK_SysTickDelay(10000);     // Delay 
	PB11=1; // PB11 = 1 to turn off Buzzer    
	CLK_SysTickDelay(10000);     // Delay 
}

void generateREC() {
	posX = rand() % 122;
	seed++;
	posY = rand() % 29;
	return;
}

void checkTouch(int *onRandom, int *x, int *y) {
	int x0 = posX;
	int x1 = posX + 5;
	int y0 = posY;
	int y1 = posY + 5;
	int isTouch = 0;
	
	if ( !(*onRandom) ) {
		return;
	}
	
	if ( (*y - y1) <= 3 || y0 - *y <= 3 ) {
		if ( *x <= x1 && *x >= x0 ) {
			isTouch = 1;
		}
	}
	
	else if ( *x - x1 <= 3 || x0 - *x <= 3 ) {
		if ( *y <= y1 && *y >= y0 ) {
			isTouch = 1;
		}
	}
	
	if ( isTouch ) {
		Buzz();
		*x = 64;
		*y = 58;
		*onRandom = 0;
		idx = 0;
	}
	return;
}

int32_t main (void)
{
	int i = 0, keyPressed = 0, onRandom = 0;
	uint16_t r;
	int x, y;
	
	OpenKeyPad();
	SYS_Init();
	init_LCD();  
	clear_LCD();
	
	x = 64;    // circle center x
	y = 58;    // circle center y
	r = 3;    // circle radius
	
	bgColor = BG_COLOR;
	while(1) {
		int nextX = x + dx[idx];
		int nextY = y + dy[idx];
		
		seed++;
		seed %= INF;
		srand(seed);
		
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
		
		checkTouch(&onRandom, &x, &y);
		
		CLK_SysTickDelay(20000);
		i=ScanKey();
		fgColor = FG_COLOR;
		clear_LCD();
		if ( onRandom ) draw_Rectangle(posX, posY, posX + 5, posY + 5, fgColor, bgColor);
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
		if ( i == 8 ) {
			onRandom = 1;
			generateREC();
		}
		
		if ( idx != 0 ) continue;
		
		if ( i == 4) idx = 6;
		else if ( i == 6 ) idx = 5;
		else if ( i == 1 ) idx = 3;
		else if ( i == 3 ) idx = 1;
		else if ( i == 7 ) idx = 4;
		else if ( i == 9 ) idx = 2;
  }
}