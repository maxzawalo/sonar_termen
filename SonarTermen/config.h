#ifndef config_h
#define config_h

#define F_CPU 16000000UL //16MHz
//#define F_CPU 1000000UL 

//#define VIRTUAL_SENSOR

#ifdef __CODEVISIONAVR__ 
	#define _CHIP_ATMEGA328P_
#endif

#if defined _CHIP_ATMEGA16A_
#include <mega16a.h>
#elif defined _CHIP_ATMEGA162_ | defined _CHIP_ATMEGA162L_ | defined _CHIP_ATMEGA162U_ | defined _CHIP_ATMEGA162V_
#include <mega162.h>
#include <mega162_bits.h>
#elif defined _CHIP_ATMEGA88_ | defined _CHIP_ATMEGA88V_
#include <mega88.h>
#elif defined _CHIP_ATMEGA328P_
#include <mega328p.h>
#endif

//#ifdef __CODEVISIONAVR__  
//#define F_CPU  _MCU_CLOCK_FREQUENCY_//11059200UL//_MCU_CLOCK_FREQUENCY_//
//#else
//#define F_CPU 16000000UL
//#endif

#define NO_LCD     


//USART
#define BAUD 9600     //57600
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)


//types
#ifdef __CODEVISIONAVR__ 
#define uint8_t unsigned char
#else
#include <avr/io.h>
#endif


//#define _BV	(bit) (1 << (bit))
//#define  SETBIT_0 (PORT, bit)  PORT &=~bit
//#define  SETBIT_1 (PORT, bit)  PORT |= (1<<bit)

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))

//=== 1 wire ===
//количество устройств на шине 1Wire
#define MAX_OWI_DEVICES  10
//#define IS_PARASITE_1WIRE 1
 ////Настраиваем конфигурационный файл OWIPolled.h 
 ////-  задаем программную реализацию 1-Wire протокола
//#define OWI_SOFTWARE_DRIVER    
 //// -  устанавливаем тактовую частоту микроконтроллера
//#define     CPU_FREQUENCY   F_CPU/1000000
 //// - задаем порт, к которому подключена 1-Wire шина
//#define     OWI_PORT        PORTD //!< 1-Wire PORT Data register
//#define     OWI_PIN         PIND    //!< 1-Wire Input pin register
//#define     OWI_DDR         DDRD    //!< 1-Wire Data direction register                               
////задаем вывод, к которому подключена OneWire шина
//#define OWI_BUS OWI_PIN_7
//Lock pin

#define OW_LOCK_PIN 6//PORTB.6

//#define SET_OWI_POWER_PIN() {OWI_DDR |=(OWI_LOCK_PIN);}

#define OW_LOCK_DELAY  1000 //700 мс минимум //F_CPU*0.00001

#define set_bit(reg,bit) reg |= (1<<bit)
#define clear_bit(reg,bit) reg &= ~(1<<bit)


#endif