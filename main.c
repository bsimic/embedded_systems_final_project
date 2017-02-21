#define F_CPU 7372800UL
//#define FOSC 7372800// Clock Speed
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "lcd.h"

void writeLCD(uint16_t adc) {
	lcd_clrscr();

	char adcStr[16];
	itoa(adc, adcStr, 10);
	lcd_puts(adcStr);
	
	if(adc > 300) {
		lcd_gotoxy(2,1);
		lcd_puts("DAN");
	} else {
		lcd_gotoxy(2,1);
		lcd_puts("NOC");
	} 
	
}

ISR(ADC_vect) {
	writeLCD(ADC);
}

void init_ADC_LCD() {
		DDRD = _BV(4);

		TCCR1A = _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
		TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
		OCR1A = 92;
		OCR1B = 31;

		lcd_init(LCD_DISP_ON);

		TCCR0 = _BV(WGM01) | _BV(CS02) | _BV(CS00);
		OCR0 = 180;

		TIMSK |= _BV(OCIE0);

		ADMUX = _BV(REFS1) | _BV(REFS0);
		ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) ;
		SFIOR = _BV(ADTS1) | _BV(ADTS0);

		sei();
}

void USART_Init( unsigned int ubrr) {
	/* Set baud rate */
	//UBRRH = (unsigned char)(ubrr>>8);
	UBRRH = 0;
	UBRRL = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE) | (1<<TXCIE);
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

char USARTReadChar() {
	
	while(!UCSRA && (1<<RXC));
	
	return UDR;	
}

int main(void)
{	
	char data,buffer[16];
	USART_Init ( MYUBRR );
	init_ADC_LCD();
	
	while (1){
		data = USARTReadChar();
		itoa(data,buffer,10);
		lcd_gotoxy(8,1);
		lcd_puts(buffer);	
	}
	
}
