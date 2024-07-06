#include <stdio.h>
#include <dos.h>
#include <conio.h>

#define BASE_ADDR		0x3F8

#define DLL_REG_OFFSET	0x00
#define DLM_REG_OFFSET	0x01

#define DATA_REG_OFFSET	0x00
#define IER_REG_OFFSET	0x01
#define FCR_REG_OFFSET	0x02
#define LCR_REG_OFFSET	0x03
#define MCR_REG_OFFSET	0x04
#define LSR_REG_OFFSET	0x05
#define MSR_REG_OFFSET	0x06

//MCR register
#define DTR_BIT			0x01
#define RTS_BIT			0x02

//MSR register
#define IN_BITS_MASK	0xF0
#define CTS_BIT			0x10
#define DSR_BIT			0x20
#define RI_BIT			0x40
#define CD_BIT			0x80

//LSR register
#define DATA_AVAIL_BIT	0x01

#define MIN_CHAR		'A'
#define MAX_CHAR		'Z'

#define LOOP_TIME			5	//ms

#define TX_CHAR_TIME		500	//ms
#define TX_CHAR_COUNT_MAX	(TX_CHAR_TIME / LOOP_TIME)

#define OUT_LINES_TIME		2000
#define OUT_LINES_COUNT_MAX	(OUT_LINES_TIME / LOOP_TIME)

unsigned char Check_lines_for_change(unsigned int base_addr)
{
	static unsigned char last_state = 0;
	unsigned char current_state, ret = 0;
	
	current_state = (inportb(base_addr+MSR_REG_OFFSET) & IN_BITS_MASK);
	
	if(last_state != current_state)
	{
		last_state = current_state;
		
		ret = 1;
	}
	
	return ret;	
}

void Get_lines_state(unsigned int base_addr, unsigned char *cts, unsigned char *dsr, unsigned char *ri, unsigned char *cd)
{
	*cts = (inportb(base_addr+MSR_REG_OFFSET) & CTS_BIT) ? 1 : 0;
	*dsr = (inportb(base_addr+MSR_REG_OFFSET) & DSR_BIT) ? 1 : 0;
	*ri = (inportb(base_addr+MSR_REG_OFFSET) & RI_BIT) ? 1 : 0;
	*cd = (inportb(base_addr+MSR_REG_OFFSET) & CD_BIT) ? 1 : 0;
	
	return;
}


void Set_lines_state(unsigned int base_addr, unsigned char dtr, unsigned char rts)
{
	unsigned char current_state;
	
	current_state = inportb(base_addr+MCR_REG_OFFSET);
	
	if(dtr)
		current_state |= DTR_BIT;
	else
		current_state &= ~(DTR_BIT);
	
	if(rts)
		current_state |= RTS_BIT;
	else
		current_state &= ~(RTS_BIT);
	
	outportb(base_addr+MCR_REG_OFFSET, current_state);

	return;
}

void Init_peripherial(unsigned int base_addr)
{
	outportb(base_addr+IER_REG_OFFSET, 0x00);	//Disable IRQs

	outportb(base_addr+FCR_REG_OFFSET, 0x00);	//Disable FIFO

	outportb(base_addr+LCR_REG_OFFSET, 0x80);	//Access DLAB

	outportb(base_addr+DLL_REG_OFFSET, 0x0C);	//9600 baud
	outportb(base_addr+DLM_REG_OFFSET, 0x00);

	outportb(base_addr+LCR_REG_OFFSET, 0x03);	//8N1, break signal disabled
	
	outportb(base_addr+MCR_REG_OFFSET, 0x00);	//RTS, DTR off
}

unsigned char Is_char_waiting(unsigned int base_addr)
{
	unsigned char ret = 0;

	ret = (inportb(base_addr+LSR_REG_OFFSET) & DATA_AVAIL_BIT) ? 1 : 0;

	return ret;
}

unsigned char Get_RX_char(unsigned int base_addr)
{
	return inportb(base_addr+DATA_REG_OFFSET);
}

void Send_TX_char(unsigned int base_addr, unsigned char character)
{
	outportb(base_addr+DATA_REG_OFFSET, character);

	return;
}

void main(void)
{
	unsigned char cts, dsr, ri, cd;

	unsigned int tx_char_counter = 0, out_lines_counter = 0;
	unsigned char out_lines_state = 0;
	unsigned char current_tx_char = MIN_CHAR;

	Init_peripherial(BASE_ADDR);

	printf("Welcome to Daewoo-FSO tester technology\r\n");
	printf("Made by trwgQ26xxx in 2024\r\n");
	printf("Testing on addr %X\r\n", BASE_ADDR);

	do
	{
		//Check input lines
		if(Check_lines_for_change(BASE_ADDR))
		{
			//Lines changed, so get status
			Get_lines_state(BASE_ADDR, &cts, &dsr, &ri, &cd);

			//Show status
			printf("CTS is %s, DSR is %s, RI is %s, CD is %s\r\n",
				cts ? " ON" : "OFF",
				dsr ? " ON" : "OFF",
				ri ? " ON" : "OFF",
				cd ? " ON" : "OFF");
		}

		//Check RX register
		if(Is_char_waiting(BASE_ADDR))
		{
			printf("Received char: %c\r\n", Get_RX_char(BASE_ADDR));
		}

		//Send character
		tx_char_counter++;
		if(tx_char_counter >= TX_CHAR_COUNT_MAX)
		{
			//printf("Send char!\r\n");
		
			Send_TX_char(BASE_ADDR, current_tx_char);

			current_tx_char++;
			if(current_tx_char > MAX_CHAR)
			{
				current_tx_char = MIN_CHAR;
			}

			tx_char_counter = 0;
		}

		//Manage out lines
		out_lines_counter++;
		if(out_lines_counter >= OUT_LINES_COUNT_MAX)
		{
			//printf("Lines changed state!\r\n");
		
			if(out_lines_state)
			{
				Set_lines_state(BASE_ADDR, 1, 0);

				out_lines_state = 0;
			}
			else
			{
				Set_lines_state(BASE_ADDR, 0, 1);

				out_lines_state = 1;
			}

			out_lines_counter = 0;
		}

		delay(LOOP_TIME);

	}while(!kbhit());

	printf("Done testing on addr %X\r\n", BASE_ADDR);

	return;
}
