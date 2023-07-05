char check_for_button_input();
char bcd_2_dec (char);
int dec_2_bcd (char);
void Do_Beep();
void Do_Beep_Good();
void Do_Beep_Bad();
void Activate_Buzzer();
void Activate_Buzzer_4KHz();
void Activate_Buzzer_2KHz();
void Activate_Buzzer_500Hz();
void Deactivate_Buzzer();
void Wait_One_Sec();
void do_update_pwm(char);
void Set_RGB_Color(int);
float read_volt();
unsigned int get_full_ADC();
void Init_ADC();

#define KEY_PRESSED PORTDbits.RD7 

#define D1_RED PORTAbits.RA2
#define D1_GREEN PORTAbits.RA3
#define D1_BLUE PORTAbits.RA4

#define D2_RED PORTBbits.RB4
#define D2_GREEN PORTBbits.RB5
#define D2_BLUE PORTBbits.RB6

#define D3_RED PORTEbits.RE0
#define D3_GREEN PORTEbits.RE1
#define D3_BLUE PORTEbits.RE2

#define OFF 0x00 
#define D3RD 0x01                                                              //Define colors for RGB D3                                      
#define D3GN 0x02
#define D3YW 0x03
#define D3BL 0x04
#define D3PU 0x05
#define D3CY 0x06
#define D3WH 0x07

char D3[8]={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
