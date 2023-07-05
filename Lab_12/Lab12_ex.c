#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config CCP2MX = PORTBE

#include "I2C_Soft.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#define _XTAL_FREQ  8000000 







#define OFF 		0               // Defines OFF as decimal value 0
#define RED 		1               // Defines RED as decimal value 1
#define GREEN 		2               // Defines GREEN as decimal value 2
#define YELLOW 		3               // Defines YELLOW as decimal value 3
#define BLUE        4
#define PURPLE      5
#define CYAN        6
#define WHITE       7

//#define RD      RED
//#define GR      GREEN
//#define BU      BLUE
//#define BL      WHITE

void Initialize_Screen();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char setup_fan_temp = 75;
unsigned char Nec_state = 0;

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
//int color[21]={RD, RD, RD, GR, GR, GR, BU, BU, MA, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL};
char array2[21]={0x01,0x01, 0x01, 0x02, 0x02, 0x02, 0x04, 0x04, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07};
short nec_ok = 0;
char Nec_code1;
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
unsigned long long Nec_code;

//unsigned char Nec_state = 0;
//unsigned char i,bit_count;
////short nec_ok = 0;
//unsigned long long Nec_code;
//char Nec_code1;
//unsigned int Time_Elapsed;

char Nec_code1;
char found;

char buffer[33]     = " ECE3301L Sp'21 L12\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_SW_Txt[]   = "OFF";                // text storage for Heater Mode

//char array1[21]={};

    
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char RTC_ALARM_Txt[]= "0";                      //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_text[]   = "075F";


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





void SET_D3 (char color)
{
    switch (color)
    {
        case OFF: D3_RED =0;D3_GREEN=0;D3_BLUE=0;break;
        case RED: D3_RED =1;D3_GREEN=0;D3_BLUE=0;break;
        case GREEN: D3_RED =0;D3_GREEN=1;D3_BLUE=0;break;
        case YELLOW: D3_RED =1;D3_GREEN=1;D3_BLUE=0;break;
        case BLUE: D3_RED =0;D3_GREEN=0;D3_BLUE=1;break;
        case PURPLE: D3_RED =1;D3_GREEN=0;D3_BLUE=1;break;
        case CYAN: D3_RED =0;D3_GREEN=1;D3_BLUE=1;break;
        case WHITE: D3_RED =1;D3_GREEN=1;D3_BLUE=1;break;
    }
}

void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0F;
    TRISA = 0x00;
    TRISB = 0x01;
    TRISC = 0x01;
    TRISD = 0x0C;
    TRISE = 0x00;

    RBPU=0;
    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 

    DS1621_Init();
    init_INTERRUPT();
    FAN = 1;
    FAN_EN = 1;
    duty_cycle = 50; 
    do_update_pwm(duty_cycle);
}

void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();  
    

//    FAN_EN = 1;
//    FAN_LED = 1;
//    FAN_PWM = 1;
//    duty_cycle = 100;
//    while (1)
//    {
//        DS3231_Read_Time();
//
//        if(tempSecond != second)
//        {   
//            rpm = get_RPM();
//            tempSecond = second;
//            DS1621_tempC = DS1621_Read_Temp();
//            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
//            
//            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
//            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
//            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
//        }
//    }
    
//    FAN_EN = 1;
//    duty_cycle = 50;
//    do_update_pwm(duty_cycle);
//        DS3231_Read_Time();
//
//        if(tempSecond != second)
//        {
//            tempSecond = second;
//            DS1621_tempC = DS1621_Read_Temp();
//            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
//            rpm = get_RPM();
//            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
//            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
//            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
//        }    
//    
//    FAN_EN = 1;
//    duty_cycle = 50;    
//    do_update_pwm(duty_cycle);
//    while (1)
//    {
//        DS3231_Read_Time();
//
//        if(tempSecond != second)
//        {
//            tempSecond = second;
//            DS1621_tempC = DS1621_Read_Temp();
//            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
//            rpm = get_RPM();
//            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
//            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
//            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
//        }
//    }
    
    while (1)
    {

        DS3231_Read_Time();

        if(tempSecond != second)
        {
            //duty_cycle = 100;
            tempSecond = second;
            DS1621_tempC = DS1621_Read_Temp();
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
            rpm = get_RPM();
            Set_DC_RGB(duty_cycle);
            Set_RPM_RGB(rpm);
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
            //printf ("NEC_Code = %x\r\n", Nec_code1); 
            
            Update_Screen();
            
        }
        if (nec_ok == 1)					// This is for the final section
        {
                nec_ok = 0;
                //Nec_code1 = (char)  ((Nec_code >> 8));
                //printf ("NEC_Code = %x\r\n", Nec_code1);
                INTCONbits.INT0IE = 1;
                INTCON2bits.INTEDG0 = 0;
                
                found = 0xff;
                
                for (char i=0; i<21; i++)
                {
                    if(Nec_code1 == array1[i])
                    {
                        found = i;
                        break;
 //                       i=21;
                          
                    }
                    
                }
                
                if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
            }
            else
            {
                Do_Beep();
                printf ("button = %d \r\n", found);
                // add code to process buttons
                if(found == 6)
                {
                    Decrease_Speed();
                }
                if(found == 7)
                {
                    Increase_Speed();
                }
                if(found == 5)
                {
                    Toggle_Fan();
                }
                if (found == 8)
                {
                    DS3231_Setup_Time();
                }
            }            
                
                PORTE = array2[found];
                
                
            //printf ("%d \r\n", found);
            
            
            
    
        }
        
        
        
        
        
        
        
        
////       if (nec_ok == 1)
////        {
////            nec_ok = 0;
////            Nec_code1 = (char) ((Nec_code >> 8)); 
////            printf ("NEC_Code = %x\r\n", Nec_code1);       // make sure to use Nec_code1
////
////            INTCONbits.INT0IE = 1;          // Enable external interrupt
////            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
////
////            found = 0xff;
////            for (int i=0; i< 21; i++)
////            {
////                if (Nec_code1 == array1[i]) 
////                {
////                    found = i;
////                    break;
////                }
////            }
////             
////            if (found == 0xff) 
////            {
////                printf ("Cannot find button \r\n");
////            }
////            else
////            {
////                Do_Beep();
////                printf ("button = %d \r\n", found);
////                // add code to process buttons
////                if(found == 6)
////                {
////                    Decrease_Speed();
////                }
////                if(found == 7)
////                {
////                    Increase_Speed();
////                }
////                if(found == 5)
////                {
////                    Toggle_Fan();
////                }
////            }            
////        }
        
        
        
    }
}







