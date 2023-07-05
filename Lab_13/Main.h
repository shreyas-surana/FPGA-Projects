
#define _XTAL_FREQ      8000000

#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02



#define FAN_EN          PORTAbits.RA5    // set port definition for FAN Enable
#define FAN_LED         PORTDbits.RD5    // set port definition for FAN LED
#define RTC_ALARM_NOT   PORTDbits.RD6    // set port definition for RTC_Matched
#define KEY_PRESSED     PORTDbits.RD7	// set port definition KEY_PRESSED LED

#define D1_RED PORTAbits.RA2
#define D1_GREEN PORTAbits.RA3
#define D1_BLUE PORTAbits.RA4

#define D2_RED PORTBbits.RB4
#define D2_GREEN PORTBbits.RB5
#define D2_BLUE PORTBbits.RB6

#define D3_RED PORTEbits.RE0
#define D3_GREEN PORTEbits.RE1
#define D3_BLUE PORTEbits.RE2

#define Ch_Minus        0
#define Channel         1
#define Ch_Plus         2
#define Prev            3
#define Next            4
#define Play_Pause      5
#define Minus           6
#define Plus            7
#define EQ              8




