void Serial_IRQ(void)
{
	uint8_t	*aldl_data_buffer_addr;
	uint8_t	modem_status_reg;
	uint8_t	line_status_reg;
	uint8_t	temp_data;
	bool	aldl_received_bit;

	// Get pointer for current data buffer
	aldl_data_buffer_addr = aldl_data_buffer;
	
	// Get modem status register
	modem_status_reg = in(0x3FE);
	
	// Check if CTS line has changed state
	if((modem_status_reg & 0x01) != 0)
	{
		// Check if CTS line is low
		if((modem_status_reg & 0x10) == 0)
		{
			// CTS is low
			
			// Echo one byte as a reference (timing mark)
			// (~1875us = 1 byte @ 6400 baud, 8E2)
			temp_data = in(0x3F8);
			out(0x3F8,temp_data);
		}
		else
		{
			// CTS is high
			
			// Get line status register
			line_status_reg = in(0x3FD);
			
			// Check if new data is available
			if((line_status_reg & 0x01) != 0)
			{
				// New data is available
				
				// So that means, CTS changed state to high later than timing mark (~1875us)
				// Thus, received ALDL bit is HIGH
				
				// Clear line status register ? - it's probably a bug
				out(0x3FD, 0x00);
				
				// Increment number of received HIGH ALDL bits
				aldl_rx_bits_counter = aldl_rx_bits_counter + 1;
				
				// Check if number of HIGH bits is less than 9
				// 9 HIGH bits in a row mean than SYNC character was received
				aldl_received_bit = aldl_rx_bits_counter < 9;
				
				// That logic is pretty tricky, but keep in mind that aldl_received_bit would be there high too,
				// when code would be receiving DATA bytes too
				
				// Check if SYNC character was received - i.e. 9 high bits in a row
				if(aldl_received_bit == false)
				{
					// Yes, SYNC character was received
					aldl_rx_bits_counter = 0x80;
					
					// Start collecting data bytes
					aldl_byte_idx = 0;
					
					goto IRQ_end;
				}
			}
			else
			{
				// No new data is available
				
				// So that means, CTS changed state to high earlier than timing mark (~1875us)
				// Thus, received ALDL bit is LOW
				
				// Clear counter for SYNC character
				aldl_rx_bits_counter = 0;
				
				// Mark that received ALDL bit is LOW
				aldl_received_bit = false;
				
				// Check if SYNC character was received
				if((aldl_rx_bits_counter & 0x80) != 0)
				{
					// Yes, SYNC character was received, and the current bit is LOW,
					// so start collecting DATA byte
					
					// Set that 8 DATA bits would need to be received
					aldl_rx_bits_counter = 8;
					
					goto IRQ_end;
				}
			}
			
			// Collect incoming ALDL bit
			aldl_received_byte = aldl_received_byte << 1 | aldl_received_bit;
			
			// Decrement number of ALDL bits to collect
			aldl_rx_bits_counter = aldl_rx_bits_counter - 1;
			
			// Check if all bits of one DATA byte was collected
			if(aldl_aldl_rx_bits_counter == 0)
			{
				// Yes, reset bits counter to SYNC mark
				aldl_aldl_rx_bits_counter = 0x80;
				
				// Store collected DATA byte in buffer
				aldl_data_buffer[aldl_byte_idx] = aldl_received_byte;
				
				// Increment buffer index
				aldl_byte_idx = aldl_byte_idx + 1;
				
				// Check if at least 20 bytes was received - redundant check, it's probably a bug
				if(aldl_byte_idx > 0x13)
				{
					// Check if exactly 20 bytes was received (i.e. whole ALDL Ponton frame)
					if(aldl_byte_idx == 0x14)
					{
						// Set data ready flag, when earlier data was processed in main loop
						if(new_aldl_frame_received_flag == false)
						{
							new_aldl_frame_received_flag = true;
							aldl_data_buffer_ptr = aldl_data_buffer_addr;
						}
						
						// Switch data buffers
						if(aldl_data_buffer_ptr == (uint8_t *)0x5b4)
						{
							aldl_data_buffer = (uint8_t *)0x598;
						}
						else
						{
							aldl_data_buffer = (uint8_t *)0x5b4;
						}
					}
					else
					{
						// Wrong number of bytes collected, start over
						aldl_byte_idx = 0;
					}
				}
			}
		}
	}
	
	IRQ_end:
	// Acknowledge the interrupt with a signal to the interrupt controller (PIC)
	out(0x20,0x20);
	
	return;
}