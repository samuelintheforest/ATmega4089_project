/*
 * project_2_defs.c
 *
 * Created: 2024. 11. 06. 11:32:24
 * Author : S�muel Keresztes | 24271195
 * 
 *
 */

#include "project_2_includes.h"

/* ########## globals definition ########## */
char queue[64];												/* msg to be transimmted */
unsigned char qcntr;										/* counter for the queue */
unsigned char sndcntr;										/* counter for the sent ch */
uint8_t usart3_transmitting = MSG_TRANSMITION_FINISHED;		/* usart3 tx flag -> synchronization */

uint8_t servo_stepper_interval = STEPPER_INTERVAL_0;		/* interval for the PWM signal on WO0 */
uint8_t adc_data_flag = OLD_ADC_DATA;						/* adc flag */
uint16_t adc_res = 0;										/* adc value (0-1023) */
uint8_t continuous_display_mode = CONT_DISPLAY_PAUSE_MODE;	/* display mode flag */

uint16_t full_signal_period_555_us = 0;						
uint16_t low_signal_period_555_us = 0;
uint16_t high_signal_period_555_us = 0;
uint8_t timer_555_input_capture_flag = OLD_TIMER_DATA;


void CLOCK_init(void)
{
	/* Disable CLK_PER Prescaler */
	ccp_write_io( (void *) &CLKCTRL.MCLKCTRLB , (0 << CLKCTRL_PEN_bp));
	/* If set from the fuses during device programming, the CPU will now run at 20MHz (default is /6) */
}

void IOPORTS_init(void)
{
	/* USART3 port setup */
	PORTB.DIRSET = PIN4_bm;		/* this is the TX output */
	PORTB.DIRCLR = PIN5_bm;		/* this is the RX input */
	
	/* LED port setup */
	PORTC.DIRSET = PIN6_bm;		/* this is LED 4 output */
	PORTB.DIRSET = PIN2_bm;		/* this is LED 5 output */
	PORTF.DIRSET = PIN4_bm;		/* this is LED 6 output */
	PORTA.DIRSET = PIN1_bm;		/* this is LED 7 output */
	
	/* TCA0 PWM output on PORTA pin 0 - WO0*/
	PORTA.DIRSET = PIN0_bm;
}

void USART_init(void)
{
	PORTMUX.USARTROUTEA |= PORTMUX_USART3_ALT1_gc;

	USART3.CTRLB |= (USART_TXEN_bm | USART_RXEN_bm);
	USART3.CTRLC |= (USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc |  USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc);
	
	USART3.TXDATAL = 0b00000000; /* set a default value for the transmition chanel */

	USART3.BAUD = (uint16_t)BAUD_VAL;
	USART3.CTRLA |= USART_TXCIE_bm;
}

void ADC0_init(void)
{	
	ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc; /* port forward to AIN3 */
	
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
	ADC0.CTRLB |= ADC_SAMPNUM_ACC1_gc;
	ADC0.CTRLC |= (ADC_SAMPCAP_bm | ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV64_gc);
	ADC0.CTRLD |= ADC_INITDLY0_bm;
	
	ADC0.INTCTRL |= ADC_RESRDY_bm; /* enable interrupt */
	
	ADC0.CTRLA |= ADC_ENABLE_bm;
	ADC0.EVCTRL |= ADC_STARTEI_bm; /* start conversion */
}

void TCA0_init(void)
{
	PORTMUX.TCAROUTEA |= PORTMUX_TCA0_PORTA_gc; /* WO0 on PA0 */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV16_gc;
	TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc);
	TCA0.SINGLE.PER = 24999; /* 20 ms - 50 Hz PWM output */
	TCA0.SINGLE.CMP0 = SERVOM_90N_CMPVAL_ORIG; /* 1 ms - initial position */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;	
}

void TCB0_init(void)
{
	TCB0.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
	TCB0.CTRLB |= TCB_CNTMODE_FRQPW_gc; /* Clock Frequency - Pulse Width Measurement mode */
	TCB0.INTCTRL |= TCB_CAPT_bm;
	TCB0.EVCTRL |= (TCB_EDGE_bm | TCB_CAPTEI_bm); /* interrupt occurs on the second falling edge (EDGE = 1) */
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

void TCB1_init(void)
{
	
	/* 
		- Timer/Counter TCB1 Clock source: CLK_PER/2 and Enable TCB1 (CTRLA)
		- The remaining settings are left as a challenge.
	*/
	
	TCB1.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
	TCB1.CTRLB |= TCB_CNTMODE_INT_gc;
	TCB1.INTCTRL |= TCB_CAPT_bm;
	TCB1.CCMP = (uint16_t)20000; /* ~ 500Hz value ~ after 2ms of no CNT change, it will throws an interrupt*/
	TCB1.CTRLA |= TCB_ENABLE_bm;
}

void TCB2_init(void)
{
	/*
		- Initialise Timer/Counter TCB2 for period timer mode to set a period of 5ms. - W
		- Timer/Counter TCB2 Clock source: CLK_PER/2 and Enable TCB2 (CTRLA) - W
		- Configure TCB2 for Periodic Interrupt Mode (CTRLB), TCB2 interrupts enabled. - W
		- Set TCB2.CCMP to give a period of 5ms. - W
	*/
	
	TCB2.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
	TCB2.CTRLB |= TCB_CNTMODE_INT_gc;
	TCB2.CCMP = (uint16_t)50000; /* Calculation: (CLK_PER / 2) / (1 / 0,005) => (20000000 / 2) / (200) => CCMP = 50000 */
	TCB2.INTCTRL |= TCB_CAPT_bm;
	TCB2.CTRLA |=  TCB_ENABLE_bm;
	
}

void EVSYS_init(void)
{
	/*
	- Generator	-> TCB2_CAP interrupt
	- User		-> ADC0
	- Channel	-> CHANNEL2
	*/
	EVSYS.CHANNEL2 = EVSYS_GENERATOR_TCB2_CAPT_gc;
	EVSYS.USERADC0 = EVSYS_CHANNEL_CHANNEL2_gc;
	
	/*
	- Generator	-> PORTE Pin 3
	- User		-> TCB0
	- Channel	-> CHANNEL4
	*/
	EVSYS.CHANNEL4 = EVSYS_GENERATOR_PORT0_PIN3_gc;
	EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL4_gc;

}

void transmitmsg(char* msg)
{
	while (usart3_transmitting != MSG_TRANSMITION_FINISHED) 
	{
		/* Wait for the previous TX operation to complete.
		 * This ensures that all sent characters will arrive to the io display
		*/
	}
	
	/* reset the queue indices */
	qcntr = 0;
	sndcntr = 1; /* the first character has been already sent */
	
	while (*msg)
	{
		queue[qcntr++] = *msg++;
	}
	
	/* Start MSG transmition */
	usart3_transmitting = MSG_TRANSMITION_ONGIONG; /* transmition has been started */
	
	USART3.TXDATAL = queue[0];  /* first character begins the operation */
	
}

void move_stepper(uint8_t *dir, uint16_t *stepper_val)
{
	if (*dir == SERVO_DIR_POS)
	{
		*stepper_val += (uint16_t)STEP_VAL;
		if (*stepper_val >= SERVOM_90P_CMPVAL_ORIG)
		{
			*dir = SERVO_DIR_NEG;
		}
	}
	else if (*dir == SERVO_DIR_NEG)
	{
		*stepper_val -= (uint16_t)STEP_VAL;
		if (*stepper_val <= SERVOM_90N_CMPVAL_ORIG)
		{
			*dir = SERVO_DIR_POS;
		}
	}	
}
