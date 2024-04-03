#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>




#define BIT(x)(1<<(x))

#define BOARD_HEIGHT	8
#define BOARD_WIDTH		4

#define LEFT	0b0000000
#define UP		0b00010100
#define DOWN	0b00000111
#define RIGHT	0b00100011
#define SELECT	0b01000001
#define NOTHING	0b01101001

typedef struct {
	int x;
	int y;
} Point;

int msCount = 0;
int buttonInput = RIGHT;
int lastDirection = RIGHT;
int gameSpeed = 128;

int readADC();
int update();
void initGame();

Point snake[BOARD_WIDTH * BOARD_HEIGHT];
Point fruit;
int snakeLength;
bool running;

void wait( int ms ) {
	for (int tms=0; tms<ms; tms++) {
		_delay_ms( 1 );
	}
}

ISR( TIMER2_COMP_vect )
{
	msCount++; // Increment ms counter
	ADMUX &= 0b11100000; // Verwijder output
	
	if ( msCount == 482 ) // Potentiometer input uitlezen
	{
		ADMUX |= 0b00000000; // Zet op port 0
		
		gameSpeed = readADC();
		
		msCount = 0; // Reset ms_count value
	}
	
	if ( (msCount % 25) == 0) { // Buttons input uitlezen
		ADMUX |= 0b00000001; // Zet op port 1
		
		int input = readADC();
		if (input != NOTHING)
			buttonInput = input;
			
		if (input == SELECT && running == false) {
			initGame();
		}
	} 
}

void inputTimerInit( void )
{
	OCR2 = 250; // Compare value of counter 2
	TCCR2 = 0b00001011; // Initialize T2: timer, prescaler = 64, compare output disconnected, CTC, RUN
	TIMSK |= (1<<7); // T2 compare match interrupt enable
}

void adcInit( void )
{
	ADMUX = 0;
	ADMUX |= 0b01000000; // Selecteer voltage selection AVCC with external capacitor at AREF pin
	ADMUX |= 0b00100000; // Stel ADLAR (ADC left adjust result) in op 0 dus right adjusted
	ADMUX |= 0b00000001; // Selecteer kanaal 1 (ADC1)
	
	ADCSRA = 0;
	ADCSRA |= 0b10000000; // Schakel de ADC-module in
	//ADCSRA |= 0b00100000; // Activeer freerun-modus
	ADCSRA |= 0b00000110; // Stel de prescaler-bits in op een prescaler van 64
}

void init() {
	DDRA = 0xFF; // Port A, B, C en D op output zetten
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
	
	DDRE = 0xFF;
	
	DDRF = 0x00; // PORT F voor input van ADC
	
	adcInit();
	
	inputTimerInit();
	sei();
}

int readADC() {
	ADCSRA |= BIT(6); // Start ADC conversie
	
	while (ADCSRA & (1 << ADSC));
	
	return ADCH;
}


int aaa = 0;

int main(void)
{		
	init();
	
	initGame();
	
	int delay;
	while (true) {
		while (running)
		{
			update();
			drawBoard();
			aaa++;
			
			delay = 150 + gameSpeed * 5;
			wait(delay);
		}
		
		delay = 150 + gameSpeed * 5;
			
		PORTA ^= 0xFF;
		PORTB ^= 0xFF;
		PORTC ^= 0xFF;
		PORTD ^= 0xFF;
			
		wait(delay);
	}
}

void initGame() {	
	Point start;
	start.x = (rand() % BOARD_WIDTH);
	start.y = (rand() % BOARD_HEIGHT);
	
	start.x = 2;
	start.y = 2;
	
	snake[0] = start;
	snakeLength = 1;
	
	newFruit();
	
	lastDirection = RIGHT;
	buttonInput = RIGHT;
	
	running = true;
}

void newFruit() {
	// TODO check of het valide is
	fruit.x = (rand() % BOARD_WIDTH);
	fruit.y = (rand() % BOARD_HEIGHT);
}

int update() {
	int direction;
	if ((buttonInput == UP) || (buttonInput == RIGHT) || (buttonInput == DOWN) || (buttonInput == LEFT))
		direction = buttonInput; // Volg de richting van de knoppen
	else
		direction = lastDirection; // Geen input van de knoppen
		
	Point lookup[4];
		
	lookup[0].x = 0; // Up
	lookup[0].y = 1;
	lookup[1].x = 1; // Right
	lookup[1].y = 0;
	lookup[2].x = 0; // Down
	lookup[2].y = -1;
	lookup[3].x = -1; // Left
	lookup[3].y = 0;
	
	int offset;
	switch (direction) {
		case UP:
			offset = 0;
			break;
		case RIGHT:
			offset = 1;
			break;
		case DOWN:
			offset = 2;
			break;
		case LEFT:
			offset = 3;
			break;
	}
	
	lastDirection = direction;
	
	moveSnake(lookup[offset]);
}

int isValidPosition(Point point) {
	if (point.y >= BOARD_HEIGHT || point.y < 0)
		return 0;
	
	for (int i = 1; i <= snakeLength; i++) {
		Point snakePart = snake[i];
		
		if ((snakePart.x == point.x) && (snakePart.y == point.y))
			return 0;
	}
	
	return 1;
}

void moveSnake(Point change) {
	Point head = snake[0];
	
	if ((head.x == fruit.x) && (head.y == fruit.y))	{
		snakeLength++;
		
		newFruit();
	}
	
	for (int i = (BOARD_WIDTH * BOARD_HEIGHT) - 1; i > 0; i--) {
		snake[i] = snake[i - 1];
	}
	
	Point newHead;
	newHead.x = (head.x + change.x);
	newHead.y = (head.y + change.y);
	
	if (!isValidPosition(newHead))		{
		running = false;
		return;
	}
	
	newHead.x = (newHead.x + BOARD_WIDTH) % BOARD_WIDTH;
	newHead.y = (newHead.y + BOARD_HEIGHT) % BOARD_HEIGHT;
	
	if (isValidPosition(newHead)) {
		snake[0] = newHead;
	} else {
		running = false;
	}
}

void drawBoard() {
	// Clear alle leds.
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	
	drawPixel(fruit);
	
	for (int i = 0; i <= snakeLength; i++) {
		drawPixel(snake[i]);
	}
	
	PORTE = aaa;
}

void drawPixel(Point point) {
	int y = (1 << point.y);
	switch (point.x) {
		case 0:
			PORTA |= y;
			break;
		case 1:
			PORTB |= y;
			break;
		case 2:
			PORTC |= y;
			break;
		case 3:
			PORTD |= y;
			break;
		default:
			break;
	}
}