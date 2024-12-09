/*
 * project_2_isr_def.c
 *
 * Created: 2024. 11. 24. 18:25:07
 *  Author: Sámuel Keresztes | 24271195
 */ 

#include "project_2_includes.h"

extern unsigned char qcntr;
extern unsigned char sndcntr;
extern char queue[64]; /* character queue*/
extern uint8_t usart3_transmitting;   // Flag to indicate an ongoing transmission

extern uint8_t servo_stepper_interval;		/* interval for the PWM signal on WO0 */
extern uint8_t adc_data_flag;
extern uint16_t adc_res;

extern uint16_t full_signal_period_555_us;
extern uint16_t low_signal_period_555_us;
extern uint16_t high_signal_period_555_us;
extern uint8_t timer_555_input_capture_flag;


ISR (ADC0_RESRDY_vect)
{
	adc_res = ADC0_RES;
	adc_data_flag = NEW_ADC_DATA;
	
	if (adc_res > ADC_VAL_3_5_V)
	{
		PORTA.OUTSET = PIN1_bm;
	}
	else
	{
		PORTA.OUTCLR = PIN1_bm;
	}
}



ISR (TCB0_INT_vect)
{
	/*
		The first edge of the pulse at t = 0 (say) is called the leading edge, the rising edge or positive going edge. 
		The second edge, at t = T1, is called the trailing edge, the falling edge or the negative going edge. 
	*/
	
	/**/
	TCB0.INTFLAGS = TCB_CAPT_bm;
	
	full_signal_period_555_us = TCB0.CNT; /* The time period in clock cycles */
	low_signal_period_555_us = TCB0.CCMP; 
	high_signal_period_555_us = (full_signal_period_555_us - low_signal_period_555_us);
	
	timer_555_input_capture_flag = NEW_TIMER_DATA;
	
	TCB1.CNT = 0;
	
	if (full_signal_period_555_us > 2000)
	{
		PORTC.OUTSET = PIN6_bm; /* (LED bit 4) */
		
	}
	else
	{
		PORTC.OUTCLR = PIN6_bm; /* (LED bit 4) */
	}
	
	if (full_signal_period_555_us > 3000)
	{
		PORTB.OUTSET = PIN2_bm; /* (LED bit 5) */
		
	}
	else
	{
		PORTB.OUTCLR = PIN2_bm; /* (LED bit 5) */
	}
	
	PORTF.OUTCLR = PIN4_bm; /* (LED bit 6) */
}

ISR (TCB1_INT_vect)
{
	TCB1.INTFLAGS = TCB_CAPT_bm;
	
	PORTF.OUTSET = PIN4_bm; /* (LED bit 6) */
	PORTB.OUTCLR = PIN2_bm; /* (LED bit 5) */
	PORTC.OUTCLR = PIN6_bm; /* (LED bit 4) */
	
	timer_555_input_capture_flag = OLD_TIMER_DATA;
}


ISR (TCB2_INT_vect)
{
	static uint8_t tcb2_counter = 0;
	static uint8_t servo_dir = SERVO_DIR_POS;
	static uint16_t stepper_val = SERVOM_90N_CMPVAL_ORIG;
	
	/* clear the interrupt flag */
	TCB2.INTFLAGS = TCB_CAPT_bm;
	
	/* if step mode was selected */
	if (servo_stepper_interval != STEPPER_INTERVAL_0)
	{
		tcb2_counter += 1;

		if (tcb2_counter >= servo_stepper_interval)
		{
			move_stepper(&servo_dir, &stepper_val);
			TCA0.SINGLE.CMP0BUF = stepper_val;
			tcb2_counter = 0;
		}
	}
}

ISR (USART3_TXC_vect)
{	
	USART3.STATUS |= USART_TXCIF_bm;
	
	if (qcntr > sndcntr)
	{
		USART3.TXDATAL = queue[sndcntr++];
	}
	else
	{
		// Message transmission complete
		usart3_transmitting = MSG_TRANSMITION_FINISHED;	
	}
	/* Stop sending when the queue is empty. TXC interrupts only happen when a character 
	   has been transmitted. Stoppping sending stops the interrupts */
	
}
