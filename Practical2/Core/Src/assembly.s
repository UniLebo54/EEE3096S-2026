/*
 * assembly.s
 *
 */
 
 @ DO NOT EDIT
	.syntax unified
    .text
    .global ASM_Main
    .thumb_func

@ DO NOT EDIT
vectors:
	.word 0x20002000
	.word ASM_Main + 1

@ DO NOT EDIT label ASM_Main
ASM_Main:

	@ Some code is given below for you to start with
	LDR R0, RCC_BASE  		@ Enable clock for GPIOA and B by setting bit 17 and 18 in RCC_AHBENR
	LDR R1, [R0, #0x14]
	LDR R2, AHBENR_GPIOAB	@ AHBENR_GPIOAB is defined under LITERALS at the end of the code
	ORRS R1, R1, R2
	STR R1, [R0, #0x14]

	LDR R0, GPIOA_BASE		@ Enable pull-up resistors for pushbuttons
	MOVS R1, #0b01010101
	STR R1, [R0, #0x0C]
	LDR R1, GPIOB_BASE  	@ Set pins connected to LEDs to outputs
	LDR R2, MODER_OUTPUT
	STR R2, [R1, #0]
	MOVS R2, #0         	@ NOTE: R2 will be dedicated to holding the value on the LEDs

@ TODO: Add code, labels and logic for button checks and LED patterns

main_loop:
    @ default increment 1 every 0.7
	MOVS R3,#1
	LDR R4, LONG_DELAY_CNT

    @ Check SW0 (PA0) - increment by 2 when pressed
	MOVS R7, #0x01           @ Load mask for SW0 (PA0)
	TST R6, R7               @ Test bit 0 (SW0)
    	BNE check_sw1            @ If not pressed, check next button
    	MOVS R3, #2              @ Set increment to 2
  	LDR R4, LONG_DELAY_CNT   @ Use long delay (0.7s)
    	B check_sw2              @ Continue checking other buttons

check_sw1:
    @ Check SW1 (PA1) - faster timing when pressed
    	MOVS R7, #0x02           @ Load mask for SW1 (PA1)
    	TST R6, R7               @ Test bit 1 (SW1)
    	BNE check_sw2            @ If not pressed, check next button
    	MOVS R3, #1              @ Set increment to 1 (default)
    	LDR R4, SHORT_DELAY_CNT  @ Use short delay (0.3s)
    	B check_sw3              @ Continue checking other buttons

check_sw2:
    @ Check SW2 (PA2) - set pattern to 0xAA when pressed
    	MOVS R7, #0x04           @ Load mask for SW2 (PA2)
    	TST R6, R7               @ Test bit 2 (SW2)
    	BNE check_sw3            @ If not pressed, check next button
    	MOVS R2, #0xAA           @ Set LED pattern to 0xAA
    	B write_leds             @ Skip increment and go to display

check_sw3:
    @ Check SW3 (PA3) - freeze pattern when pressed
    	MOVS R7, #0x08           @ Load mask for SW3 (PA3)
    	TST R6, R7               @ Test bit 3 (SW3)
    	BNE buttons_done         @ If not pressed, continue normally
    	B write_leds             @ Freeze - skip increment

buttons_done:
    @ --- Update LED pattern normally ---
    	ADDS R2, R2, R3          @ Increment LED pattern

    @ --- Delay ---
    	MOV R7, R4               @ Load delay counter
delay_loop:
    	SUBS R7, R7, #1
    	BNE delay_loop

write_leds:
    @ --- Write to LEDs ---
    	STR R2, [R1, #0x14]      @ Output to GPIOB_ODR (LEDs)
    	B main_loop

@ LITERALS; DO NOT EDIT
	.align
RCC_BASE: 			.word 0x40021000
AHBENR_GPIOAB: 		.word 0b1100000000000000000
GPIOA_BASE:  		.word 0x48000000
GPIOB_BASE:  		.word 0x48000400
MODER_OUTPUT: 		.word 0x5555

@ TODO: Add your own values for these delays

LONG_DELAY_CNT: 	.word 750000
SHORT_DELAY_CNT: 	.word 60000
