void Monitor_Heater();
int get_RPM();
void Toggle_Heater();
void Turn_Off_Fan();
void Turn_On_Fan();
void do_update_pwm(char) ;
void Increase_Speed();
void Decrease_Speed();
void Set_RPM_RGB(int rpm);
void Set_DC_RGB(int duty_cycle);

#define D1_RED PORTAbits.RA2
#define D1_GREEN PORTAbits.RA3
#define D1_BLUE PORTAbits.RA4

#define D2_RED PORTBbits.RB4
#define D2_GREEN PORTBbits.RB5
#define D2_BLUE PORTBbits.RB6

#define D3_RED PORTEbits.RE0
#define D3_GREEN PORTEbits.RE1
#define D3_BLUE PORTEbits.RE2
