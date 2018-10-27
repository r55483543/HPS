/*
This program demonstrate how to use hps communicate with FPGA through light AXI Bridge.
uses should program the FPGA by GHRD project before executing the program
refer to user manual chapter 7 for details about the demo
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
#include  "disp_config.h"


int encrypt() {

return( 0 );
}
int Enable_encrypt()
{
	setH2FcontrolBit(BIT_H2F_DECRYPT,0);
	setH2FcontrolBit(BIT_H2F_ENCRYPT,1);
	
	return 0;
}

int Enable_decrypt()
{
	setH2FcontrolBit(BIT_H2F_DECRYPT,1);
	setH2FcontrolBit(BIT_H2F_ENCRYPT,0);
	
	return 0;
}

int resetFPGA()
{
	setH2FcontrolBit(BIT_H2F_SOFTWARE_RESET,1);
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_SOFTWARE_RESET,0);
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_SOFTWARE_RESET,1);
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_GLOBAL_RESET,1);
	usleep(200*1000);	
	setH2FcontrolBit(BIT_H2F_GLOBAL_RESET,0);
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_GLOBAL_RESET,1);	
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_FPGA_START,1);	
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_FPGA_START,0);	
	usleep(200*1000);
	setH2FcontrolBit(BIT_H2F_FPGA_START,1);	
	
	return 0;
}