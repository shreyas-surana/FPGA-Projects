#include <p18f4620.h> 
#include <stdio.h> 
#include <math.h> 
#include <usart.h>

void Wait_Half_Second();
void Wait_N_Seconds (char seconds);
void Wait_One_Second_With_Beep();
void Activate_Beep();
void Deactivate_Beep();

#pragma config OSC = INTIO67 
#pragma config WDT=OFF 
#pragma config LVP=OFF 
#pragma config BOREN =OFF
#pragma config CCP2MX = PORTBE

#define SEC_LED PORTDbits.RD7

#define NS_RED PORTAbits.RA1
#define NS_GREEN PORTAbits.RA2

#define NSLT_RED PORTAbits.RA3
#define NSLT_GREEN PORTAbits.RA4

#define EW_RED PORTBbits.RB4
#define EW_GREEN PORTBbits.RB5


#define EWLT_RED PORTEbits.RE0

//Defines EWLT RED as PORTE bit RE1
#define EWLT_GREEN PORTEbits.RE2
//Defines EWLT GREEN as PORTE bit RE2
//Defines NS PED as PORTA bit RA3 for Fed-switch
//Defines NS LT as PORTA bit RAI for left turn
#define NS_PED PORTAbits.RA5
#define NS_LT PORTBbits.RB1


#define EW_PED PORTBbits.RB0
#define EW_LT PORTBbits.RB2

#define SEG_G PORTEbits.RE1

#define MODE_LED PORTBbits.RB6
//Defines MODE LED as PORTA bit RA7, to differentiate day/night mode
//colors

// Defines OFF as decimal value O
// Defines RED as decimal value 1
// Defines GREEN as decimal value 2
#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3
// Defines YELLOW as decimal value 3
//char array[10] = {0x7f, 0x4f, 0x12, 0x06, 0x4c, 0x24, 0x20, 0x0f, 0x00, 0x04};
char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};

void WAIT_1_SEC()
{
    for (int j=0;j<0xffff;j++);
}

void Init_ADC(void)
{
    ADCON1 = 0x0E; //set pin 2 as analog signal
    ADCON2 = 0xA9;
    
}

void Select_ADC_Channel (char channel)
{
    ADCON0 = channel * 4.88 + 1; //4.8828125
}

unsigned int Get_Full_ADC(void)
{
int result;
 ADCON0bits.GO=1; // Start Conversion
 while(ADCON0bits.DONE==1); // Wait for conversion to be completed (DONE=0)
 result = (ADRESH * 0x100) + ADRESL; // Combine result of upper byte and lower byte into
 WAIT_1_SEC();
 return result; // return the most significant 8- bits of the result.
}

void init_UART()
{
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
 OSCCON = 0x60;
}

void putch (char c)
{
    while (!TRMT);  
    TXREG = c;
}

void Wait_Half_Second()
{
    T0CON = 0x02;
    TMR0L = 0xDB;
    TMR0H = 0x0B;
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON = 1;
    while (INTCONbits.TMR0IF == 0);
    T0CONbits.TMR0ON = 0;
}

void Set_NS(char color)
{
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;
        case RED: NS_RED =1;NS_GREEN=0;break;
        case GREEN: NS_RED =0;NS_GREEN=1;break;
        case YELLOW: NS_RED =1;NS_GREEN=1;break;
    }
}
void Set_NS_LT(char color)
{
    switch (color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break;
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;
    }
}
void Set_EW(char color)
{
    switch (color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break;
        case RED: EW_RED =1;EW_GREEN=0;break;
        case GREEN: EW_RED =0;EW_GREEN=1;break;
        case YELLOW: EW_RED =1;EW_GREEN=1;break;
    }
}
void Set_EW_LT(char color)
{
    switch (color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break;
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;
    }
}

void DO_DISPLAY_7SEG_UPPER(char digit)
{
    PORTC = array[digit] & 0x3F;
    if ((array[digit] &0x40) == 0x40) SEG_G = 1;
    else SEG_G = 0;
}

void DO_DISPLAY_7SEG_LOWER(char digit)
{
    PORTD = (PORTD & 0x80) | array[digit];
}

void PED_Control( char Direction, char Num_Sec)
{
    DO_DISPLAY_7SEG_LOWER(0x00);
    DO_DISPLAY_7SEG_UPPER(0x00);
    for(char i = Num_Sec-1;i>0; i--)
    {
        if(Direction == 0)
        {
            DO_DISPLAY_7SEG_UPPER(i);
        }
        else
        {
            DO_DISPLAY_7SEG_LOWER(i);
        }
        Wait_One_Second_With_Beep();
    }
    DO_DISPLAY_7SEG_UPPER(0x00);
    DO_DISPLAY_7SEG_LOWER(0x00);
    Wait_One_Second_With_Beep();
}

void Activate_Beep()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ; 
}

void Deactivate_Beep()
{
    CCP2CON = 0x0;
 PORTBbits.RB3 = 0; 
}

void Wait_One_Second()
{
    SEC_LED = 0;
    Wait_Half_Second();
    SEC_LED = 1;
    Wait_Half_Second();
}

void Wait_One_Second_With_Beep()
{
    SEC_LED = 1;
    Activate_Beep();
    Wait_Half_Second();
    SEC_LED = 0;
    Deactivate_Beep();
    Wait_Half_Second();
}

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = 0; I < seconds; I++)
    {
        Wait_One_Second();
    }
}

void Night_Mode()
{
    PORTD = 0xFF;
    PORTC = 0xFF;
    PORTEbits.RE1 = 1;
    MODE_LED = 0;
    Set_NS(GREEN);
    Set_NS_LT(RED);
    Set_EW(RED);
    Set_EW_LT(RED);
    Wait_N_Seconds(9);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);
    if(EW_LT == 1)
    {
        Set_EW_LT(GREEN);
        Wait_N_Seconds(7);
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(3);
        Set_EW_LT(RED);
    }
    Set_EW(GREEN);
    Wait_N_Seconds(9);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    
    if(NS_LT == 1)
    {
        Set_EW(RED);
        Set_NS_LT(GREEN);
        Wait_N_Seconds(7);
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(3);
        
    }
    
}



void Day_Mode()
{
    
    MODE_LED = 1;
    Set_NS(GREEN);
    Set_NS_LT(RED);
    Set_EW(RED);
    Set_EW_LT(RED);
    if(NS_PED == 1)
    {
        PED_Control(0, 7);
    }
    Wait_N_Seconds(9);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);
    if(EW_LT == 1)
    {
        Set_EW_LT(GREEN);
        Wait_N_Seconds(7);
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(3);
        Set_EW_LT(RED);
    }
    Set_EW(GREEN);
    if(EW_PED == 1)
    {
        PED_Control(1, 9);
    }
    Wait_N_Seconds(8);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    
    if(NS_LT == 1)
    {
        Set_EW(RED);
        Set_NS_LT(GREEN);
        Wait_N_Seconds(7);
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(3);
        
    }
    
}

void main(void)
{
    TRISA = 0xE1;
    TRISB = 0x07;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    Init_ADC();
    init_UART();
    Select_ADC_Channel (0);
    
    while (1)
    {
            
        
        int num_step = Get_Full_ADC();
        float voltage_mv = num_step * 4.0;
        float voltage = voltage_mv/1000;

        if(voltage >= 2.5)
        {
            Night_Mode();
        }
        else
            Day_Mode();
    }
    
    

    
//    while (1)
//    {
////        for (int i=0; i<4;i++)
////        {
////            Set_NS(i);
////            Set_NS_LT(i);
////            Set_EW(i);
////            Set_EW_LT(i);
////            //Wait_One_Second();
////            Wait_One_Second_With_Beep();
////            
////        }
//        PED_Control(0, 7);
//        PED_Control(1,6);
//    }
}



