
/* mipslabwork.c
 
 This file written 2015 by F Lundevall
 Updated 2017-04-21 by F Lundevall
 
 This file should be changed by YOU! So you must
 add comment(s) here with your name(s) and date(s):
 
 This file modified 2017-04-31 by Ture Teknolog
 
 For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#define TMR2PERIOD ((80000000/256)/10)
int count = 0;
int timeoutcount = 0;
char game_player = 0;
char textstring[] = "text, more text, and even more text!";
int round = 1;
float xwon = 0;
float owon = 0;
volatile int * LED = (volatile int *) 0xbf886110;
char board[4][17] = {   {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5},
                        {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,6},
                        {9,4,5,6,7,8,3,7,4,2,4,1,1,3,7,9},
                        {1,2,4,7,3,7,9,4,3,7,9,7,5,2,1,1}};

void user_isr( void )
{
    return;
}

void labinit( void )
{
   TRISE = TRISE & 0xFF00;
    * LED = 0x0;
    TRISD = TRISD & 0x0fe0;
    
    //PRx period register
    PR2 = TMR2PERIOD;
    //prescale
    T2CON = 0x70;
    //TMRx räknar upp till PRx, reset till 0
    TMR2 = 0;
    //startar timer
    T2CONSET = 0x8000;
    
    return;
}

void labwork( void )
{
    int sw = getsw();
    int btn = getbtns();
    check_player(count);
    show_round(round);

    //clear
    if (sw == 15 && btn ==1) {
        clear();
        (*LED) = 0;
        round = 1;
        count = 0;
    }
    
    //rad 1
    if (sw == 8) {
        if (btn == 4) {
            update_board(1,1,game_player);
        } else if (btn == 2) {
            update_board(1,3,game_player);
        } else if (btn == 1) {
            update_board(1,5,game_player);
        }
    }

    //rad 2
    if (sw == 4) {
        if (btn == 4) {
            update_board(2,1,game_player);
        } else if (btn == 2) {
            update_board(2,3,game_player);
        } else if (btn == 1) {
            update_board(2,5,game_player);
        }
    }
    
    
    //rad 3
    if (sw == 2) {
        if (btn == 4) {
            update_board(3,1,game_player);
        } else if (btn == 2) {
            update_board(3,3,game_player);
        } else if (btn == 1) {
            update_board(3,5,game_player);
        }
    }
    
    //timer
    //time-out event flag, bit 8
    if(IFS(0) & 0x100){
        //ökar counter
        timeoutcount++;
        //reseting flag
        IFSCLR(0) = 0x100;
    }
    
    //utför var 10e time-out cykel
    if(timeoutcount == 10) {
        display_update();
        timeoutcount = 0;
    }

}


