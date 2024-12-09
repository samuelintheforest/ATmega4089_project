/*
 * main.c
 *
 * Created: 2024. 11. 06. 11:11:35
 * Author : Sámuel Keresztes | 24271195
 * 
 *
 */ 

#include "project_2_includes.h" 

extern uint8_t servo_stepper_interval;		/* interval for the PWM signal on WO0 */
extern uint8_t adc_data_flag;
extern uint16_t adc_res;
extern uint8_t continuous_display_mode;

extern uint16_t full_signal_period_555_us;
extern uint16_t low_signal_period_555_us;
extern uint16_t high_signal_period_555_us;
extern uint8_t timer_555_input_capture_flag;

int main(void)
{
	char ch;
	char msg_buffer[64];
	
	CLOCK_init();
	EVSYS_init();
	IOPORTS_init();
	TCA0_init();
	TCB0_init();
	TCB1_init();
	TCB2_init();
	USART_init();
	ADC0_init();
	
	sei();
	
    /* Replace with your application code */
    while (1)
    {
		
		/* Parse USART3 character from the computer */
		if (USART3.STATUS & USART_RXCIF_bm)
		{	/* If a character has been received, read it - this structure allows other code to run */
			ch = USART3.RXDATAL;
			sprintf(msg_buffer, "%s", "\0");
			/* parse input */
			switch (ch)
			{
				case '0': /* No movement step */
					servo_stepper_interval = STEPPER_INTERVAL_0;
					sprintf(msg_buffer, "0: [servo m.] no movement\n");
					break;
				case '1': /* 1s per step */
					servo_stepper_interval = STEPPER_INTERVAL_1;
					sprintf(msg_buffer, "1: [servo m.] 1s steps\n");
					break;
				case '2': /* 0,75s per step */
					servo_stepper_interval = STEPPER_INTERVAL_2;
					sprintf(msg_buffer, "2: [servo m.] 0,75s steps\n");
					break;
				case '3': /* 0,5s per step */
					servo_stepper_interval = STEPPER_INTERVAL_3;
					sprintf(msg_buffer, "3: [servo m.] 0,5s steps\n");
					break;
				case '4': /* 0,4s per step */
					servo_stepper_interval = STEPPER_INTERVAL_4;
					sprintf(msg_buffer, "4: [servo m.] 0,4s steps\n");
					break;
				case '5': /* 0,25s per step */
					servo_stepper_interval = STEPPER_INTERVAL_5;
					sprintf(msg_buffer, "5: [servo m.] 0,25s steps\n");
					break;
				case '6': /* 0,2s per step */
					servo_stepper_interval = STEPPER_INTERVAL_6;
					sprintf(msg_buffer, "6: [servo m.] 0,2s steps\n");
					break;
				case '7': /* 0,15s per step */
				    servo_stepper_interval = STEPPER_INTERVAL_7;
					sprintf(msg_buffer, "7: [servo m.] 0,15s steps\n");
					break;
				case '8': /* 0,1s per step */
					servo_stepper_interval = STEPPER_INTERVAL_8;
					sprintf(msg_buffer, "8: [servo m.] 0,1s steps\n");
					break;
				case '9': /* 0,05s per step */
					servo_stepper_interval = STEPPER_INTERVAL_9;
					sprintf(msg_buffer, "9: [servo m.] 0,05s steps\n");
					break;
				case 't':
				case 'T': /* ‘T’ or ‘t’: Report the period of the 555 Timer in microseconds. */
					if (timer_555_input_capture_flag == NEW_TIMER_DATA)
					{
						sprintf(msg_buffer, "t or T: 555 timer period: %dus\n", (uint16_t)(full_signal_period_555_us / 10));
						timer_555_input_capture_flag = OLD_TIMER_DATA;
					}
					else
					{
						sprintf(msg_buffer, "t or T: 555 timer period: no input signal detected\n");
					}
					break;
				case 'l':
				case 'L': /* ‘L’ or ‘l’: Report the time taken by the low pulse of the 555 Timer signal in microseconds. */ 
					if (timer_555_input_capture_flag == NEW_TIMER_DATA)
					{
						sprintf(msg_buffer, "l or L: 555 timer low pulse period: %dus\n", (uint16_t)(low_signal_period_555_us / 10));
						timer_555_input_capture_flag = OLD_TIMER_DATA;
					}
					else
					{
						sprintf(msg_buffer, "l or L: 555 timer low pulse period: no input signal detected\n");
					}
					break;
				case 'h':
				case 'H': /* ‘H’ or ‘h’: Report the time taken by the high pulse of the 555 Timer signal in microseconds. */
					if (timer_555_input_capture_flag == NEW_TIMER_DATA)
					{
						sprintf(msg_buffer, "h or H: 555 timer high pulse period: %dus\n", (uint16_t)(high_signal_period_555_us / 10));
						timer_555_input_capture_flag = OLD_TIMER_DATA;
					}
					else
					{
						sprintf(msg_buffer, "h or H: 555 timer high pulse period: no input signal detected\n");
					}
					break;
				case 'c':
				case 'C': /* Continuously report the Timer input period in microseconds. Set Continuous Timer Mode. */
					if (continuous_display_mode != CONT_DISPLAY_ADC_MODE)
					{
						continuous_display_mode = CONT_DISPLAY_TIMER_MODE;	
					}
					break;
				case 'e':
				case 'E': /* Stop continuous reporting of Timer input. Clear Continuous Timer Mode. */
					if (continuous_display_mode == CONT_DISPLAY_TIMER_MODE || continuous_display_mode == CONT_DISPLAY_PAUSE_MODE)
					{
						continuous_display_mode = CONT_DISPLAY_PAUSE_MODE;
						sprintf(msg_buffer, "e or E: Stopped Timer continuous report mode\n");	
					}
					break;
				case 'a':
				case 'A': /* Report the ADC conversion result. This is the ADC value. */
					if (adc_data_flag == NEW_ADC_DATA)
					{
						sprintf(msg_buffer, "a or A: ADC conversion result: %d\n", adc_res);
						adc_data_flag = OLD_ADC_DATA;	
					}
					break;
				case 'v':
				case 'V': /* Report the ADC conversion result in mV. You must convert the ADC value to mV.  */
					if (adc_data_flag == NEW_ADC_DATA)
					{
						sprintf(msg_buffer, "v or V: ADC conversion result in mV: %dmV\n", (uint16_t)(adc_res * 1000000 / 1023 * 5 / 1000));
						adc_data_flag = OLD_ADC_DATA;
					}
					break;
				case 'm':
				case 'M': /* Continuously report the ADC conversion result in mV. You must convert the ADC value to mV. Set Continuous ADC Mode. */
					if (continuous_display_mode != CONT_DISPLAY_TIMER_MODE)
					{
						continuous_display_mode = CONT_DISPLAY_ADC_MODE;	
					}
					break;
				case 'n':
				case 'N': /* Stop continuous reporting of ADC input. Clear Continuous ADC Mode. */
					if (continuous_display_mode == CONT_DISPLAY_ADC_MODE || continuous_display_mode == CONT_DISPLAY_PAUSE_MODE)
					{
						continuous_display_mode = CONT_DISPLAY_PAUSE_MODE;
						sprintf(msg_buffer, "n or N: Stopped ADC continuous report mode\n");
					}
					break;
				default:
					sprintf(msg_buffer, "%c: Invalid character\n", ch);
					break;
			}
			
			transmitmsg(msg_buffer);
			
		}
		/* Even if a character has not been received, code inserted here can still run */
		
		
		/* Check for continuous mode operations */
		if (continuous_display_mode == CONT_DISPLAY_TIMER_MODE)
		{
			if (timer_555_input_capture_flag == NEW_TIMER_DATA)
			{
				sprintf(msg_buffer, "c or C: [cont] 555 timer period: %dus\n", (uint16_t)(full_signal_period_555_us / 10));
				timer_555_input_capture_flag = OLD_TIMER_DATA;
			}
			else
			{
				sprintf(msg_buffer, "c or C: [cont] 555 timer period: no input signal detected\n");
			}
			transmitmsg(msg_buffer);
		}
		else if (continuous_display_mode == CONT_DISPLAY_ADC_MODE)
		{
			if (adc_data_flag == NEW_ADC_DATA)
			{
				sprintf(msg_buffer, "m or M: [cont] ADC conversion result in mV: %dmV\n", (uint16_t)(adc_res * 1000000 / 1023 * 5 / 1000));
				transmitmsg(msg_buffer);
				adc_data_flag = OLD_ADC_DATA;
			}
		}
    }
}
