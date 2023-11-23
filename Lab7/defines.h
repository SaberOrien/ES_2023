#define NODIGITS 4
#define DIGIT_1 1<<2
#define DIGIT_2 1<<3
#define DIGIT_3 1<<4
#define DIGIT_4 1<<5

#define DIG_1 2
#define DIG_2 3
#define DIG_3 4
#define DIG_4 5

#define SEGMENT_A 1<<0
#define SEGMENT_B 1<<1
#define SEGMENT_C 1<<2
#define SEGMENT_D 1<<3
#define SEGMENT_E 1<<4
#define SEGMENT_F 1<<5
#define SEGMENT_G 1<<6

#define SEG_A 0
#define SEG_B 1
#define SEG_C 2
#define SEG_D 3
#define SEG_E 4
#define SEG_F 5
#define SEG_G 6

#define JOYSTICK_OK 1<<15
#define JOY_OK 15
#define COUNT_DELAY 125

enum DigitPosition {LEFT, CENTRAL_LEFT, CENTRAL_RIGHT, RIGHT};

uint32_t digit[] = {DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4};

uint32_t numbers[] = {
		SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,
		SEGMENT_B | SEGMENT_C,
		SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,
		SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,
		SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,
		SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,
		SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,
		SEGMENT_A | SEGMENT_B | SEGMENT_C,
		SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,
		SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G
};

uint32_t display[NODIGITS];


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

#define PIOG_BASE_ADDRESS ((Init_Port)0x48001800)
#define PIOB_BASE_ADDRESS ((Init_Port)0x48000400)
#define PIOE_BASE_ADDRESS ((Init_Port)0x48001000)
#define PIOC_BASE_ADDRESS ((Init_Port)0x48000800)

#define TIM6_CR1 ((volatile uint16_t*) 0x40001000)
#define TIM6_SR	 ((volatile uint16_t*) 0x40001010)
#define TIM6_CNT ((volatile uint32_t*) 0x40001024)
#define TIM6_PSC ((volatile uint16_t*) 0x40001028)
#define TIM6_ARR ((volatile uint16_t*) 0x4000102C)

#define TIM6EN 1<<4
#define ARPE 1<<7
#define UG 1<<0
#define CEN 1<<0
#define UIF 1<<0

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

#define RCC_AHB2ENR ((volatile uint32_t*) 0x4002104C)
#define RCC_APB1ENR1 ((volatile uint32_t*) 0x40021058)
#define RCC_APB1ENR2 ((volatile uint32_t*) 0x4002105C)

void SystemClock_Config(void);
void InitSegments();
void InitDigit();
void InitJoystick();
void separateDigits(int number);
void displayCount();
int setCountDirection();

void InitTimer();
void MyDelay(int time);

void InitLpuart();
void transmitChar(char letter);
void transmitString(char* string);
char getChar();

void GPIO_SET_PIN(volatile uint16_t* port, volatile uint16_t pin);
void GPIO_RESET_PIN(volatile uint16_t* port, volatile uint16_t pin);
int GPIO_READ_PIN(volatile uint16_t* port, volatile uint16_t pin);


