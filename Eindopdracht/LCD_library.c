/*
 * LCD_library.c
 *
 * Created: 07/03/2024 12:27:36
 *  Author: Yuich
 */ 

#include "LCD_library.h"

void strobe_lcd_e(void) {
	PORTC |= (1<<LCD_E);	// E high
	_delay_ms(1);			// nodig
	PORTC &= ~(1<<LCD_E);  	// E low
	_delay_ms(1);			// nodig?
}

void init_lcd(void) {
	// PORTC output mode and all low (also E and RS pin)
	DDRC = 0xFF;
	PORTC = 0x00;

	// Step 2 (table 12)
	PORTC = 0x20;	// function set
	strobe_lcd_e();

	// Step 3 (table 12)
	PORTC = 0x20;   // function set
	strobe_lcd_e();
	PORTC = 0x80;
	strobe_lcd_e();

	// Step 4 (table 12)
	PORTC = 0x00;   // Display on/off control
	strobe_lcd_e();
	PORTC = 0xF0;
	strobe_lcd_e();

	// Step 4 (table 12)
	PORTC = 0x00;   // Entry mode set
	strobe_lcd_e();
	PORTC = 0x60;
	strobe_lcd_e();

}

void display_text(char *str) {
	clear_display();
	for(;*str; str++){
		write_data(*str);
	}
}

void reset_cursor(int position) {
	write_command (0x80 | position);
}

void clear_display() {
	write_command (0x01);
	_delay_ms(2);
	reset_cursor(0);
}

void write_data(unsigned char byte) {
	// First nibble.
	PORTC = byte;
	PORTC |= (1<<LCD_RS);
	strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC |= (1<<LCD_RS);
	strobe_lcd_e();
}

void write_command(unsigned char byte)
{
	// First nibble.
	PORTC = byte;
	PORTC &= ~(1<<LCD_RS);
	strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC &= ~(1<<LCD_RS);
	strobe_lcd_e();
}