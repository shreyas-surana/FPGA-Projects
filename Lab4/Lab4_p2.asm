#include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF 
 input_a equ 0x20
 result equ 0x21
 ORG 0x0000 
    
START:
    MOVLW 0x0F
    MOVWF ADCON1
    
    MOVLW 0xFF
    MOVWF TRISA
    
    MOVLW 0x00
    MOVWF TRISB
    
    MOVLW 0xFF
    MOVWF TRISC
    
    MOVLW 0x07
    MOVWF TRISD
    
    MOVLW 0x00
    MOVWF TRISE

MAIN_LOOP:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF input_a
    COMF input_a,0
    
    ANDLW 0x0F
    MOVWF result
    MOVFF result, PORTB
    BCF PORTE,1
    
    BNZ CHECK_NOTZ
    BSF PORTE,1
    
CHECK_NOTZ:
    GOTO MAIN_LOOP
    
    BZ SET_BIT
    BCF PORTE, 1
    GOTO JUMP
    
SET_BIT:
    BSF PORTE, 1
    
JUMP:
    GOTO MAIN_LOOP
    
    
    END
    
 
 
 

