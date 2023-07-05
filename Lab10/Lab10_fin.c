
/* 
 * File:   Lab10_p2.c
 * Author: suran
 *
 * Created on April 13, 2022, 8:28 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#include "ST7735.h"
#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz


void TIMER1_isr(void);
void INT0_isr(void);
void Initialize_Screen();

unsigned char Nec_state = 0;
unsigned char i,bit_count;
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;

// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define WT               ST7735_WHITE

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define D1R             0x02
#define D1B             0x10
#define D1M             0x0A
#define D1W             0x1A
#define D2R             0x10
#define D2B             0x40
#define D2M             0x50
#define D2W             0x70
#define D3R             0x01
#define D3G             0x02
#define D3M             0x05
#define D3W             0x07


char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
char txt1[21][4] ={"CH-\0"};
int color[21]={RD, RD, RD, BU, BU, GR, MA, MA, MA. WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT};
char d1[21]={D1R,0,0,D1B,0,0,D1M,0,0,D1W,0,0,D1W,0,0,D1W,0,0,D1W,0,0};
char d2[21]={0, D2R, 0, 0, D2B, 0, 0, D2M, 0, 0, D2W, 0, 0, D2W, 0, 0, D2W, 0, 0, D2W, 0};
char d3[21]={0,0,D3R,0,0,D3G,0,0,D3M,0,0,D3W,0,0,D3W,0,0,D3W,0,0,D3W};

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}
void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();
    if (INTCONbits.INT0IF == 1) INT0_isr();
}

void TIMER1_isr(void)
{
    Nec_state = 0;                          // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                   // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                    // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;
}

void INT0_isr(void)
{
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;       // Store Timer1 value
        TMR1H = 0;                          // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_state)
    {
        case 0 :
        {
                                            // Clear Timer 1
            TMR1H = 0;                      // Reset Timer1
            TMR1L = 0;                      //
            PIR1bits.TMR1IF = 0;            //
            T1CON= 0x90;                    // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;           // Enable Timer 1
            bit_count = 0;                  // Force bit count (bit_count) to 0
            Nec_code = 0;                   // Set Nec_code = 0
            Nec_state = 1;                  // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;        // Change Edge interrupt of INT0 to Low to High            
            return;
        }
        
        case 1 :
        {
            if ((Time_Elapsed>8500) && (Time_Elapsed<9500))
            {
                Nec_state = 2;
            }
            else
                force_nec_state0();
            INTCON2bits.INTEDG0 = 0;
            return;
        }
        
        case 2 :                            // Add your code here
        {
            if ((Time_Elapsed>4000) && (Time_Elapsed<5000))
            {
                Nec_state = 3;
            }
            else
                force_nec_state0();
            INTCON2bits.INTEDG0 = 1;
            return;
        }
        
        case 3 :                            // Add your code here
        {
            if ((Time_Elapsed>400) && (Time_Elapsed<700))
            {
                Nec_state = 4;
            }
            else
                force_nec_state0();
            INTCON2bits.INTEDG0 = 0;
            return;
        }
        
        case 4 :                            // Add your code here
        {
            if ((Time_Elapsed>400) && (Time_Elapsed<1800))
            {
                Nec_code = Nec_code << 1;
                if (Time_Elapsed>1000)
                {
                    Nec_code = Nec_code +1;
                }
                bit_count++;
                if (bit_count > 31)
                {
                    nec_ok = 1;
                    INT0IE = 0;
                    Nec_state = 0;
                    
                }
                Nec_state = 3;
            }
            else
                force_nec_state0();
            INTCON2bits.INTEDG0 = 0;
            return;
        }
    }
}

void main()
{
    init_UART();
    OSCCON = 0x70;                          // 8 Mhz
    nRBPU = 0;                              // Enable PORTB internal pull up resistor
    TRISB = 0x01;
    TRISC = 0x00;                           // PORTC as output
    TRISD = 0x00;
    ADCON1 = 0x0F;                          //
    Initialize_Screen();
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge H to L
    INTCONbits.INT0IE = 1;                  // Enable external interrupt
    TMR1H = 0;                              // Reset Timer1
    TMR1L = 0;                              //
    PIR1bits.TMR1IF = 0;                    // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                    // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                    // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                     // Enable global interrupts
    nec_ok = 0;                             // Clear flag
    Nec_code = 0x0;                         // Clear code
    
    while(1)
    {
        if (nec_ok == 1)
        {
            nec_ok = 0;

            Nec_code1 = (char) ((Nec_code >> 8));
            
            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
             
            char found = 0xff;
            
            for(char i = 0; i<=20; i++)
            {
                if(Nec_code1 == array1[i])
                {
                    found = i;
                }
            }
            printf ("NEC_Code = %08lx %x ", Nec_code, Nec_code1);
            printf ("%d \r\n", found);
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
            }
            PORTA = d1[found];
            PORTB = d2[found];
            PORTE = d3[found];
            
        }
    }
}


void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Sp 22-S4");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 10 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

