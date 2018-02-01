#include <stdio.h>
#include "xparameters.h"

#define ENTRY_POINT 0x006e0000
#define LOAD_FROM_CF 1
#define KERNEL_FILENAME "kernel.bin"


#ifdef LOAD_FROM_CF
#include <sysace_stdio.h>
#include "xsysace_l.h"
#include "xsysace.h"
#endif

void (*start)() = (void (*) ()) (XPAR_DDR2_SDRAM_MEM_BASEADDR + ENTRY_POINT);

int main()
{
#ifdef LOAD_FROM_CF
	//sysace_fread can read byte instead of word, so char is used
	unsigned char *ram_p = (unsigned char *) (XPAR_DDR2_SDRAM_MEM_BASEADDR + ENTRY_POINT); //it points to the 1st writable byte
	int Status;
	XSysAce SysAce;
	SYSACE_FILE *file;
	int numread = 0;
#else
	unsigned int *flash_p = (unsigned int *) XPAR_FLASH_MEM0_BASEADDR;
	unsigned int *ram_p = (unsigned int *) (XPAR_DDR2_SDRAM_MEM_BASEADDR + ENTRY_POINT);
	unsigned int i;
#endif


#ifdef LOAD_FROM_CF
	Status = XSysAce_Initialize(&SysAce, XPAR_SYSACE_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		print("System ACE initialization failure.\n\r");
		return XST_FAILURE;
	}

	print("Copying kernel file to RAM...\n\r");
	file = sysace_fopen(KERNEL_FILENAME, "r");

	if(file) {
		do {
			numread = sysace_fread(ram_p, 1, 512, file);
			ram_p += numread;
		} while(numread);

		sysace_fclose(file);
	} else {
		print("File not found.\n\r");
		return XST_FAILURE;
	}

#else
	print("Copying FLASH contents to RAM...\n\r");

	for(i=0; i <= XPAR_FLASH_MEM0_HIGHADDR - XPAR_FLASH_MEM0_BASEADDR; i+=4) {
		*ram_p = *flash_p;
		flash_p++;
		ram_p++;
	}
	print("done!\n\r");
#endif

	print("Starting application...\n\r");
	start();
	return 0;
}

