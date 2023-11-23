#include "startup.h"

#ifndef __PERIPHERALS
#define __PERIPHERALS

void initPeripherals();
void SystemClock_Config(void);
void Error_Handler(void);


#define BIT(x) 1<<x

//-------------PORT-------------

typedef volatile struct {
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint16_t IDR;
	uint16_t reserved_1;
	uint16_t ODR;
	uint16_t reserved_2;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
	uint32_t BRR;
	uint32_t ASCR;
} *Init_Port;

#define PIOB_BASE_ADDRESS ((Init_Port)0x48000400)
#define PIOC_BASE_ADDRESS ((Init_Port)0x48000800)
#define PIOD_BASE_ADDRESS ((Init_Port)0x48000C00)
#define PIOE_BASE_ADDRESS ((Init_Port)0x48001000)
#define PIOG_BASE_ADDRESS ((Init_Port)0x48001800)

void InitGPIO();
void GPIO_RESET_PIN(volatile uint16_t* port, uint16_t pin);
void GPIO_SET_PIN(volatile uint16_t* port, uint16_t pin);
int GPIO_READ_PIN(volatile uint16_t* port, uint16_t pin);
void WritePin(volatile uint16_t* port, uint16_t pin, int state);

typedef struct {
	volatile uint16_t* port;
	uint16_t pin;
} pio_pin;

extern pio_pin leds[];
extern pio_pin rgb[];
extern pio_pin joystick[];

//-------------DISPLAY-------------
#define NODIGITS 4

#define DIG_1 2
#define DIG_2 3
#define DIG_3 4
#define DIG_4 5

#define SEG_A 0
#define SEG_B 1
#define SEG_C 2
#define SEG_D 3
#define SEG_E 4
#define SEG_F 5
#define SEG_G 6

extern uint32_t digit[];

extern uint32_t numbers[];

extern uint32_t display[NODIGITS];

void InitSegments();
void InitDigit();
void separateDigits(int number);
void displayCount();

extern void (*doAsyncWork)(void);
void doWhileWaiting();

//-------------JOY_OK-------------
#define JOY_RT 0
#define JOY_LT 1
#define JOY_DN 2
#define JOY_UP 3
#define JOY_OK 15
void InitJoystick();

//-------------LPUART-------------

#define RCC_AHB2ENR ((volatile uint32_t*) 0x4002104C)
#define RCC_APB1ENR1 ((volatile uint32_t*) 0x40021058)
#define RCC_APB1ENR2 ((volatile uint32_t*) 0x4002105C)

typedef volatile struct {
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t BRR;
	uint32_t reserved[2];
	uint32_t RQR;
	uint32_t ISR;
	uint32_t ICR;
	uint32_t RDR;
	uint32_t TDR;
} *LPUART;

#define PIOC_EN 1<<2
#define LPUART1_EN 1<<0
#define LPUART_1 ((LPUART) 0x40008000)

//CR1
#define UE 1<<0
#define RE 1<<2
#define TE 1<<3
#define PCE 1<<10  // Parity Control Enable
#define M0 (1<<12 | 1<<13)
#define M1 (1<<28 | 1<<29)

//CR2
#define STOP (1<<12 | 1<<13)

//CR3
#define DMAR 1<<6
#define DMAT 1<<7

//ISR
#define RXNE 1<<5
#define TC 1<<6

void InitLpuart();
void transmitChar(char letter);
void transmitString(char* string);
char getChar();

#endif
