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


typedef struct {
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint16_t IDR;
	volatile uint16_t reserved_1;
	volatile uint16_t ODR;
	volatile uint16_t reserved_2;
} *Init_Port;

#define PIOG_BASE_ADDRESS ((Init_Port)0x48001800)
#define PIOB_BASE_ADDRESS ((Init_Port)0x48000400)
#define PIOE_BASE_ADDRESS ((Init_Port)0x48001000)

void SystemClock_Config(void);
void InitSegments();
void InitDigit();
void InitJoystick();
void separateDigits(int number);
void displayCount();
int setCountDirection();

void GPIO_SET_PIN(volatile uint16_t* port, volatile uint16_t pin);
void GPIO_RESET_PIN(volatile uint16_t* port, volatile uint16_t pin);
int GPIO_READ_PIN(volatile uint16_t* port, volatile uint16_t pin);


