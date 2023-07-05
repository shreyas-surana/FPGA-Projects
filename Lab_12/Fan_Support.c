#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

#define FAN_EN          PORTAbits.RA5
#define FAN_PWM         PORTCbits.RC2
#define FAN_LED         PORTDbits.RD7

#define D1_RED PORTAbits.RA2
#define D1_GREEN PORTAbits.RA3
#define D1_BLUE PORTAbits.RA4
#define D2_RED PORTBbits.RB4
#define D2_GREEN PORTBbits.RB5
#define D2_BLUE PORTBbits.RB6

extern char FAN;
extern char duty_cycle;

void SET_D1 (int color)
{
    switch (color)
    {
        case 0: D1_RED =0;D1_GREEN=0;D1_BLUE=0;break;
        case 1: D1_RED =1;D1_GREEN=0;D1_BLUE=0;break;
        case 2: D1_RED =0;D1_GREEN=1;D1_BLUE=0;break;
        case 3: D1_RED =1;D1_GREEN=1;D1_BLUE=0;break;
        case 4: D1_RED =0;D1_GREEN=0;D1_BLUE=1;break;
        case 5: D1_RED =1;D1_GREEN=0;D1_BLUE=1;break;
        case 6: D1_RED =0;D1_GREEN=1;D1_BLUE=1;break;
        case 7: D1_RED =1;D1_GREEN=1;D1_BLUE=1;break;
    }
}

void SET_D2 (char color)
{
    switch (color)
    {
        case 0: D2_RED =0;D2_GREEN=0;D2_BLUE=0;break;
        case 1: D2_RED =1;D2_GREEN=0;D2_BLUE=0;break;
        case 3: D2_RED =0;D2_GREEN=1;D2_BLUE=0;break;
        case 2: D2_RED =1;D2_GREEN=1;D2_BLUE=0;break;
        case 4: D2_RED =0;D2_GREEN=0;D2_BLUE=1;break;
        case 5: D2_RED =1;D2_GREEN=0;D2_BLUE=1;break;
        case 6: D2_RED =0;D2_GREEN=1;D2_BLUE=1;break;
        case 7: D2_RED =1;D2_GREEN=1;D2_BLUE=1;break;
    }
}

int get_RPM() 
{ 
    int RPS = TMR3L / 2;    // read the count. Since there are 2 pulses per rev 
                            // then RPS = count /2 
    TMR3L = 0;              // clear out the count 
    return (RPS * 60);      // return RPM = 60 * RPS 
} 
void Toggle_Fan()
{
    if (FAN == 1)
    {
        Turn_Off_Fan();
    }
    else
        Turn_On_Fan();
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FAN_LED = 0;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FAN_LED = 1;
}

void Increase_Speed()
{
    if (duty_cycle == 100)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(duty_cycle);
    }
    else
    {
        duty_cycle = duty_cycle + 5;
        do_update_pwm(duty_cycle);
    }
        

}

void Decrease_Speed()
{
     if (duty_cycle == 0)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(duty_cycle);
    }
    else
    {
        duty_cycle = duty_cycle - 5;
        do_update_pwm(duty_cycle);
    }
}

void do_update_pwm(char duty_cycle) 
{ 
float dc_f; 
 int dc_I; 
PR2 = 0b00000100 ; // set the frequency for 25 Khz 
T2CON = 0b00000111 ; // 
dc_f = ( 4.0 * duty_cycle / 20.0) ; // calculate factor of duty cycle versus a 25 Khz 
 // signal 
dc_I = (int) dc_f; // get the integer part 
 if (dc_I > duty_cycle) dc_I++; // round up function 
 CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100; 
 CCPR1L = (dc_I) >> 2; 
} 

void Set_DC_RGB(int duty_cycle)
{
    if(duty_cycle >= 70)
    {
        SET_D1(7);
    }
    else if (duty_cycle >= 60)
    {
        SET_D1(6);
    }
    else if (duty_cycle >= 50)
    {
        SET_D1(5);
    }
    else if (duty_cycle >= 40)
    {
        SET_D1(4);
    }
    else if (duty_cycle >= 30)
    {
        SET_D1(3);
    }
    else if (duty_cycle >= 20)
    {
        SET_D1(2);
    }
    else if (duty_cycle >= 10)
    {
        SET_D1(1);
    }
    else if (duty_cycle >= 0)
    {
        SET_D1(0);
    }
}

void Set_RPM_RGB(int rpm)
{
    if(rpm >= 3000)
    {
        SET_D2(7);
    }
    else if (rpm >= 2500)
    {
        SET_D2(6);
    }
    else if (rpm >= 2000)
    {
        SET_D2(5);
    }
    else if (rpm >= 1500)
    {
        SET_D2(4);
    }
    else if (rpm >= 1000)
    {
        SET_D2(3);
    }
    else if (rpm >= 500)
    {
        SET_D2(2);
    }
    else if (rpm > 0)
    {
        SET_D2(1);
    }
    else if (rpm == 0)
    {
        SET_D2(0);
    }
}



