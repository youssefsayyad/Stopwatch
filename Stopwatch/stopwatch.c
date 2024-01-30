/*
 * stopwatch.c
 *
 *  Created on: Sep 14, 2023
 *      Author: Youss
 */
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char seconds = 0;
unsigned char mins = 0;
unsigned char hrs = 0;
void TIMER1_init(void) {
	TCCR1A = (1 << FOC1A);
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS11);
	TCNT1 = 0;
	OCR1A = 234375;
	TIMSK = (1 << OCIE1A);
	SREG = (1 << 7);
}
ISR(TIMER1_COMPA_vect) {
	seconds++;
	if (seconds > 59) {
		mins++;
		seconds = 0;
	}
	if (mins > 59) {
		mins = 0;
		hrs++;
	}
}
void INT0_init(void) {
	MCUCR |= (1 << ISC01);     	//INT-0 WITH FALLING EDGE SENSE
	GICR |= (1 << INT0);		//INT-0 REQUEST ENABLE
	SREG |= (1 << 7);
}
ISR(INT0_vect) {				//TIMER RESET
	TCNT1 = 0;
	seconds = 0;
	mins = 0;
	hrs = 0;
}
void INT1_init(void) {
	MCUCR |= (1 << ISC11) | (1 << ISC10); 	//INT-1 WITH RISING EDGE SENSE
	GICR |= (1 << INT1);					//INT-1 REQUEST ENABLE
	SREG |= (1 << 7);
}
ISR(INT1_vect) {				//CLK TURNED OFF
	TCCR1B &= ~(1 << CS10) & ~(1 << CS11);
}
void INT2_init(void) {
	MCUCSR |= (1 << ISC2);  	 //INT-2 WITH FALLING EDGE SENSE
	GICR |= (1 << INT2);		//INT-2 REQUEST ENABLE
	SREG |= (1 << 7);
}
ISR(INT2_vect) { 				//CLK TURNED ON
	TCCR1B |= (1 << CS10) | (1 << CS11);
}
void DisplaySeconds(unsigned char seconds) {
	unsigned char tempUnits = 0, tempTenths = 0;
	tempUnits = seconds % 10;
	tempTenths = seconds / 10;
	PORTA = 0x20;
	PORTC = (PORTC & 0xF0) | (tempUnits & 0x0F); //DISPLAYING SECONDS UNIT
	_delay_ms(3);
	PORTA = 0x10;
	PORTC = (PORTC & 0xF0) | (tempTenths & 0x0F); //DISPLAYING SECONDS TENTHS
	_delay_ms(3);
}
void DisplayMinutes(unsigned char mins) {
	unsigned char tempUnits = 0, tempTenths = 0;
	tempUnits = mins % 10;
	tempTenths = mins / 10;
	PORTA = 0x08;
	PORTC = (PORTC & 0xF0) | (tempUnits & 0x0F); //DISPLAYING MINUTES UNIT
	_delay_ms(3);
	PORTA = 0x04;
	PORTC = (PORTC & 0xF0) | (tempTenths & 0x0F); //DISPLAYING MINUTES TENTHS
	_delay_ms(3);
}
void DisplayHours(unsigned char mins) {
	unsigned char tempUnits = 0, tempTenths = 0;
	tempUnits = hrs % 10;
	tempTenths = hrs / 10;
	PORTA = 0x02;
	PORTC = (PORTC & 0xF0) | (tempUnits & 0x0F); //DISPLAYING HOURS UNIT
	_delay_ms(3);
	PORTA = 0x01;
	PORTC = (PORTC & 0xF0) | (tempTenths & 0x0F); //DISPLAYING HOURS TENTHS
	_delay_ms(3);
}
int main() {
	DDRA = 0xFC;						//7-SEGMENT ANODES
	PORTA = 0x00;
	DDRB &= ~(1 << PB2);				//INT-2 PUSH BUTTON
	PORTB |= (1 << PB2);			//PUSH BUTTON INT-2 INTERNAL PULL UP CONFIG
	DDRC = 0x0F; 						//7-SEGMENT PINS
	DDRD &= ~(1 << PD2) & ~(1 << PD3);	//INT-0 ,INT-1 PUSH BUTTON
	PORTD |= (1 << PD2);			//PUSH BUTTON INT-0 INTERNAL PULL UP CONFIG
	TIMER1_init();
	INT0_init();
	INT1_init();
	INT2_init();
	while (1) {
		DisplaySeconds(seconds);
		DisplayMinutes(mins);
		DisplayHours(hrs);
	}
}

