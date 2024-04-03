/*
 * LCD_library.h
 *
 * Created: 07/03/2024 12:28:01
 *  Author: Yuich
 */ 


#ifndef LCD_LIBRARY_H_
#define LCD_LIBRARY_H_

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LCD_E 	3
#define LCD_RS	2

void strobe_lcd_e(void);
void init_lcd(void);
void display_text(char *str);
void write_data(unsigned char byte);
void write_command(unsigned char byte);

#endif /* LCD_LIBRARY_H_ */