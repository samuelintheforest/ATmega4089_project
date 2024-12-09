/*
 * project_2_includes.h
 *
 * Created: 2024. 11. 06. 11:34:08
 * Author : Sámuel Keresztes | 24271195
 * 
 *
 */ 


#ifndef PROJECT_2_INCLUDES_H_
#define PROJECT_2_INCLUDES_H_

/* ########## Include AVR Headers ########## */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdio.h>

/* ########## macros, and struct abstractions ########## */

/* --- Macros --- */

/* BAUD calculation 
The formula:
- fCLK_PER=20MHz, desired Baud Rate = 115200 bps
- BAUD = (64*fCLK_PER)/(S*fBAUD), S=16 ; Normal Mode => BAUD = 694,44444 ~ 694 (integer)
*/
#define BAUD_VAL 694

/* servo misc. */

//#define FULL_ROT_MODE
#ifndef FULL_ROT_MODE
	#define SERVOM_90N_CMPVAL_ORIG 1249	/* ~1 ms */
	#define SERVOM_90P_CMPVAL_ORIG 2499	/* ~2 ms */
#else
	#define SERVOM_90N_CMPVAL_ORIG 434	/* 0,348 ms */
	#define SERVOM_90P_CMPVAL_ORIG 3180	/* 2,624 ms */
#endif
#define NUMBER_OF_STEP 25
#define STEP_VAL (SERVOM_90P_CMPVAL_ORIG - SERVOM_90N_CMPVAL_ORIG) / NUMBER_OF_STEP


/* logic and state macros */
#define SERVO_DIR_POS 0
#define SERVO_DIR_NEG 1
#define NEW_ADC_DATA 0
#define OLD_ADC_DATA 1
#define NEW_TIMER_DATA 0
#define OLD_TIMER_DATA 1
#define CONT_DISPLAY_TIMER_MODE 0
#define CONT_DISPLAY_ADC_MODE 1
#define CONT_DISPLAY_PAUSE_MODE 2
#define MSG_TRANSMITION_ONGIONG 0
#define MSG_TRANSMITION_FINISHED 1


/* 
- Voltage value calculation:
VAL = (Vin / Vref) * (2^10 - 1) 
*/
#define ADC_VAL_3_5_V 716



/*
servo stepper interval: 
0 - 0
*/
#define STEPPER_INTERVAL_0 0	/* no step */ 
#define STEPPER_INTERVAL_1 200	/* 1s: (1 / 0,005) */
#define STEPPER_INTERVAL_2 150	/* 0,75s: (0,75 / 0,005) */
#define STEPPER_INTERVAL_3 100	/* 0,5s: (0,5 / 0,005) */
#define STEPPER_INTERVAL_4 80	/* 0,4s: (0,4 / 0,005) */
#define STEPPER_INTERVAL_5 50	/* 0,25s: (0,25 / 0,005) */
#define STEPPER_INTERVAL_6 40	/* 0,2s: (0,2 / 0,005) */
#define STEPPER_INTERVAL_7 30	/* 0,15s: (0,15 / 0,005) */
#define STEPPER_INTERVAL_8 20	/* 0,1s: (0,1 / 0,005) */
#define STEPPER_INTERVAL_9 10	/* 0,05s: (0,05 / 0,005) */



/* ########## Function Declaration ########## */

/* --- methods for initialization of peripherals, devices, clocks, etc --- */

/* ---------------------
 * void CLOCK_init(void)
 * ---------------------
 * Configures the system clock to operate at 20 MHz.
 *
 * This function disables the prescaler for CLK_PER, allowing the CPU
 * to run at its maximum frequency of 20 MHz. As the register is protected,
 * it uses the `ccp_write_io` macro to safely modify it.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void CLOCK_init(void);

/* -----------------------
 * void IOPORTS_init(void)
 * -----------------------
 * Configures the direction of various I/O ports.
 *
 * Sets up the pins for USART3, LEDs, and TCA0 PWM output. Defines TX
 * as an output, RX as an input, and assigns output pins for multiple LEDs.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void IOPORTS_init(void);

/* --------------------
 * void USART_init(void)
 * ---------------------
 * Initializes USART3 for asynchronous communication.
 *
 * Configures USART3 with the following settings:
 *   - 115200 baud rate
 *   - 8-bit data
 *   - No parity
 *   - 1 stop bit
 *   - TX and RX enabled
 * TX complete interrupt is also enabled.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void USART_init(void);

/* --------------------
 * void ADC0_init(void)
 * --------------------
 * Configures and enables ADC0 with 10-bit resolution.
 *
 * Settings include:
 *   - AIN3 as the positive input
 *   - AVDD as the reference voltage
 *   - No sample accumulation
 *   - Event-controlled conversion enabled
 *   - Prescaler set to divide clock by 64
 * Enables ADC interrupt on result ready.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void ADC0_init(void);

/* --------------------
 * void TCA0_init(void)
 * --------------------
 * Initializes TCA0 in single-slope PWM mode.
 *
 * Configures TCA0 for:
 *   - 50 Hz PWM frequency
 *   - Initial position at -90 degrees (1 ms on-time)
 *   - CLK_PER divided by 16 as the clock source
 * Output is mapped to WO0 (PORTA pin 0).
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void TCA0_init(void);

/* ---------------------
 * void EVSYS_init(void)
 * ---------------------
 * Configures the Event System for routing signals.
 *
 * Routes the following:
 *   - Generator: TCB2 capture interrupt -> User: ADC0 (Channel 2)
 *   - Generator: PORT E pin 3 -> User: TCB0 (Channel 4)
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void EVSYS_init(void);

/* --------------------
 * void TCB0_init(void)
 * --------------------
 * Configures TCB0 for pulse width and frequency measurement.
 *
 * Settings include:
 *   - Clock source: CLK_PER divided by 2
 *   - Mode: Frequency and Pulse Width Measurement
 *   - Falling edge event input
 *   - Capture interrupt enabled
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void TCB0_init(void);

/* --------------------
 * void TCB1_init(void)
 * --------------------
 * Initializes TCB1 as a watchdog for input signal timing.
 *
 * Configured for periodic interrupts with a capture value of 20,000,
 * corresponding to approximately 500 Hz (~2 ms).
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void TCB1_init(void);

/* --------------------
 * void TCB2_init(void)
 * --------------------
 * Configures TCB2 as a periodic timer with a 5 ms interval.
 *
 * Settings include:
 *   - Clock source: CLK_PER divided by 2
 *   - Mode: Periodic Interrupt Mode
 *   - CCMP set to 50,000 (calculated for 5 ms)
 * Enables capture interrupts.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void TCB2_init(void);

/* ---------------------------
 * void transmitmsg(char* msg)
 * ---------------------------
 * Sends a string message via USART3.
 *
 * Enqueues the message for transmission and starts the process by
 * sending the first character. Subsequent characters are transmitted
 * in the USART3 TXC interrupt.
 *
 * Parameters:
 *   msg - Pointer to the null-terminated string to transmit.
 *
 * Returns:
 *   None
 */
void transmitmsg(char* msg);

/* ------------------------------------------------------
 * void move_stepper(uint8_t *dir, uint16_t *stepper_val)
 * ------------------------------------------------------
 * Moves the stepper servo in the specified direction.
 *
 * Adjusts the PWM duty cycle value (`stepper_val`) in steps and switches
 * direction upon reaching the configured limits (-90° to +90°).
 *
 * Parameters:
 *   dir        - Pointer to the direction variable (SERVO_DIR_POS or SERVO_DIR_NEG).
 *   stepper_val - Pointer to the current stepper value (PWM duty cycle).
 *
 * Returns:
 *   None
 */
void move_stepper(uint8_t *dir, uint16_t *stepper_val);


#endif /* PROJECT_2_INCLUDES_H_ */