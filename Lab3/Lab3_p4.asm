#include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF 
 Delay1 equ 0xFF 
Delay2 equ 0xFF 
 ORG 0x0000 
; CODE STARTS FROM THE NEXT LINE 
START: 
 ORG 0x0000 
START: 
 MOVLW 0x0F ; Load W with 0x0F0 
 MOVWF ADCON1 ; Make ADCON1 to be all digital 
 MOVLW 0x00 ; Load W with 0x00 
 MOVWF TRISC ; Make PORT C as outputs 
 ;MOVWF TRISD
 
MAIN_LOOP: ; start of While LOOP 
 MOVLW 0x00 ; load W with 0 
 MOVWF 0x20 ; store W to location 0x20 
 MOVLW 0x08 ; load W with 08 
 MOVWF 0x21 ; store W to location 0x21 
 
FOR_LOOP: 
 MOVF 0x20,W ; read content of 0x20 into W 
 MOVWF PORTC ; output W to PORT C 
 ;SUBLW 0x07
 ;MOVWF PORTD
 
 
 CALL DELAY_ONE_SEC ; wait one sec 
 INCF 0x20,F ; increment location 0x20 by 1 
 DECF 0x21,F ; decrement location 0x21 by 1 
 BNZ FOR_LOOP ; if not equal, then (0x21) not equal to 0 
 ; go back to FOR_LOOP 
 GOTO MAIN_LOOP ; go back to While LOOP 
DELAY_ONE_SEC: 
 MOVLW Delay1 ; Load constant Delay1 into W 
 MOVWF 0x28 ; Load W to memory 0x21 
LOOP_1_OUTER: 
 NOP ; Do nothing 
 MOVLW Delay2 ; Load constant Delay2 into W 
 MOVWF 0x29 ; Load W to memory 0x29 
LOOP_1_INNER: 
 NOP ; Do nothing 
 DECF 0x29,F ; Decrement memory location 0x20 
 BNZ LOOP_1_INNER ; If value not zero, go back to 
 DECF 0x28,F ; Decrement memory location 0x28 
 BNZ LOOP_1_OUTER ; If value not zero, go back to 
RETURN
END 


