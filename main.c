#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include  "disp_config.h"


int main()
{
	int enter;
	initBMP();
	//resetDisplay();	
	while(1)
	{
		printf("===================\n");
		printf("1:show bmp\n");
		printf("2:Gen chaos\n");
		printf("3:trasfer bmp\n");
		printf("4:save bmp\n");
		printf("5:FR start\n");
		printf("6:FR stop\n");
		printf("7:VIP MIX start\n");
		printf("8:VIP MIX stop\n");
		printf("9:Ram4HPS\n");
		printf("10:Ram4FPGA\n");
		printf("11:encrypt\n");
		printf("12:resetDisplay\n");
		printf("13:resetSDRAM\n");
		printf("14:decrypt\n");
		printf("0:Exit\n");
		printf("===================\n");
		scanf("%d",&enter);
		switch (enter){
			case 1:
				showBMP();
				break;
			case 2:
				genChaos();
				break;
			case 3:
				socket_server();
				break;
			case 4:
				saveBMP();
				break;				
			case 5:
				VIP_FR_Start(VIDEO_WIDTH, VIDEO_HEIGHT);
				break;
			case 6:
				VIP_FR_Stop();
				break;
			case 7:
				VIP_MIX_Start();
				break;
			case 8:
				VIP_MIX_Stop();
				break;
			case 9:
				Ram4HPS();
				break;
			case 10:
				Ram4FPGA();
				break;
			case 11:
				Enable_encrypt();
				resetFPGA();
				break;
			case 12:
				resetDisplay();
				break;
			case 13:
				resetSDRAM();
				break;	
			case 14:
				Enable_decrypt();
				resetFPGA();
				break;				
			case 0:
				return 0;
				break;				
			default:
				printf("invalid entry \n"); 
				break;
		}
		
	}
	deinitBMP();
	return 0;
}