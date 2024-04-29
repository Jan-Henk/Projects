#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SPI.h>

// D0 (Seeeduino Xiao) connected to switch							-> D0 configured as input.
// D1 (Seeeduino Xiao) connected to D4 (CH376S, BZ)					-> D1 configured as input.
// D2 (Seeeduino Xiao) connected to INT (CH376S, INT)				-> D2 configured as input.
// D3 (Seeeduino Xiao) connected to COMMAND (pin 7 SIO ATARI 8-bit)	-> D3 configured as input.
// D4 (Seeeduino Xiao) connected to D3 (CH376S, SCS) 				-> D4 configured as output.
// D5 (Seeeduino Xiao) connected to LED								-> D5 configured as output.
// D6 (Seeeduino Xiao) connected to DATA IN (pin 3 SIO ATARI 8-bit)	-> Hardware UART TX.
// D7 (Seeeduino Xiao) connected to DATA OUT (pin 5 SIO ATARI 8-bit)-> Hardware UART RX.
// D8 (Seeeduino Xiao) connected to D5 (CH376S, SCK)				-> Hardware SPI.
// D9 (Seeeduino Xiao) connected to D7 (CH376S, SDO)				-> Hardware SPI.
// D10(Seeeduino Xiao) connected to D6 (CH376S, SDI)				-> Hardware SPI.

#define SWITCH_pin	0	// Seeeduino Xiao has pin D0 connected to pin PA02 of SAMD21D.
#define BZ_pin		1	// Seeeduino Xiao has pin D1 connected to pin PA04 of SAMD21D.
#define INT_pin 	2	// Seeeduino Xiao has pin D2 connected to pin PA10 of SAMD21D.
#define CMD_pin 	3	// Seeeduino Xiao has pin D3 connected to pin PA11 of SAMD21D.
#define SCS_pin		4	// Seeeduino Xiao has pin D4 connected to pin PA08 of SAMD21D.
#define LED_pin 	5	// Seeeduino Xiao has pin D5 connected to pin PA09 of SAMD21D.

// These are the addresses of the registers of the IOBUS that control the I/O pins.
// We don't really need the maximum speed of the MCU via the IOBUS, but hey, it's there, so let's use it anyway.
// The base address is 0x60000000 with an offset of 0x00 for port A.

// We need to define the registers of port A to control 6 pins (SWITCH_pin, BZ_pin, INT_pin, CMD_pin, SCS_pin, LED_pin).
#define DIR0			0x60000000
#define DIRSET0 		0x60000008
#define DIRCLR0 		0x60000004
#define OUTSET0 		0x60000018
#define OUTCLR0 		0x60000014
#define WRCONFIG0 		0x60000028
#define CTRL0 			0x60000024
#define IN0 			0x60000020
#define PINCFG0			0x60000040

// We need to define input/output
#define InPut			0
#define OutPut			1

// Define a structure for pin configuration
typedef struct {
	uint32_t *dir;
	uint32_t *dirset;
	uint32_t *dirclr;
	uint32_t *ctrl;
	uint32_t *wrconfig;
	uint32_t *in;
	uint8_t *pincfg;
	uint8_t Pxy;					//This variable represents the padnumber of the MCU, not the pinnumber of the breakout board.
} Pin_Config;

typedef struct {
   uint32_t *outclr;
   uint32_t *outset;
   uint8_t Pxy;
} Pin_State;

// Define pins with their properties using an array of structures
Pin_Config Pin_Settings[] = {
	{ (uint32_t *)DIR0, ( uint32_t *)DIRSET0, ( uint32_t *)DIRCLR0, ( uint32_t *)CTRL0, ( uint32_t *)WRCONFIG0, ( uint32_t *)IN0, (uint8_t *)PINCFG0, 2},		// Pin 0: Port A
	{ (uint32_t *)DIR0, ( uint32_t *)DIRSET0, ( uint32_t *)DIRCLR0, ( uint32_t *)CTRL0, ( uint32_t *)WRCONFIG0, ( uint32_t *)IN0, (uint8_t *)PINCFG0, 4},		// Pin 1: Port A
	{ (uint32_t *)DIR0, ( uint32_t *)DIRSET0, ( uint32_t *)DIRCLR0, ( uint32_t *)CTRL0, ( uint32_t *)WRCONFIG0, ( uint32_t *)IN0, (uint8_t *)PINCFG0, 10},		// Pin 2: Port A
	{ (uint32_t *)DIR0, ( uint32_t *)DIRSET0, ( uint32_t *)DIRCLR0, ( uint32_t *)CTRL0, ( uint32_t *)WRCONFIG0, ( uint32_t *)IN0, (uint8_t *)PINCFG0, 11},		// Pin 3: Port A
	{ (uint32_t *)DIR0, ( uint32_t *)DIRSET0, ( uint32_t *)DIRCLR0, ( uint32_t *)CTRL0, ( uint32_t *)WRCONFIG0, ( uint32_t *)IN0, (uint8_t *)PINCFG0, 8},		// Pin 4: Port A
	{ (uint32_t *)DIR0, ( uint32_t *)DIRSET0, ( uint32_t *)DIRCLR0, ( uint32_t *)CTRL0, ( uint32_t *)WRCONFIG0, ( uint32_t *)IN0, (uint8_t *)PINCFG0, 9},		// Pin 5: Port A
};

Pin_State Pin_Manipulation[] = {
  { ( uint32_t *)OUTCLR0, ( uint32_t *)OUTSET0, 2},    // Pin 0: Port A
  { ( uint32_t *)OUTCLR0, ( uint32_t *)OUTSET0, 4},    // Pin 1: Port A
  { ( uint32_t *)OUTCLR0, ( uint32_t *)OUTSET0, 10},   // Pin 2: Port A
  { ( uint32_t *)OUTCLR0, ( uint32_t *)OUTSET0, 11},   // Pin 3: Port A
  { ( uint32_t *)OUTCLR0, ( uint32_t *)OUTSET0, 8},    // Pin 4: Port A
  { ( uint32_t *)OUTCLR0, ( uint32_t *)OUTSET0, 9},    // Pin 5: Port A
};

//*************************************************************************************************************
//	Configure the I/O pin (Seeeduino XIAO).
//*************************************************************************************************************
void Set_PinMode(uint32_t pinIndex, bool direction) {
	Pin_Config *settings = &Pin_Settings[pinIndex];
	
	if (!direction) {
		*settings->dir |= (0 << (1 << settings->Pxy));
		*settings->ctrl |= (1 << settings->Pxy);
		*(settings->pincfg + settings->Pxy) |= ((1 << 2) | (1 << 1) | (1 << 6));
		//pull_pin_high(pinIndex);					This will activate the pullup resistor of the pin. Don't use it because the inputs will always be pulled either low or high by the external MCU. The input will never be read while floating.
	}
	else if (direction) {
		*settings->dir |= (1 << settings->Pxy);
	}
}
	
//*************************************************************************************************************
//	Read the I/O pin (CH376S).
//*************************************************************************************************************
bool read_pin(uint32_t pinIndex) {
	volatile Pin_Config *settings = &Pin_Settings[pinIndex];
	
	return ((bool)(*settings->in & (1 << settings->Pxy)));											// Cast the state to Boolean and return it.
}

//*************************************************************************************************************
//	Pull the I/O pin low (CH376S).
//*************************************************************************************************************
void pull_pin_low(uint32_t pinIndex) {
	Pin_State *manipulate = &Pin_Manipulation[pinIndex];

	*manipulate->outclr = (1 << manipulate->Pxy);												// Pull the I/O pin low.								
}

//*************************************************************************************************************
//	Pull the I/O pin high (CH376S).
//*************************************************************************************************************
void pull_pin_high(uint32_t pinIndex) {
	Pin_State *manipulate = &Pin_Manipulation[pinIndex];
	
	*manipulate->outset = (1 << manipulate->Pxy);												// Pull the I/O pin high.								
}

//************************************************************************************************************
//	SPI function. Sends and receives data to and from the IO port of the CH376S (CH376S).
//************************************************************************************************************
void SPI_transfer_data(uint8_t SPI_data_length, uint8_t *SPI_data) {
	
	pull_pin_low(SCS_pin);
	SPI.transfer(SPI_data, SPI_data_length);
	pull_pin_high(SCS_pin);
	
	while (read_pin(BZ_pin)) {	 						// Wait for the release of the buffer of the CH376S.
	}
}

//************************************************************************************************************
//	Interrupt handling (#INT pin CH376S) (CH376S).
//************************************************************************************************************
uint8_t interrupt_handling_CH376S(void) {
	uint8_t SPI_data_length = 2;
	uint8_t SPI_data[] = {0x22, 0x00};
	uint8_t status;

	while (read_pin(INT_pin)) { 									// Wait for the interrupt line (#INT) to go low.
	}
	SPI_transfer_data(SPI_data_length, SPI_data); 										// Reset the interrupt that was initiated by a previous command.
	status = SPI_data[1];

	switch (status) {
	case 0x14: {																			// Transfer succeeded.
	}
	return (0x14);

	case 0x15: {																			// Attachment of USB device detected.
	}
	return (0x15);

	case 0x16: {				  															// Removal of USB device detected.
	}
	return (0x16);

	case 0x17: {																			// Data error or buffer overflow.
	}
	return (0x17);

	case 0x18: { 																			// USB device initialized.  
	}
	return (0x18);

	case 0x1D: { 																			// There are more bytes left to read.  
	}
	return (0x1D);

	case 0x1E: { 																			// There are more bytes left to write.  
	}
	return (0x1E);

	case 0x1F: { 																			// Failure of USB device.  
	}
	return (0x1F);

	case 0x41: { 																			// Can't open directory.  
	}
	return (0x41);

	case 0x42: { 																			// File not found.  
	}
	return (0x42);

	case 0x43: { 																			// Filename error.  
	}
	return (0x43);

	case 0x82: { 																			// Disk connection failure.  
	}
	return (0x82);

	case 0x84: { 																			// Sector size too large.  
	}
	return (0x84);

	case 0x92: { 																			// Wrong type disk partition.  
	}
	return (0x92);

	case 0xA1: { 																			// Format error.  
	}
	return (0xA1);

	case 0xB1: { 																			// Disk is full.  
	}
	return (0xB1);

	case 0xB2: { 																			// Too many files in directory.  
	}
	return (0xB2);

	case 0xB4: { 																			// File is closed.  
	}
	return (0xB4);
}
	return(1);																				// Return 1 because the CH376S has sent an unknown status.
}

//************************************************************************************************************
//	Check for presence of CH376S (CH376S).
//************************************************************************************************************
void check_CH376S(void) {
	uint8_t complementary_byte;
	uint8_t SPI_data_length = 3;
	uint8_t SPI_data[] = {0x06, 0xEA, 0x00};
	
	SPI_transfer_data(SPI_data_length, SPI_data);											// Send a byte and expect to receive the complementary of that byte (e.g. 0xFF - 0x57 = 0xA8).
	complementary_byte = SPI_data[2];
	if (complementary_byte != (0xFF - 0xEA)) {												// Think of putting useful code here. 
	}
}

//*************************************************************************************************************
//	Check for presence of USB bulk device (CH376S).
//*************************************************************************************************************
uint8_t check_USB_bulk_device(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x30};
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// Check if a USB bulk device is present.
	status = interrupt_handling_CH376S();
	return(status);
}

//*************************************************************************************************************
//	Reset CH376S (CH376S).
//*************************************************************************************************************
void reset_CH376S(void) {
  	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x05};
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// Hardware reset of the CH376S.
	delay(35); 																				// Within 35 milliseconds the CH376S will perform a hardware reset.
}

//*************************************************************************************************************
//	Reset USB bus (CH376S).    
//*************************************************************************************************************
void reset_USB_bus(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x07};
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// USB bus reset of the CH376S.
	delayMicroseconds(10); 																	// Within 10 microseconds the CH376S will perform a USB bus reset.
}

//*************************************************************************************************************
//	Set USB mode (CH376S).
//*************************************************************************************************************
uint8_t set_USB_mode(uint8_t usb_mode) {
  	uint8_t SPI_data_length = 2;
	uint8_t SPI_data[] = {0x15, usb_mode};
	uint8_t status;

	SPI_transfer_data(SPI_data_length, SPI_data); 											// Set the USB mode of the CH376S.
	status = SPI_data[1]; 																	// Get the status of the USB mode setting.
	return (status);
}

//*************************************************************************************************************
//	Mount disk (CH376S).  
//*************************************************************************************************************
uint8_t mount_disk_CH376S(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x31};
	uint8_t status;

	SPI_transfer_data(SPI_data_length, SPI_data); 											// Mount the attached bulk device.
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Set filename (CH376S).
//**************************************************************************************************************
void set_filename_CH376S(const char fname[15]) {
	uint8_t SPI_data_length = 16;
	uint8_t SPI_data[16] = {0x2F};
	
	for (uint8_t i = 0; i < 15; i++) {
        SPI_data[i + 1] = fname[i]; 														// Copy the filename characters to SPI_data
    }
	SPI_transfer_data(SPI_data_length, SPI_data); 											// Send 15 characters in total with the main filename having 8 characters and the extention having 3 characters. End the filename with \0.
}

//*************************************************************************************************************
//	Open file (CH376S).
//**************************************************************************************************************
uint8_t open_file_CH376S(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x32};
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// Open the file.
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Close file (CH376S).
//*************************************************************************************************************
uint8_t close_file_CH376S(uint8_t update_filelength) {
	uint8_t SPI_data_length = 2;
	uint8_t SPI_data[] = {0x36, update_filelength};											// Update_filelength: 0x00 -> no update of filelength, 0x01 -> update of filelength.
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data);
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Continue enumerate file (CH376S).
//*************************************************************************************************************
uint8_t proceed_enumerate_CH376S(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x33};
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// This command tells the CH376S to proceed to read the file.
	status = interrupt_handling_CH376S();
	return (status);
}	

//*************************************************************************************************************
//	Search file (use only with * e.g. "USB*") (CH376S).
//*************************************************************************************************************
uint8_t search_file_CH376S(const char fname[13], uint8_t *usb_data) {
	uint8_t status;
	const char *char_buf = NULL;	
	
	char_buf = strchr(fname, '*');
	if (char_buf != NULL) {
		set_filename_CH376S(fname);
		status = open_file_CH376S();
		if (status == 0x1D) {
			read_data_CH376S(usb_data);
			return (status);
		} else if (status == 0x42) {
			usb_data = NULL;
			return (status);
		}
	} else {
		usb_data = NULL;
		return (1);
	}
}

//*************************************************************************************************************
//	Get filesize (CH376S).
//*************************************************************************************************************
uint32_t get_filesize_CH376S(void) {
	uint8_t filesize_byte[4];
	uint8_t SPI_data_length = 6;
	uint8_t SPI_data[6] = {0x0C, 0x68};													// 0x68 is the first byte to send after the command 0x0C. It's part of the command (see the datasheet of the CH376S).

	SPI_transfer_data(SPI_data_length, SPI_data);											
	for (uint8_t i = 0; i < 4; i++) {
    filesize_byte[i] = SPI_data[i + 2]; 													// Receive the 4 bytes of data that, together, make the filelength.
	}
	return ( *(uint32_t *) filesize_byte); 												// Cast the array filelength_byte[] to uint32_t. The result is a 32 bit value of the filesize that we wanted. Return the pointer to it.
}

//*************************************************************************************************************
//	Move the byte index in the file to the requested location (CH376S).
//*************************************************************************************************************
uint8_t locate_sector_CH376S(uint8_t atari_sector_lo, uint8_t atari_sector_hi) {
	uint8_t SPI_data_length = 5;
	uint8_t SPI_data[5] = {0x39};
	uint8_t status;
	uint32_t total_bytes;
	uint32_t total_sectors;
  
	total_sectors =  (((uint32_t)atari_sector_hi << 8) | (uint32_t)atari_sector_lo) - 1;	// The data in every file begins at byte index zero. We have to subtract 1 sector because we start with an offset of 1 sector.
	total_bytes = total_sectors << 7;															// The total number of bytes is the total number of sectors multiplied by 128 bytes in every sector.
	SPI_data[1] = total_bytes & 0xFF;															// Isolate the LSB byte and send it to the ATARI.
	SPI_data[2] = (total_bytes >> 8) & 0xFF;													// Isolate the second byte after the LSB byte and send it.
	SPI_data[3] = (total_bytes >> 16) & 0xFF;	                								// Isolate the third byte after the LSB byte and send it.
	SPI_data[4] = (total_bytes >> 24) & 0xFF;													// Isolate the MSB byte and send it.
	SPI_transfer_data(SPI_data_length, SPI_data);
	status = interrupt_handling_CH376S();													// Problems detected after execution of this code will have to be reported by returning the status.
	return (status);
}

//*************************************************************************************************************
//	Initialize read data (CH376S).
//*************************************************************************************************************
uint8_t init_read_CH376S(uint8_t number_of_bytes) {
	uint8_t SPI_data_length = 3;
	uint8_t SPI_data[] = {0x3A, number_of_bytes, 0x00};
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// Initialize read data from file.
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Initialize write data (CH376S).
//*************************************************************************************************************
uint8_t init_write_CH376S(uint8_t number_of_bytes) {
	uint8_t SPI_data_length = 3;
	uint8_t SPI_data[] = {0x3C, number_of_bytes, 0x00};
	uint8_t status;
  
	SPI_transfer_data(SPI_data_length, SPI_data); 											// Initialize write data to file.
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Continue read file (CH376S). 
//*************************************************************************************************************
uint8_t proceed_read_CH376S(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x3B};
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// This command tells the CH376S to proceed to read the file.
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Continue write file (CH376S).
//*************************************************************************************************************
uint8_t proceed_write_CH376S(void) {
	uint8_t SPI_data_length = 1;
	uint8_t SPI_data[] = {0x3D};
	uint8_t status;
	
	SPI_transfer_data(SPI_data_length, SPI_data); 											// This command tells the CH376S to proceed to write to the file.
	status = interrupt_handling_CH376S();
	return (status);
}

//*************************************************************************************************************
//	Read data from file (CH376S). 
//*************************************************************************************************************
void read_data_CH376S(uint8_t *usb_data) {
	uint8_t SPI_data_length = 2;
	uint8_t SPI_data[] = {0x27, 0x00};	
	
	SPI_transfer_data(SPI_data_length, SPI_data);											// First we send the command that tells the CH376S we want to read all the data there is to read (max. one Atari sector size).
    SPI_data_length = SPI_data[1]; 															// We might run into problems when we reach the end of the file and we still have to read more bytes of a sector ( Most unlikely because the Atari was designed to read complete sectors, therefore file sizes were always a multitude of 128 bytes).
	SPI_transfer_data(SPI_data_length, usb_data);											// So, let the CH376S tell us how much data there is left to read.
}

//*************************************************************************************************************
//	Write data to file (CH376S).
//*************************************************************************************************************
void write_data_CH376S(uint8_t *usb_data) {
	uint8_t SPI_data_length = 2;
	uint8_t SPI_data[] = {0x2D, 0x00};

	SPI_transfer_data(SPI_data_length, SPI_data);											// First we send the command that tells the CH376S we want to write all the data there is to write (max. one Atari sector size).
    SPI_data_length = SPI_data[1]; 															// We might run into problems when we reach the end of the file and we still have to write more bytes of a sector ( Most unlikely because the Atari was designed to write complete sectors, therefore file sizes were always a multitude of 128 bytes).
	SPI_transfer_data(SPI_data_length, usb_data);											// So, let the CH376S tell us how much data there is left to write.
}

//*************************************************************************************************************
//	Read sector from file (CH376S).
//*************************************************************************************************************
uint8_t read_sector_CH376S(uint8_t sector_size, uint8_t *usb_data, uint8_t atari_sector_lo, uint8_t atari_sector_hi) {
  uint8_t status;

  status = locate_sector_CH376S(atari_sector_lo, atari_sector_hi);						// Let's put the byte index in the file on the right spot.
  if (status != 0x14) {
    usb_data = NULL;
    return (status);
  }

  status = init_read_CH376S(sector_size);													// Give the CH376S the command to prepare itself for reading data.
  if (status == 0x1D) {																	// If the status is 0x1D then there is still data left to read.
    read_data_CH376S(usb_data);																// Read the actual data.
    status = proceed_read_CH376S();														// Tell the CH376S that we want to read more data.
    if (status != 0x14) {																	// If the status is not 0x14 then something went wrong. If it is 0x14 then continue.
      usb_data = NULL;																		// Discard the data that has been read.		
      return (status);																	// Return the status that isn't equal to 0x14.
    }
  }
  else if (status != 0x14) {																// We know that if the status is not 0x1D, it has to be 0x14. If not then there is trouble.
	  usb_data = NULL;																		// Discard the data that has been read.
	  return(status);																		// Return the status that isn't equal to 0x14.
  }
  return (status);																		// Return 0x14 (read complete).
}

//*************************************************************************************************************
//	Write sector to file (CH376S).
//*************************************************************************************************************
uint8_t write_sector_CH376S(uint8_t sector_size, uint8_t *usb_data, uint8_t atari_sector_lo, uint8_t atari_sector_hi) {
  uint8_t status;

  status = locate_sector_CH376S(atari_sector_lo, atari_sector_hi);						// Let's put the byte index in the file on the right spot.
  if (status != 0x14) {
    usb_data = NULL;
    return (status);
  }

  status = init_write_CH376S(sector_size);												// Give the CH376S the command to prepare itself for writing data.
  if (status == 0x1E) {																	// If the status is 0x1E then there is still data left to write.
    write_data_CH376S(usb_data);															// Write the actual data.
    status = proceed_write_CH376S();														// Tell the CH376S that we want to write more data.
    if (status != 0x14) {																	// If the status is not 0x14 then something went wrong. If it is 0x14 then continue.
      usb_data = NULL;																		// Discard the data that has been read.		
      return (status);																	// Return the status that isn't equal to 0x14.
    }
  }
  else if (status != 0x14) {																// We know that if the status is not 0x1E, it has to be 0x14. If not then there is trouble.
	  usb_data = NULL;																		// Discard anything that is inside usb_data.
	  return(status);																		// Return the status that isn't equal to 0x14.
  }
  return (status);																		// Return 0x14 (write complete).
}

//*************************************************************************************************************
//	Analyze checksum (ATARI).
//*************************************************************************************************************
bool analyze_checksum(uint8_t *atari_data, uint8_t data_size, uint8_t checksum) {
  bool result = true;																		// The data is valid.
  int sum = 0; 																				// This has to be an INT type variable, because it must be able to hold a value of at least 256 bits + carry.

  for (uint8_t i = 0; i < data_size; i++) {
    sum += atari_data[i];
    if (sum >= 256) {
      sum = (sum - 256) + 1; 																// Add carry into sum.
    }
  }
  if (sum != checksum) {
    result = false; 																		// The data is invalid.
  }
  return (result);
}

//*************************************************************************************************************
//	Calculate checksum (ATARI).
//*************************************************************************************************************
uint8_t calculate_checksum(uint8_t *atari_data, int data_size) {
  int checksum = 0; 																		// This must be an INT type variable, because it must be able to hold a value of at least 256 bits + carry.

  for (int i = 0; i < data_size; i++) {
    checksum += atari_data[i];
    if (checksum >= 256) {
      checksum = (checksum - 256) + 1; 														// Add carry into checksum.
    }
  }
  return (checksum);
}

//**************************************************************************************************************
//	Read commandframe (ATARI).
//**************************************************************************************************************
void read_commandframe(uint8_t *commandframe) {
  uint8_t checksum;
  uint8_t device;

  do {
    do {
      while (read_pin(CMD_pin)) { 								// If the command line is high then no command is being sent, so just wait for it to go low.
		while (Serial1.available() > 0) {
          Serial1.read(); 																	// Discard any data garbage from the serial buffer.
		  }
		}
      
	  Serial1.readBytes(commandframe, 5); 											// Read the command frame while the command line is low. The ATARI sends 5 consecutive bytes. The last byte is checksum.
      device = commandframe[0];
	  
	  while (!read_pin(CMD_pin)) {						// Wait for the command line to go high again.
	  }
    }
    while ((device & 0x30) != 0x30); 														// Repeat until the ATARI sends a commandframe regarding the diskdrive (0x31 - 0x39).

    checksum = commandframe[4];
	if (analyze_checksum(commandframe, 4, checksum) == false) {						// Check if the commandframe contains valid data.
      delay(1); 																			// We have to wait for the Atari to be ready.
      Serial1.write(0x4E); 																	// The commandframe is invalid. Send NAK (0x4E).
    }
  }
  while (analyze_checksum(commandframe, 4, checksum) == false);						// Repeat reading command frames until we get a valid one.
}

//**************************************************************************************************************
//	Execute command (ATARI).
//**************************************************************************************************************
void execute_command(bool boot) {
  uint8_t status_frame[5] = {																// Statusframe of the 'diskdrive' (thumbdrive).
    0x10, 0x00, 0xE0, 0x00, 0x00															// Last byte (0x00) is a dummy and will be replaced by the function calculate_checksum.
  };  
  uint8_t atari_sector[0x81];
  uint8_t status;
  uint8_t checksum;
  uint8_t usb_data[0x81];
  uint8_t commandframe[5];
  uint8_t command;
  uint8_t atari_sector_lo;
  uint8_t atari_sector_hi;

  read_commandframe(commandframe);
  command = commandframe[1];															// The command that has to be executed is sent by the Atari via the second byte of the command frame.
  atari_sector_lo = commandframe[2];	
  atari_sector_hi = commandframe[3];
  switch (command) {									
  
  case 0x21: { 																				// Format (0x21).
    delayMicroseconds(250);																// Wait at least 250 microseconds.
	Serial1.write(0x41); 																	// Write 0x41 (ACK) to confirm the command.
    delayMicroseconds(1250); 																// Write some code to format the ATARI disk (file on 'thumb drive'). Just writing all zeros to the ATARI disk (file on 'thumb drive') will suffice.
    Serial1.write(0x45); 																	// Send an error code (0x45) for now. Still work in progress.
  }
  break;

  case 0x50: {																				// Put sector (without verify) (0x50).
    while (Serial1.available() > 0) {		
      Serial1.read();                														// Discard any data garbage from the serial buffer.
    }
    delayMicroseconds(250);
	Serial1.write(0x41);                													// Write 0x41 (ACK) to confirm the command.
    Serial1.readBytes(atari_sector, 0x81);              									// Read the sector from the ATARI, 0x80 contiguous data bytes + checksum.
    checksum = atari_sector[0x80];
	if (analyze_checksum(atari_sector, 0x80, checksum) == true) {         				// Check if data received is identical to data sent.
      delayMicroseconds(850);																// Wait a minimum of 850 microseconds, so the ATARI can prepare itself to accept 0x41 (ACK).
	  Serial1.write(0x41);                													// Write 0x41 (ACK) to confirm that the data that has been sent is ok.
      delayMicroseconds(250);                												// Wait at least 250 microseconds, so the ATARI can prepare itself to accept 0x43 (COMPLETE).
      status = write_sector_CH376S(0x80,
	  atari_sector, atari_sector_lo,
	  atari_sector_hi);
      if (status != 0x14) {               													// Check the status and act on it.
        Serial1.write(0x45);                												// Something went wrong, so send an error code (0x45).
      } else {
        Serial1.write(0x43);                												// All went well, so send 0x43 (COMPLETE).
      }
    } else {
      Serial1.write(0x4E);                													// No, data is corrupted, so send NAK (0x4E).
    }
  }
  break;

  case 0x52: {																				// Read sector (0x52).
    delayMicroseconds(250); 																// Wait at least 250 microseconds.
    Serial1.write(0x41); 																	// Write 0x41 (ACK) to confirm the command.
    if (boot == false) {																	// Do we boot?
		status = read_sector_CH376S(0x80, usb_data,
		atari_sector_lo, atari_sector_hi);
		if (status != 0x14) {																	// check the status and act on it.
			Serial1.write(0x45);		 														// Something went wrong, so send an error code (0x45).
      } else {
			delayMicroseconds(250); 															// Wait at least 250 microseconds, so the ATARI can prepare itself to accept 0x43 (COMPLETE).
			Serial1.write(0x43); 																// All went well, so send 0x43 (COMPLETE).
			checksum = calculate_checksum(usb_data, 0x80); 										// Calculate the checksum.
			usb_data[0x80] = checksum;															// Append the checksum to the data.
			Serial1.write(usb_data, 0x81); 														// Send the sector to the ATARI.
			}
	} else {
		delayMicroseconds(250); 																// Wait at least 250 microseconds, so the ATARI can prepare itself to accept 0x43 (COMPLETE).
		Serial1.write(0x43); 																	// Send 0x43 (COMPLETE) and proceed the boot process.
		}
  }
  break;

  case 0x53: {																				// Get status (0x53).
    delayMicroseconds(250);																	// Wait at least 250 microseconds.
	Serial1.write(0x41); 																	// Write 0x41 (ACK), because the command is what we expected it to be.
    delayMicroseconds(250); 																// Delay is needed here. We have to wait for the ATARI to get ready.
    Serial1.write(0x43); 																	// Tell the ATARI the job is done.
    status_frame[4] =
	calculate_checksum(status_frame, 4);
    Serial1.write(status_frame, 5); 														// Let the ATARI know that the 'diskdrive' is connected, alive and available.
  }
  break;

  case 0x57: {               																// Write sector (we still have to make it with verify) (0x57).
    while (Serial1.available() > 0) {		
      Serial1.read();                														// Discard any data garbage from the serial buffer.
    }
    delayMicroseconds(250);
	Serial1.write(0x41);                													// Write 0x41 (ACK) to confirm the command.
    Serial1.readBytes(atari_sector, 0x81);              									// Read the sector from the ATARI, 0x80 contiguous data bytes + checksum.
    checksum = atari_sector[0x80];
	if (analyze_checksum(atari_sector, 0x80, checksum) == true) {         					// Check if data received is identical to data sent.
      delayMicroseconds(850);																// Wait a minimum of 850 microseconds, so the ATARI can prepare itself to accept 0x41 (ACK).
	  Serial1.write(0x41);                													// Write 0x41 (ACK) to confirm that the data that has been sent is ok.
      delayMicroseconds(250);                												// Wait at least 250 microseconds, so the ATARI can prepare itself to accept 0x43 (COMPLETE).
      status = write_sector_CH376S(0x80,
	  atari_sector, atari_sector_lo,
	  atari_sector_hi);
      if (status != 0x14) {               													// check the status and act on it.
        Serial1.write(0x45);                												// Something went wrong, so send an error code (0x45).
      } else {
        Serial1.write(0x43);                												// All went well, so send 0x43 (COMPLETE).
      }
    } else {
      Serial1.write(0x4E);                													// No, data is corrupted, so send NAK (0x4E).
    }
  }
  break;
  }
}

//*************************************************************************************************************
//	Boot ATARI (ATARI).
//**************************************************************************************************************
void boot_atari(void) {
  uint8_t checksum;
  uint8_t *bootsector;
  uint8_t bootcode[] = {
	0x00,0x01,0x00,0x06,0x06,0x06,0xEA,0xA2,0x00,0xA9,0x09,0x9D,0x42,0x03,0xA9,0x28,
	0x9D,0x44,0x03,0xA9,0x06,0x9D,0x45,0x03,0xA9,0x00,0x9D,0x48,0x03,0xA9,0x0A,0x9D,
	0x49,0x03,0x20,0x56,0xE4,0x4C,0xA7,0xC5,0x42,0x4F,0x4F,0x54,0x49,0x4E,0x47,0x2E,
	0x2E,0x2E,0x9B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};

	execute_command(true);														// Execute the received command (0x53) and make known we're booting (true).
	for (uint8_t i = 0; i < bootcode[1]; i++) {    											// Send the bootcode in sectors (0x80 bytes) to the ATARI. Initially, the ATARI can only receive sectors of 0x80 bytes as it's parameter is part of the ROM.
		bootsector = bootcode + (0x80 * i);														// The # of sectors to send can be found in the second byte (of the header) of the bootcode.
		checksum = calculate_checksum(bootsector, 0x80);
		execute_command(true);                									// Call the function with true because we boot.
		Serial1.write(bootsector, 0x80); 														// Send the bootsector to the ATARI.
		Serial1.write(checksum);
	}
}
 
//*************************************************************************************************************
//	Read a complete file from an USB memory stick and send it to the ATARI (ATARI).
//**************************************************************************************************************
void read_file(const char filename[15]) {

  set_filename_CH376S(filename);
  open_file_CH376S();

  do {
	execute_command(false); 													// Call the function with false because we don't boot.
  }
  while (1);
}

//*************************************************************************************************************
//	Setup (Arduino).
//*************************************************************************************************************
void setup(void) {
	
	Set_PinMode(SWITCH_pin, InPut);
	Set_PinMode(BZ_pin, InPut);
	Set_PinMode(INT_pin, InPut);
	Set_PinMode(CMD_pin, InPut);
	Set_PinMode(SCS_pin, OutPut);
	Set_PinMode(LED_pin, OutPut);
		
	SPI.begin();
	SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
	Serial1.begin(19200, SERIAL_8N1);
}

//*************************************************************************************************************
//	Loop (Arduino).
//*************************************************************************************************************
void loop(void) {
  boot_atari();
  check_CH376S();
  set_USB_mode(0x07); 																		// Check if a USB mass storage device is connected and choose between USB_modes.
  set_USB_mode(0x06);
  mount_disk_CH376S();
  read_file("/name_of_game");																// Put here the name of the game or program you want to load and run.
}