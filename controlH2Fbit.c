#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
#include  "hps_0.h"
#include "math.h"
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"	
#include "socal/alt_gpio.h"
#include  "disp_config.h"
#include  "lib_bitmap.h"

void setH2FcontrolBit(int bit, int value)
{
	if(bit > BIT_RESET_HPS_SDRAM)
		printf("setH2FControlBit bit over");
	else if(value != 1 && value != 0)
		printf("setH2FControlBit value over");
	else
	{
		if(value == 1)
			*(uint32_t *)h2p_lw_h2f_addr = *(uint32_t *)h2p_lw_h2f_addr | MASK_BIT(bit);
		else
			*(uint32_t *)h2p_lw_h2f_addr = *(uint32_t *)h2p_lw_h2f_addr & ~MASK_BIT(bit);
	}
	
}

void setH2FcontrolBitToZero()
{
	*(uint32_t *)h2p_lw_h2f_addr = 0;
}