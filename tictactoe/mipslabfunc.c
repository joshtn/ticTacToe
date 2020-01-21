/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

extern int round;
extern int xwon;
extern int owon;
extern int count;
extern char board[4][17];
extern volatile int * LED;
extern char game_player;

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

char random[4][17] = {  {1,2,4,7,3,7,9,4,3,7,9,7,5,2,1,1,1},
                        {9,4,5,6,7,8,3,7,4,2,4,1,1,3,7,9,1},
                        {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6},
                        {1,2,4,7,3,7,9,4,3,7,9,7,5,2,1,1,8}};

int getsw (void) {
    return (PORTD >> 8) & 0x000F;
}

int getbtns (void) {
    return (PORTD >> 5) & 0x0007;
}

//funktion för att visa spelare
//spelare får börja varannan gång
void check_player(int count) {
    if (round%2 != 0) {
        if (count%2 == 0) {
            textbuffer[2][8] = game_player + '0';
            game_player = 72;
        } else {
            textbuffer[2][8] = game_player + '0';
            game_player = 63;
        }
        
    } else {
        if (count%2 == 0) {
            textbuffer[2][8] = game_player + '0';
            game_player = 63;
        } else {
            textbuffer[2][8] = game_player + '0';
            game_player = 72;
        }
    }
}

void show_round(int i) {
    textbuffer[3][15] = round + '0';
}


void update_board(int x, int y, char game_player) {
    if (board[x][y] != 'x' && board[x][y] != 'o') {
        board[x][y] = game_player + '0';
        textbuffer[x][y] = board[x][y];
        count++;
        display_update();
        check_won();
    }
}

void clear(void) {
    int i, j;
    count = 0;
    display_string(0, "tic tac toe");
    display_string(1, " - - -  ");
    display_string(2, " - - -  - turn");
    display_string(3, " - - -  round ");
    delay(3000);
    
    for (i = 0; i <= 4; i++){
        for (j = 0; j <= 16; j++){
            board[i][j] = random[i][j];
        }
    }
    
    //reset
    if (round == 7 || xwon == 3 || owon == 3) {
        round = 1;
        count = 0;
        xwon = 0;
        owon = 0;
        delay(2000);
        (*LED) = 0;
        
    }
}


void check_won(void) {
    int i, j;
    
    //ingen vinnare
    if (count == 9) {
        display_string(0, "game over");
        display_update();
        delay(3000);
        round++;
        clear();
    }
    
    //vågrät
    for (i = 1; i <= 4; i++){
        if ((board[i][1] == board[i][3]) && (board[i][1] == board[i][5])) {
            game_over();
        }
    }
    
    //lodrät
    for (i = 1; i <= 5; i = i+2){
        if ((board[1][i] == board[2][i]) && (board[1][i] == board[3][i])) {
            game_over();
        }
    }
    
    //diagnoal
    if (((board[1][1] == board[2][3]) && (board[1][1] == board[3][5])) ||
        ((board[1][5] == board[2][3]) && (board[1][5] == board[3][1]))){
        game_over();
    }
}

//funktion för att få rätt LED lampor att lysa
int pow (int x) {
    int y = 1;
    int i;
    for (i = 0; i < x; i++){
        y = y*2;
    }
return y;
}


void game_over(void) {
    display_string(0, "game over");
    display_update();
    delay(3000);
    round++;
   
    if (game_player == 63) { //motsatt, dvs när game_player är x pga timer
        (*LED) = ((*LED) + (pow(xwon)));
        xwon++;
       
    } else if (game_player == 72) { //motsatt, dvs när game_player är x pga timer
        (*LED) = ((*LED) + (pow(7-owon)));
        owon++;
    
    }

clear();
    
}

//FRÅN LAB
/* quicksleep:
 A simple function to create a small delay.
 Very inefficient use of computing resources,
 but very handy in some special cases. */
void quicksleep(int cyc) {
    int i;
    for(i = cyc; i > 0; i--);
}


uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}


void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}


void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

