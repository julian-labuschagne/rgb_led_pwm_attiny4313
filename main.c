/**
 * First AVR Program
 * 
 * Blinking RGB LED with the ATTiny4313
 * 
 * Author       : Julian Labuschagne
 * Date         : 2015/06/12
 * Last Updated : 2015/06/29
 */ 
 
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#define BAUD 9600
#define MYUBRR F_CPU/8/BAUD-1

#define LED_CLEAR 	PB2
#define LED_RED 	PD5
#define LED_GREEN 	PB3
#define LED_BLUE 	PB4

#define SERIAL_RECEIVE PD0
#define SERIAL_TRANSMIT PD1

#define DELAY_TIME 10;

// Declare LED PWM functions
void pwm_init(void);
void display_color(uint8_t white, uint8_t red, uint8_t green, uint8_t blue);
void cycle_test(uint8_t speed);
void cycle_colors(uint8_t speed);
void delay(uint8_t ms);

// Declare USART functions
void usart_init(uint16_t ubrr);
void usart_putchar(char data);
void usart_pstr(char * str);
char usart_getchar(void);
void usart_gstr(char myString[], uint8_t maxLength);

// Declare command functions
void command_parse(char * cmd);
void command_response(uint8_t status);
void command_setcolor(char * cmd);

// Global variables
int whiteValue = 0;
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

int main(void) {
	
	/**
	 * Enable PWM
	 */
	pwm_init();
	
	/**
	 * Enable USART
	 */
	usart_init(MYUBRR);
	
	
	// -------- Inits -------- //
	DDRB |= (1 << LED_GREEN) | (1 << LED_BLUE) | (1 << LED_CLEAR);
	DDRD |= (1 << LED_RED);
	
	/**
	 * Display default colors
	 */
	display_color(whiteValue, redValue, greenValue, blueValue);
	
	char serialRead[64]; // Our buffer to record received bytes
	
	// ------ Event loop ----- //
	while (1) {
		
		// Get string from Serial and parse the command
		usart_gstr(serialRead, 64);
		command_parse(serialRead);
		
		//cycle_test(10);
		//cycle_colors(15);

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

void usart_pstr(char *s) {
	
	// Loop through entire string
	while(*s) {
		usart_putchar(*s);
		s++;
	}
	
}

char usart_getchar() {
	// Wait for incoming data
	while(!(UCSRA & (1 << RXC)));
	return UDR;
}

void usart_gstr(char myString[], uint8_t maxLength) {
	char response;
	uint8_t i;
	
	i = 0;
	
	while(i < (maxLength - 1)) {
		
		response = usart_getchar();
		
		if(response == '\n') {
			break;
		} else {
			myString[i] = response;
			i++;
		}
		
		myString[i] = 0;
	
	}
}

void command_parse(char * cmd) {
	
	char serialCommand[6];
	
	strncpy(serialCommand, cmd, 5);
	serialCommand[5] = 0; // null terminate string

	usart_pstr(serialCommand);
	usart_putchar('\n');

	if (strcmp(cmd, "AT") == 0) {
		command_response(1);
	}
	
	else if (strstr(cmd, "AT+SETCOLOR") != 0) {
		usart_pstr("Set color command\n");
		command_setcolor(cmd);
		command_response(1);
	}
	
	else if (strstr(cmd, "AT+SAVECOLOR") != 0) {
		usart_pstr("Save color command\n");
		command_response(1);
	}
	
	else {
		usart_pstr("Unknown command\n");
		command_response(0);
	} 
}

void command_response(uint8_t status) {
	if(status) {
		usart_pstr("OK\n");
	}
	else {
		usart_pstr("ERROR\n");
	}
}

void command_setcolor(char *cmd) {
	
	// We dont want to destoy the original command string
	// So we create a copy to perform strtok() on
	char tmpstr[strlen(cmd) + 1];
	strcpy(tmpstr, cmd);
	
	// Remove everything in front of the equals sign
	strtok(tmpstr, "=");
	
	// Get the color values in the command string
	char *color_values = strtok(NULL, "=");
	
	// Get the value for the White LED and 
	// assign to global variable whiteValue 	
	char *whiteString = strtok(color_values, ",");
	whiteValue = atoi(whiteString);
	
	// Get the value for the Red LED and 
	// assign to global variable redValue	
	char *redString = strtok(NULL, ",");
	redValue = atoi(redString);
	
	// Get the value for the Green LED and 
	// assign to global variable greenValue	
	char *greenString = strtok(NULL, ",");
	greenValue = atoi(greenString);
	
	// Get the value for the Blue LED and 
	// assign to global variable blueValue	
	char *blueString = strtok(NULL, ",");
	blueValue = atoi(blueString);
		
	// Display the color
	display_color(whiteValue, redValue, greenValue, blueValue);
	
	// Echo the command 
	usart_pstr(cmd);
	usart_pstr("\n");

}

void pwm_init() {
	
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
