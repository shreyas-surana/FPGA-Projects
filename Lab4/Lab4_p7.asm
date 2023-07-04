#include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF 
 input_a equ 0x20
 result equ 0x21
 input_c equ 0x22
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
    BTFSC   PORTD, 2
    GOTO    PORTD2EQ1
    GOTO    PORTD2EQ0
    
    

PORTD2EQ1:
    GOTO TASK_BCD
    
PORTD2EQ0:
    BTFSC PORTD, 1
    GOTO PORTD21EQ01
    GOTO PORTD21EQ00
    
PORTD21EQ01:
    BTFSC PORTD, 0
    GOTO TASK_XOR
    GOTO TASK_AND
    
PORTD21EQ00:
    BTFSC PORTD, 0
    GOTO TASK_ADD
    GOTO TASK_COMP
    

    
    
    
    
    
    
    
TASK_COMP:
    BCF	PORTD, 7
    BCF PORTD, 6
    BCF PORTD, 3
    CALL SUBROUTINE_COMP
    GOTO MAIN_LOOP
    
TASK_ADD: 
    BCF PORTD, 7 
    BCF PORTD, 6 
    BSF PORTD, 3 
    CALL SUBROUTINE_ADD 
    GOTO MAIN_LOOP
TASK_AND: 
    BCF PORTD, 7 
    BCF PORTD, 6 
    BSF PORTD, 3 
    CALL SUBROUTINE_AND 
    GOTO MAIN_LOOP
TASK_XOR: 
    BCF PORTD, 7 
    BSF PORTD, 6 
    BSF PORTD, 3 
    CALL SUBROUTINE_XOR
    GOTO MAIN_LOOP
TASK_BCD:
    BSF PORTD, 7 
    BCF PORTD, 6 
    BCF PORTD, 3 
    CALL SUBROUTINE_BCD
    GOTO MAIN_LOOP
    
    
    
    
SUBROUTINE_COMP:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF input_a
    COMF input_a,0
    ANDLW 0x0F
    MOVWF result
    MOVFF result, PORTB
    
    BCF PORTE,1
    BNZ CHECK_NOTZ1
    BSF PORTE,1
    
CHECK_NOTZ1:
    RETURN
    
SUBROUTINE_ADD:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF input_a
    MOVF PORTC, W
    ANDLW 0x0F
    MOVWF input_c
    ADDWF input_a,0
    MOVWF result
    MOVFF result, PORTB
    
    BCF PORTE,1
    BNZ CHECK_NOTZ2
    BSF PORTE,1
    
CHECK_NOTZ2:
    RETURN
    
SUBROUTINE_AND:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF input_a
    MOVF PORTC, W
    ANDLW 0x0F
    MOVWF input_c
    ANDWF input_a,0
    MOVWF result
    MOVFF result, PORTB
    
    BCF PORTE,1
    BNZ CHECK_NOTZ3
    BSF PORTE,1
    
CHECK_NOTZ3:
    RETURN
    
SUBROUTINE_XOR:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF input_a
    MOVF PORTC, W
    ANDLW 0x0F
    MOVWF input_c
    XORWF input_a,0
    MOVWF result
    MOVFF result, PORTB
    
    BCF PORTE,1
    BNZ CHECK_NOTZ4
    BSF PORTE,1
    
CHECK_NOTZ4:
    RETURN
    
SUBROUTINE_BCD:
    MOVF PORTA, W
    ANDLW 0x0F
    MOVWF input_a
    MOVLW 0x09
    CPFSGT input_a, 1
    GOTO NGREATER
    MOVLW 0x06
    ADDWF input_a, 0 
    GOTO L1
    
    BCF PORTE,1
    BNZ CHECK_NOTZ5 
    BSF PORTE,1
CHECK_NOTZ5:
    RETURN
NGREATER:
    MOVF input_a, W
L1:
    MOVWF result
    MOVFF result, PORTB
    
    RETURN
    
;CHECK_NOTZ:
;    GOTO MAIN_LOOP
;    
;    BZ SET_BIT
;    BCF PORTE, 1
;    GOTO JUMP
    
SET_BIT:
    BSF PORTE, 1
    
JUMP:
    GOTO MAIN_LOOP
    
    
    END
    
 
 
 

