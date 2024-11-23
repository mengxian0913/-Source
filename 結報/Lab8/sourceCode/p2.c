#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Seven_Segment.h"

#define INF (int)(1e9)
#define MAXN (int)(1e5 + 10)

int onRestart = 0;
int onStart   = 0; 


int players[4] = {0};
int speedOfPlayers[4] = {0};
int posOfPlayers[4][2] = {
	{0, 0},
	{0, 16},
	{0, 32},
	{0, 48}
};

int onEnd[4] = {0, 0, 0, 0};

const int staticSpeed[4] = {2, 4, 6, 8};

int seed = 0;

void show_LCD() {
	int i = 0;
	clear_LCD();
	for(i = 0; i < 4; i++) {
		char player = (char)(players[i] + '0');
		printC(posOfPlayers[i][0], posOfPlayers[i][1], player);
	}
	return;
}

int cmp(const void* a, const void* b) {
	return (*(int*)a - *(int*)b);
}


void restart() {
	
	int i = 0;
	int temp[4];
	int map[20] = {0};
	int visited[20] = {0};
	
	
	onRestart = 0;
	PC12 = PC13 = PC14 = PC15 = 1;
	for(i = 0; i < 4; i++) {
		posOfPlayers[i][0] = 0;
		onEnd[i] = 0;
	}
	
	for(i = 0; i < 4; i++) {
		
		while( 1 ) {
			int newPlayer = rand() % 9 + 1;		
			if ( !visited[newPlayer] ) {
				visited[newPlayer] = 1;
				players[i] = newPlayer;
				break;
			}
			seed = (seed + 1) % INF;
		}
		
	}
	
	for(i = 0; i < 4; i++) {
		temp[i] = players[i];
	}

	qsort(temp, 4, sizeof(int), cmp);
	
	for(i = 0; i < 4; i++) {
		map[temp[i]] = staticSpeed[i];
	}
	
	for(i = 0; i < 4; i++) {
		speedOfPlayers[i] = map[players[i]];
	}
	
	show_LCD();
	
	return;
}

void GPAB_IRQHandler(void)
{
	int flag, i;
    if (PA->ISRC & BIT0) {        // check if PB12 interrupt occurred
        PA->ISRC |= BIT0;         // clear PB12 interrupt status
			               // set a flag for PB12(KEY1)
    } else if (PA->ISRC & BIT1) { // check if Pb13 interrupt occurred
        PA->ISRC |= BIT1;         // clear PB13 interrupt status  
                      // set a flag for PB13(KEY2)
    } else if (PA->ISRC & BIT2) { // check if PB14 interrupt occurred
        PA->ISRC |= BIT2;         // clear PB14 interrupt status  
        	              // set a flag for PB14(KEY3)			
    } else {                      // else it is unexpected interrupts
        PA->ISRC = PA->ISRC;	      // clear all GPB pins
				PA0 = PA1 = PA2 = 1;
				
		}
		
		if ( onRestart ) restart();
}

void Init_KEY(void)
{
    GPIO_SetMode(PA, (BIT3 | BIT4 | BIT5), GPIO_MODE_OUTPUT);
		GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 2, GPIO_INT_FALLING);		
    NVIC_EnableIRQ(GPAB_IRQn);   
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));
	  PA3 = PA4 = PA5 = 0;
}



void EINT1_IRQHandler(void)
{
	GPIO_CLR_INT_FLAG(PB, BIT15);	// Clear GPIO interrupt flag
	if ( onRestart ) {
		restart();
		return;
	}
	
	onStart = 1;
}

void Init_EXTINT(void)
{
	// Configure EINT1 pin and enable interrupt by rising and falling edge trigger
	GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
	GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
	NVIC_EnableIRQ(EINT1_IRQn);

	// Enable interrupt de-bounce function and select de-bounce sampling cycle time
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
	GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}




void showEnd(int winner) {
	winner += 1;
	if ( winner == 1 ) PC12 = 0;
	else if ( winner == 2 ) PC13 = 0;
	else if ( winner == 3 ) PC14 = 0;
	else if ( winner == 4 ) PC15 = 0;
	return;
}



void checkEndGame() {
	int amountOfEnd = 0, i = 0;
	for(i = 0; i < 4; i++) {
		if ( posOfPlayers[i][0] >= 120 ) {
			onEnd[i] = 1;
			showEnd(i);
		}
	}
	
	for(i = 0; i < 4; i++) {
		amountOfEnd += onEnd[i] != 0 ? 1 : 0;
	}
	
	if ( amountOfEnd >= 4 ){
		onRestart = 1;
		onStart   = 0;
	}
	return;
}


int main(void)
{
	int i;
	SYS_Init();
	Init_KEY();
	Init_EXTINT();
	OpenKeyPad();
	init_LCD();
	clear_LCD();
  
	restart();
	
	while(1) {
		show_LCD();
		CLK_SysTickDelay(2000000);
		
		checkEndGame();
		seed = (seed + 1) % INF;
		srand(seed);

		for(i = 0; i < 4; i++) {
			if ( !onEnd[i] && onStart ) {
				posOfPlayers[i][0] += speedOfPlayers[i];
			}
		}
	}
	
	return 0;
}