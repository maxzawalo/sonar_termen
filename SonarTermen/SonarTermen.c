#include "config.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#define TMR_INIT 0
#define SOUND_PIN PC0
//Переключатель Мажор/Минор
#define GAMMA_PIN PC2

#define TRIG_PIN PC1
#define ECHO_PIN PC5

//Минимальное расстояние от сонара. Начало ноты До. Сантиметры.
#define MIN_SM 1
//Расстояние между нотами. Сантиметры.
#define SM_PER_NOTE 9

#define F4 349
#define A4S 466
#define F5 698
#define C5 523
#define D5 587
#define D5S 622
#define A5S 932
#define LOOPS_PAUSE 1000 //between loops, ms
#define NOTES_PAUSE 1 //between notes, ms
#define SEQU_SIZE 19 //notes quantity
#define TEMPO 108 //quarter in minute
#define WHOLE_NOTE_DUR 240000/TEMPO //ms
uint16_t notes[]={F4, F4, F4, A4S, F5, D5S, D5, C5, A5S, F5,
D5S, D5, C5, A5S, F5, D5S, D5, D5S, C5 };
//whole note = 255
uint16_t beats[]={21, 21, 21, 128, 128, 21, 21, 21, 128, 64,
21, 21, 21, 128, 64, 21, 21, 21, 128};
uint16_t note_duration[SEQU_SIZE];
uint32_t signal_period[SEQU_SIZE];
uint32_t elapsed_time;
uint32_t i;

//Мажорная гамма
uint16_t gamma_maj[]={262,294,330,349,392,440,494,523 };
//Минорная гамма
uint16_t gamma_min[]={262,294,311,349,392,415,466,523 };
volatile uint16_t sm;


VarDelay_us(uint32_t takt) {
	while(takt--) {
		_delay_us(1);
	}
}

//Играет Имперский Марш
//Код взят отсюда http://www.customelectronics.ru/avr-rabota-s-pezoizluchatelem-proigryivanie-melodii/
Play()
{
	PORTC &= ~1<<SOUND_PIN;
	//converting notes to signal period, us
	for (i = 0; i < SEQU_SIZE; i++) {
		signal_period[i] = (1000000 / notes[i]);
	}
	//converting beats to notes duration, ms
	for (i = 0; i < SEQU_SIZE; i++) {
		note_duration[i] = (WHOLE_NOTE_DUR*beats[i])/255;
	}
	
	{
		for (i = 0; i < SEQU_SIZE; i++) {
			elapsed_time = 0;
			while (elapsed_time < 1000*((uint32_t)note_duration[i])) {
				PORTC |= 1<<SOUND_PIN;
				VarDelay_us(signal_period[i] >> 1);
				PORTC &= ~(1<<SOUND_PIN);
				VarDelay_us(signal_period[i] >> 1);
				elapsed_time += signal_period[i];
			}
			_delay_ms(NOTES_PAUSE);
		}
		_delay_ms(LOOPS_PAUSE);
		
	}
}

int main(void){
	DDRC = 0xFF;							// Port C all output.
	DDRC &= ~(1<<ECHO_PIN);					// Set Pin ECHO_PIN as input to read Echo
	PORTC |= (1<<ECHO_PIN);					// pull up
	
	DDRC &= ~(1<<GAMMA_PIN);
	PORTC |= (1<<GAMMA_PIN);				// pull up
	
	PORTC &= ~(1<<TRIG_PIN);				// Init C4 as low (trigger)
	
	PRR &= ~(1<<PRTIM1);					// To activate timer1 module
	TCNT1 = TMR_INIT;						// Initial timer value
	TCCR1B |= (1<<CS11);					// Timer prescaller. If clock for atmega328p is 1Mhz period is 1uS
	TCCR1B |= (1<<ICES1);					// First capture on rising edge
	
	PCICR = (1<<PCIE1);						// Enable PCINT[14:8] we use pin C5 which is PCINT13
	PCMSK1 = (1<<PCINT13);					// PCINT13 Enable C5 interrupt
	
	sm = 0;
	sei();
	
	//BONUS!!! Если убрать комментарий ниже - играет Имперский Марш при включении игрушки
	//Play();
	
	while(1) {
		//Сразу "перегоняем" volatile переменную, используемую в обработке прерывания в локальную. Иначе хрюкает и хлюпает.
		uint16_t ind = sm;
		if(ind < MIN_SM)
		ind = 0;
		else
		//Подгоняем математику сонара под настройки МК и оптимизируем на вычисления без float. Так быстрее.
		ind =((ind/29 >> 2) - MIN_SM) / SM_PER_NOTE;
		if(ind > 0 && ind < 8+1)
		{
			ind--;
			uint16_t note = gamma_maj[ind];
			if (!bit_is_set(PINC, GAMMA_PIN))
			note = gamma_min[ind];
			
			uint32_t period = (1000000 / note) >> 1;
			//uint32_t period = (1000000 / (200 + sm << 2)) >> 1;
			for (i = 0; i < 40000/period; i++) {
				PORTC |= 1<<SOUND_PIN;
				VarDelay_us(period);
				PORTC &= ~(1<<SOUND_PIN);
				VarDelay_us(period);
			}
		}
		else
		{
			_delay_ms(60);
		}
		
		TCNT1 = TMR_INIT;
		PORTC |= (1<<TRIG_PIN); // Set trigger high
		_delay_us(10); // for 10uS
		PORTC &= ~(1<<TRIG_PIN); // to trigger the ultrasonic module
	}
}

/*******************************************INTURRUPT PCINT1 FOR PIN C5*******************************************/
ISR(PCINT1_vect) {
	//Помним, что функция обработки прерываний должна быть как можно легковеснее
	if (bit_is_set(PINC, ECHO_PIN)) {// Checks if echo is high
		TCNT1 = TMR_INIT; // Reset Timer
		} else {
		// Save Timer value
		sm = TCNT1;
		uint8_t oldSREG = SREG;
		cli(); // Disable Global interrupts
		SREG = oldSREG;// Enable interrupts
	}
}