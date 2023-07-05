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

#include "I2C_Soft.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "ST7735.h"
#define _XTAL_FREQ  8000000 

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text


// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BL               ST7735_BLACK

//#define kp               PORTEbits.RE1     

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text



#define D1R             0x04
#define D1B             0x10
#define D1M             0x14
#define D1W             0x1C
#define D2R             0x10
#define D2B             0x40
#define D2G             0x20
#define D2M             0x50
#define D2W             0x70
#define D3R             0x01
#define D3G             0x02
#define D3B             0x04
#define D3M             0x05
#define D3W             0x07

char tempSecond = 0xff; 
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0", "PRV\0", "NXT\0", "PLY\0", "VL-\0", "VL+\0", "EQ \0", "0  \0", "100\0", "200\0", "1  \0", "2  \0", "3  \0", "4  \0", "5  \0", "6  \0", "7  \0", "8  \0", "9  \0"};
//int color[21]={RD, RD, RD, BU, BU, GR, MA, MA, MA. BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL};
int color[21]={RD, RD, RD, GR, GR, GR, BU, BU, MA, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL};
char d1[21]={D1R,D1R,D1R,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char d2[21]={0,0,0,D2G,D2G, D2G, D2B, D2B, D2M, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char d3[21]={0,0,0,0,0,0,0,0,0,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W};

char TempSecond;

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

void TIMER1_isr(void);
void INT0_isr(void);
void Initialize_Screen();

unsigned char Nec_state = 0;
unsigned char i,bit_count;
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;

char Nec_code1;
char found;

void Deactivate_Buzzer();
void Activate_Buzzer();
void Initialize_Screen();
void Wait_Half_Sec();
void Wait_One_Sec();

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}




void Wait_Half_Sec()
{
    T0CON = 0x03;                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                               // set the lower byte of TMR
    TMR0H = 0x0B;                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0; 
}

void Wait_One_Sec()
{
    Wait_Half_Sec();
    Wait_Half_Sec();
}

void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    init_INTERRUPT();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1= 0x0F;		    // Fill out values
    TRISA = 0x00;
    TRISB = 0x01;
    TRISC = 0x00;                   
    TRISD = 0x00;
    TRISE = 0x00;
    RBPU=0;
    I2C_Init(100000); 

    DS1621_Init();

}



void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ; 
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0; 
}

void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Fall 21-S4");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 11 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}



void main() 
{ 
    
    Do_Init();       
    Initialize_Screen();
    nec_ok = 0;                             // Clear flag

      while (1)						// This is for the DS3231 testing. Comment this section
      {							// out to move on to the next section

        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();
            signed char tempF = (tempC * 9 / 5) + 32;
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
        }  
        if (nec_ok == 1)					// This is for the final section
        {
                nec_ok = 0;
                //Nec_code1 = (char)  ((Nec_code >> 8));
                
                INTCONbits.INT0IE = 1;
                INTCON2bits.INTEDG0 = 0;
                
                char found = 0xff;
                
                for (char i=0; i<21; i++)
                {
                    if(Nec_code1 == array1[i])
                    {
                        found = i;
                        break;
 //                       i=21;
                          
                    }
                    
                }
                
                if (found == 8)
                {
                    DS3231_Setup_Time();
                    
                    
                }
                
               
            printf ("%d \r\n", found);
            
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
                PORTA = d1[found];
                PORTB = d2[found];
                PORTE = d3[found];
                Activate_Buzzer();
                PORTDbits.RD7 = 1;
                Wait_Half_Sec();
                Deactivate_Buzzer();
                PORTDbits.RD7 = 0;
            }
            
            
    
        }
        
    

      }
            
   

//    while (1)							// This is for the DS1621 testing. Comment this section
//    {								// out to move on to the next section
//
//        char tempC = DS1621_Read_Temp();
//        char tempF = (tempC * 9 / 5) + 32;
//        printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
//        Wait_One_Sec();
//    }
 
    
    
}





