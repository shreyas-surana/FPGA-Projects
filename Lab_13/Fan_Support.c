
#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"
#include "utils.h"

#define FAN_EN          PORTAbits.RA5
#define FAN_PWM         PORTCbits.RC2
#define FAN_LED         PORTDbits.RD5

extern char HEATER;
extern char duty_cycle;
extern char heater_set_temp;
extern signed int DS1621_tempF;

//#define FAN_EN          PORTAbits.RA5
//#define FAN_PWM         PORTCbits.RC2
//#define FAN_LED         PORTDbits.RD7

//#define D1_RED PORTAbits.RA2
//#define D1_GREEN PORTAbits.RA3
//#define D1_BLUE PORTAbits.RA4
//#define D2_RED PORTBbits.RB4
//#define D2_GREEN PORTBbits.RB5
//#define D2_BLUE PORTBbits.RB6

int diff;    // to calculate the difference
extern int FAN;

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

int get_duty_cycle(signed int temp, int set_temp)
{
// add code to check if temp is greater than set_temp. If so, set dc according to the handout
// check if dc is greater than 100. If so, set it to 100
// if dc is negative, set to 0
// return dc
    
    if (temp >= set_temp)
        {
            duty_cycle = 0;
        }
    else if (temp < set_temp) 
        {
            diff = set_temp - temp;
            
            if (diff > 50)
            {
                duty_cycle = 100;
            }
            if (diff >= 25 && diff < 50)
            {
                duty_cycle = diff * 2;
            }
            if (diff >= 10 && diff < 25)
            {
                duty_cycle = diff * 3/2;
            }
            if (diff >= 0 && diff < 10)
            {
                duty_cycle = diff;
            }
        }
    else
        duty_cycle = 0;
    return duty_cycle;

}

void Monitor_Heater()
{
    get_duty_cycle(DS1621_tempF, heater_set_temp);
    do_update_pwm(duty_cycle);

    if (HEATER == 1) 
    {
        FAN_EN = 1;
    }
    else FAN_EN = 0;
}

void Toggle_Heater()
{
// add code just to flip the variable FAN
    if (FAN == 1)
    {
        Turn_Off_Fan();
    }
    else
        Turn_On_Fan();

}

int get_RPM()
{
    int RPS = TMR3L / 2;    // read the count. Since there are 2 pulses per rev 
                            // then RPS = count /2 
    TMR3L = 0;              // clear out the count 
    return (RPS * 60);      // return RPM = 60 * RPS 
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FAN_LED = 0;
    HEATER = 0;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FAN_LED = 1;
    HEATER = 1;
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



