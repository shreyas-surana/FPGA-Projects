#include <p18f4620.h> 
#include <stdio.h> 
#include <math.h> 
#include <usart.h> 
#pragma config OSC = INTIO67 
#pragma config WDT=OFF 
#pragma config LVP=OFF 
#pragma config BOREN =OFF

#define D1_RED PORTBbits.RB3 
#define D1_GREEN PORTBbits.RB4 
#define D1_BLUE PORTBbits.RB5 
#define D2_RED PORTBbits.RB0
#define D2_GREEN PORTBbits.RB1
#define D2_BLUE PORTBbits.RB2 
#define D3_RED PORTAbits.RA4 
#define D3_GREEN PORTAbits.RA5 


#define delay 5
//void Select_ADC_Channel ();
void Init_ADC();  //initialize the A2D converter



char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};

void putch (char c)
{
    while (!TRMT);  
    TXREG = c;
}
void SET_D1_OFF() 
{ 
D1_RED = 0; 
D1_GREEN = 0; 
D1_BLUE = 0; 
} 
void SET_D1_RED() 
{ 
D1_RED = 1; 
D1_GREEN = 0; 
D1_BLUE = 0; 
} 
void SET_D1_GREEN() 
{ 
D1_RED = 0; 
D1_GREEN = 1; 
D1_BLUE = 0; 
} 
void SET_D1_BLUE() 
{ 
D1_RED = 0; 
D1_GREEN = 0; 
D1_BLUE = 1; 
} 
void SET_D1_WHITE() 
{ 
D1_RED = 1; 
D1_GREEN = 1; 
D1_BLUE = 1; 
} 
void SET_D2_OFF() 
{ 
D2_RED = 0; 
D2_GREEN = 0; 
D2_BLUE = 0; 
} 
void SET_D2_RED() 
{ 
D2_RED = 1; 
D2_GREEN = 0; 
D2_BLUE = 0; 
} 
void SET_D2_GREEN() 
{ 
D2_RED = 0; 
D2_GREEN = 1; 
D2_BLUE = 0; 
} 
void SET_D2_YELLOW() 
{ 
D2_RED = 1; 
D2_GREEN = 1; 
D2_BLUE = 0; 
} 
void SET_D2_BLUE() 
{ 
D2_RED = 0; 
D2_GREEN = 0; 
D2_BLUE = 1; 
} 
void SET_D2_PURPLE() 
{ 
D2_RED = 1; 
D2_GREEN = 0; 
D2_BLUE = 1; 
} 
void SET_D2_CYAN() 
{ 
D2_RED = 0; 
D2_GREEN = 1; 
D2_BLUE = 1; 
} 
void SET_D2_WHITE() 
{ 
D2_RED = 1; 
D2_GREEN = 1; 
D2_BLUE = 1; 
} 
void SET_D3_RED() 
{ 
D3_RED = 1; 
D3_GREEN = 0; 
} 
void SET_D3_GREEN() 
{ 
D3_RED = 0; 
D3_GREEN = 1; 
} 
void SET_D3_YELLOW() 
{ 
D3_RED = 1; 
D3_GREEN = 1; 
} 
void init_UART()
{
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
 OSCCON = 0x60;
}

void WAIT_1_SEC()
{
    for (int j=0;j<17000;j++);
}

void Display_Lower_Digit(int digit)
{
    PORTD = array[digit];
}
void Display_Upper_Digit (int digit)
{
    PORTC = array[digit] & 0x3F;
    char  pinRC6 = array[digit] & 0x40;
    if ( pinRC6 == 0)
     PORTE = 0x00;
    else PORTE = 0x04;
}

void Select_ADC_Channel (char channel)
{
    ADCON0 = channel * 4 + 1;
}

void Init_ADC(void)
{
    ADCON1 = 0x1B; //set pin 2 as analog signal
    ADCON2 = 0xA9;
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
//unsigned int Get_Full_ADC(void)
//{
//  int result;                        
//    ADCON0bits.GO=1;      //start conversion          
//    while (ADCON0bits.DONE==1); //wait for conversion to be completed
//    result = (ADRESH * 0x100) + ADRESL; //Combine result of upper byte and lower byte
//    WAIT_1_SEC();
//    return result;          //Return the most significant 8-bits
//}

void main(void)
{
    init_UART();
    Init_ADC();
    TRISA = 0x0F;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    
    Select_ADC_Channel (0);
    int num_step = Get_Full_ADC();
    float voltage_mv = num_step * 4.0;
    float temperature_C = (1035.0 - voltage_mv)/5.50;
    float temperature_F = 1.80 * temperature_C + 32.0;
    int tempF = (int) temperature_F;
    int U = tempF/10;
    int L = tempF%10;
    Display_Upper_Digit(U);
    Display_Lower_Digit(L);
    
    printf ("step = %d \r \n", num_step);
    
    printf( "Temperature = %d/F \r \n", tempF);
    
    if(tempF >= 76)
    {
        SET_D1_WHITE();
        SET_D2_WHITE();
    }
    else if(tempF >= 70)
    {
        SET_D1_BLUE();
        SET_D2_WHITE();
    }
    else if(tempF >= 66)
    {
        SET_D1_BLUE();
        SET_D2_CYAN();
    }
    else if(tempF >= 60)
    {
        SET_D1_GREEN();
        SET_D2_CYAN();
    }
    else if(tempF >= 56)
    {
        SET_D1_GREEN();
        SET_D2_PURPLE();
    }
    else if(tempF >= 50)
    {
        SET_D1_RED();
        SET_D2_PURPLE();
    }
    else if(tempF >= 46)
    {
        SET_D1_RED();
        SET_D2_BLUE();
    }
    else if(tempF >= 40)
    {
        SET_D1_OFF();
        SET_D2_BLUE();
    }
    else if(tempF >= 30)
    {
        SET_D1_OFF();
        SET_D2_YELLOW();
    }
    else if(tempF >= 20)
    {
        SET_D1_OFF();
        SET_D2_GREEN();
    }
    else if(tempF >= 10)
    {
        SET_D1_OFF();
        SET_D2_RED();
    }
    else
    {
        SET_D1_OFF();
        SET_D2_OFF();
    }
    Select_ADC_Channel (1);
    
    num_step = Get_Full_ADC();
    voltage_mv = num_step * 4.0;
    float voltage = voltage_mv/1000;
    printf ("Voltage = %f \r \n", voltage_mv);
    printf ("Voltage = %f \r \n", voltage_mv);
    
    if(voltage >= 3.4)
    {
        SET_D3_YELLOW();
    }
    else if(voltage >= 2.5)
    {
        SET_D3_GREEN();
    }
    else
        SET_D3_RED();
    
    
//    while(1)
//    {
//        for (int i=0;i<10;i++)
//        {
//            Display_Upper_Digit(i);
//            Display_Lower_Digit(i);
//            
//            WAIT_1_SEC();
//        }
//        PORTC = 0xFF;
//        PORTD = 0xFF;
//        PORTE = 0xFF;
//        
//        for (int i=0;i<8;i++)
//       {
//           PORTB = i;
//           WAIT_1_SEC();
//       }
//       PORTB = 0x00;
//      
//        for (int i=0;i<8;i++)
//       {
//           PORTB = i<<3;
//           WAIT_1_SEC();
//       }
//       PORTB = 0x00;
//      
//       for (int i=0; i<4; i++)
//       {
//           PORTA = i<<4;
//           WAIT_1_SEC();
//       }
//       PORTA = 0x00;
        
    
    
    
    }
