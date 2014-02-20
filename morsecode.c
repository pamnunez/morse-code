/*
 * This code was written for use on a MC9S12C128 microcontroller, with a single
 * keypad button used as input. The goal was to implement a Morse code
 * detector that would measure the press duration for each press, keep track
 * of dots and dashes, and display the corresponding letter to the terminal.
 *
 */


#include <hidef.h>      /* common defines and macros */  
#include "mc9s12c128.h"
#include "stdio.h"
#include "termio.h"
int overflow, presstime, pressed, index, i;
int input[5] = { 0 };

                 
void interrupt 16 timer_overflow(void) {
  TFLG2 = 0x80;
  overflow++;
}

void interrupt 10 pulse(void) {
  TFLG1 |= 0x04;      // clearing the C2F flag (preparing for detection) 

  if(PTT & 0x04) {    // if button is pressed
    overflow = 0;
    pressed = 1;
    return;
  } else if (!(PTT & 0x04)) {
    pressed = 0;
  }


}
                 
void main(void) {
  TERMIO_Init();
  EnableInterrupts;
  TIE = 0x04;       // enable hardware interrupt
  DDRB = 0xFF;
  PORTB = 0x00;
  
  overflow = 0;
  pressed = 0;
  presstime = 0;
  index = 0;
  
  TSCR1 |= 0x80;    // start timer
  
  TFLG1 |= 0x04;    // clearing the C2F flag (preparing for detection)
  TCTL4 = 0x30;     // prepare to detect rising or falling edge (interrupt)
  TSCR2 |= 0x80;    // timer overflow interrupt enable



  while(1) {
    presstime = overflow * 32;
     
    if(!pressed) {
      if((presstime < 200) && (presstime > 20)) {
        PORTB = 0x40;
        //printf(".\n");
        input[index] = 1;
        index++;
                
      } else if((presstime >= 200)) {
        PORTB = 0x80;
        //printf("-\n");
        input[index] = 2;
        index++; 
      }
      pressed = 1;
      overflow = 0;
      presstime = 0;
      
    } else {
      if (presstime > 400) {
        PORTB = 0x00;
        index = 0;
        if(input[0] == 1) {                    // 1
          if(input[1] == 0) {                  // 10
            printf("E");                       
          } else if (input[1] == 1) {          // 11
            if(input[2] == 0) {                // 110
              printf("I");
            } else if (input[2] == 1) {        // 111
              if(input[3] == 0) {              // 1110
                printf("S");
              } else if (input[3] == 1) {      // 1111
                if(input[4] == 0) {            // 11110
                  printf("H");
                } else if (input[4] == 1) {    // 11111
                  printf("5");
                } else if (input[4] == 2) {    // 11112
                  printf("4");
                }
              } else /*if (input[3] == 2)*/ {  // 1112
                if (input[4] == 0) {           // 11120
                  printf("V");
                } else if (input[4] == 2) {    // 11122
                  printf("3");
                }
              }
            } else /*if (input[2] == 2)*/ {    // 112
              if (input[3] == 0) {             // 1120
                printf("U");
              } else if (input[3] == 1) {      // 1121
                printf("F"); 
              } else if ((input[3] == 2)&&(input[4] == 2)) {      // 11222
                printf("2");
              }
            }
          } else /*if (input[1] == 2)*/ {      // 12
            if (input[2] == 0) {               // 120
              printf("A");
            } else if (input[2] == 1) {        // 121
              // R, L
              if (input[3] == 0) {             // 1210
                printf("R");
              } else if (input[3] == 1) {      // 1211
                if (input[4] == 0) {           // 12110
                  printf("L");
                }
              }
            } else /*if(input[2] == 2)*/ {     // 122 
              // W, J, P
              if (input[3] == 0) {             // 1220
                printf("W");
              } else if (input[3] == 1) {      // 1221
                printf("P");
              } else if (input[3] == 2) {      // 1222
                if (input[4] == 0) {           // 12220
                  printf("J");
                } else if (input[4] == 2) {    // 12222
                  printf("1");
                }
              }
            }
          }
        } else if(input[0] == 2) {             // 2
          if(input[1] == 0) {                  // 20
            printf("T");
          } else if (input[1] == 1) {          // 21
            // N, D, B
            if (input[2] == 0) {               // 210
              printf("N");
            } else if (input[2] == 1) {        // 211
              if (input[3] == 0) {             // 2110
                printf("D");
              } else if (input[3] == 1) {      // 2111
                if (input[4] == 0) {           // 21110
                  printf("B");
                } else if (input[4] == 1) {    // 21111
                  printf("6"); 
                }
              } else /*if (input[3] == 2)*/ {  // 2112
                printf("X");
              }
            } else /*if (input[2] == 2)*/ {    // 212
                if (input[3] == 0) {           // 2120
                  printf("K");
                } else if (input[3] == 1) {    // 2121
                  printf("C");
                } else /*if (input[3] == 2)*/ {    // 2122
                  printf("Y");
                }
            }
          } else /*if (input[1] == 2)*/ {      // 22
            if (input[2] == 0) {               // 220
              printf("M");
            } else if (input[2] == 1) {        // 221
              // G, Q, Z
              if (input[3] == 0) {             // 2210
                printf("G");  
              } else if (input[3] == 1) {      // 2211
                if (input[4] == 0) {           // 22110
                  printf("Z");
                } else if (input[4] == 1) {    // 22111
                  printf("7");
                }
              } else /*if (input[3] == 2)*/ {  // 2212
                printf("Q");
              }
            } else /* if (input[2] == 2)*/ {   // 222
                if (input[3] == 0) {           // 2220
                  printf("O");
                } else if ((input[3] == 1)&&(input[4] == 1)) {  // 22211
                  printf("8");
                } else if ((input[3] == 2)&&(input[4] == 1)) {  // 22221
                  printf("9");
                } else if ((input[3] == 2)&&(input[4] == 2)) {  // 22222
                  printf("0");
                }
            } 
          }
        }
            
            
        for(i = 0; i < 5; i++) {
          input[i] = 0;
          index = 0;
        }
        
      }
    }

  }
  
  
}
