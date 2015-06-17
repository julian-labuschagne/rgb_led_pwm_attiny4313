/**
 * First AVR Program
 * 
 * Blinking RGB LED with the ATTiny4313
 * 
 * Author       : Julian Labuschagne
 * Date         : 2015/06/12
 * Last Updated : 2015/06/14
 */ 
 
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define MYUBRR F_CPU/8/BAUD-1

#define LED_CLEAR 	PB2
#define LED_RED 	PD5
#define LED_GREEN 	PB3
#define LED_BLUE 	PB4

#define SERIAL_RECEIVE PD0
#define SERIAL_TRANSMIT PD1

#define DELAY_TIME 10;

void display_color(uint8_t white, uint8_t red, uint8_t green, uint8_t blue);
void cycle_test(uint8_t speed);
void cycle_colors(uint8_t speed);
void delay(uint8_t ms);

void usart_init(uint16_t ubrr);
void usart_putchar(char data);

int main(void) {
	
	/**
	 * Counter 0
	 * Setup  fast PWM (mode 3)
	 */ 
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);
	TCCR0B |= (1 << CS01);
	
	/**
	 * Counter 1
	 * Setup fast PWM 
	 */
	TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1); 
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	
	/**
	 * Enable USART
	 */
	usart_init(MYUBRR);
	
	// -------- Inits -------- //
	DDRB |= (1 << LED_GREEN) | (1 << LED_BLUE) | (1 << LED_CLEAR);
	DDRD |= (1 << LED_RED);
	
	// ------ Event loop ----- //
	
	while (1) {
		
		//cycle_test(10);
		//cycle_colors(15);
		usart_putchar('H');
		usart_putchar('e');
		usart_putchar('l');
		usart_putchar('l');
		usart_putchar('o');
		usart_putchar(' ');
		usart_putchar('W');
		usart_putchar('o');
		usart_putchar('r');
		usart_putchar('l');
		usart_putchar('d');
		usart_putchar('!');
		usart_putchar('\n');
		
		_delay_ms(1000);

	}
	
	return 0;
}

void usart_init(uint16_t ubrr) {
	
	// Set baud rate
	UBRRH = (uint8_t) (ubrr>>8);
	UBRRL = (uint8_t) ubrr;
	
	// Double the USART Transmission Speed
	UCSRA |= (1 << U2X);
	
	// Enable receiver and transmitter
	UCSRB |= (1 << RXEN) | (1 << TXEN);
	
	// Set frame format: 8 data, 1 stop bit
	UCSRC |= (1 << UCSZ0) | (1 << UCSZ1);

}

void usart_putchar(char data) {
	// Wait for empty transmit buffer
	while (!(UCSRA & (1 << UDRE)));
	// Start transmission
	UDR = data;
}

void display_color(uint8_t white, uint8_t red, uint8_t green, uint8_t blue) {
	
	OCR0A = white;
	OCR0B = red;
	OCR1A = green;
	OCR1B = blue;
	
	if(OCR0A == 0) {
		DDRB &= ~(1 << PB2);
	}
	else {
		DDRB |= (1 << PB2);
	}
	
	if(OCR0B == 0) {
		DDRD &= ~(1 << LED_RED);
	}
	else {
		DDRD |= (1 << LED_RED);
	}
	
	if(OCR1A == 0) {
		DDRB &= ~(1 << LED_GREEN);
	}
	else {
		DDRB |= (1 << LED_GREEN);
	}
	
	if(OCR1B == 0) {
		DDRB &= ~(1 << LED_BLUE);
	}
	else {
		DDRB |= (1 << LED_BLUE);
	}	
}

void cycle_test(uint8_t speed) {
	
	int i;
	
	for (i = 0; i < 256; i++) {
		display_color(i, 0, 0, 0);
		delay(speed);
	}

	for (i = 255; i >= 0; i--) {
		display_color(i, 0, 0, 0);
		delay(speed);
	}

	for (i = 0; i < 256; i++) {
		display_color(0, i, 0, 0);
		delay(speed);
	}

	for (i = 255; i >= 0; i--) {
		display_color(0, i, 0, 0);
		delay(speed);
	}

	for (i = 0; i < 256; i++) {
		display_color(0, 0, i, 0);
		delay(speed);
	}

	for (i = 255; i >= 0; i--) {
		display_color(0, 0, i, 0);
		delay(speed);
	}

	for (i = 0; i < 256; i++) {
		display_color(0, 0, 0, i);
		delay(speed);
	}

	for (i = 255; i >= 0; i--) {
		display_color(0, 0, 0, i);
		delay(speed);
	}
}

void cycle_colors(uint8_t speed) {
	
	int i;
	
	uint8_t white = 0;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	
	for(i = 0; i < 256; i++) {
		
		red = 255 - i;
		green = i;
		blue = 0;
		
		display_color(white, red, green, blue);
		delay(speed);
	}
	
	for(i = 0; i < 256; i++) {
		
		red = 0;
		green = 255 - i;
		blue = i;
		
		display_color(white, red, green, blue);
		delay(speed);
	}
	
	for(i = 0; i < 256; i++) {
		
		red = i;
		green = 0;
		blue = 255 - i;
		
		display_color(white, red, green, blue);
		delay(speed);
	}
}

void delay(uint8_t ms) {
	
	int i;
	
	for (i = 0; i < ms; i++) {
		_delay_ms(1);
	}
	
}
