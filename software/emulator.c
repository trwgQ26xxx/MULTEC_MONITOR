/*
	ALDL Emulator for Atmega328p by trwgQ26xxx.
	03.01.2026
	Emulates ALDL data transmission by sending predefined data frames.
*/

#include <avr/io.h>
#include <util/delay.h>

// Define the output pin for ALDL data
#define CONFIGURE_OUTPUT	do{ DDRD |= (1 << 7);	}while(0)
#define SET_LINE_TO_LOW		do{ PORTD |= (1 << 7);	}while(0)
#define SET_LINE_TO_HIGH	do{ PORTD &= ~(1 << 7);	}while(0)

// ALDL frame constants
#define ALDL_SYNC_BYTE		0xFF
#define NUM_OF_DATA_BYTES	20
#define SYNC_BYTE_FIRST_BIT	1
#define DATA_BYTE_FIRST_BIT	0

#define TOTAL_BIT_TIME		7800	//us
#define ONE_BIT_TIME		3000	//us
#define ZERO_BIT_TIME		500		//us

// Check if bit timing constants are valid
#if ONE_BIT_TIME >= TOTAL_BIT_TIME
	#error "ONE_BIT_TIME must be less than TOTAL_BIT_TIME"
#endif

#if ZERO_BIT_TIME >= TOTAL_BIT_TIME
	#error "ZERO_BIT_TIME must be less than TOTAL_BIT_TIME"
#endif

// ALDL data
#define DATA_LENGTH 2

const uint8_t data[DATA_LENGTH][NUM_OF_DATA_BYTES] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Function prototypes
void Send_ALDL_frame(const uint8_t *aldl_frame);
void Send_ALDL_byte(const uint8_t first_bit, const uint8_t data_byte);
void Send_ALDL_bit(const uint8_t bit);

int main(void)
{
	uint8_t i;

	// Configure output pin, idle state high
	CONFIGURE_OUTPUT;
	SET_LINE_TO_HIGH;

	// Send ALDL frames in an infinite loop
	while (1)
	{
		// Loop through new_slow_data and send each frame
		for(i = 0; i < DATA_LENGTH; i++)
		{
			Send_ALDL_frame(&data[i][0]);
		}
	}
}

void Send_ALDL_frame(const uint8_t *aldl_frame)
{
	uint8_t i;

	// Send SYNC byte
	Send_ALDL_byte(SYNC_BYTE_FIRST_BIT, ALDL_SYNC_BYTE);

	// Send DATA bytes
	for(i = 0; i < NUM_OF_DATA_BYTES; i++)
	{
		Send_ALDL_byte(DATA_BYTE_FIRST_BIT, aldl_frame[i]);
	}
}

void Send_ALDL_byte(const uint8_t first_bit, const uint8_t data_byte)
{
	int8_t i;

	// Send first bit (1 for SYNC, 0 for DATA)
	Send_ALDL_bit(first_bit);

	// Send 8 data bits, MSB first
	for(i = 7; i >= 0; i--)
	{
		Send_ALDL_bit((data_byte >> i) & 0x01);
	}
}

void Send_ALDL_bit(const uint8_t bit)
{
	// Check if bit is '1' or '0' and send corresponding signal
	if(bit)
	{
		SET_LINE_TO_LOW;
		_delay_us(ONE_BIT_TIME);
		SET_LINE_TO_HIGH;
		_delay_us(TOTAL_BIT_TIME - ONE_BIT_TIME);
	}
	else
	{
		SET_LINE_TO_LOW;
		_delay_us(ZERO_BIT_TIME);
		SET_LINE_TO_HIGH;
		_delay_us(TOTAL_BIT_TIME - ZERO_BIT_TIME);
	}
}
