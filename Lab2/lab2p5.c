
#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 
char array[8] = {0x81, 0xc7, 0x43, 0x50, 0x30, 0x20, 0x10, 0x05};
void Delay_One_Sec() 
{ 
 for(int I=0; I <17000; I++); 
 } 


void main() 
{ 
char in; // Use variable ?in? as char 
 TRISA = 0xff; // fill out the ?? with the proper values 
 TRISB = 0x00; // fill out the ?? with the proper values 
 TRISC = 0x00;
 ADCON1 = 0x0f; // fill out the ?? with the proper values 
 PORTB = 0x00;
 
 while (1) 
{ 
     for (char i =0;i<8;i++)
     {
         PORTC = i;
         Delay_One_Sec();
         PORTD = array[i];
     }
} 
 } 