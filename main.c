#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include  "disp_config.h"


int main()
{
					uint8_t md,i;
	int enter;
	char filename[20];
	char RGBname[20];	
	char RGBAESname[20];	
	initBMP();
	//resetDisplay();
				//printf("enter SWAP type 0~15\n");

/*				//scanf("%d", &md);	
for(md = 0;md<256;md++){				
i = Mtest_aes(md);
if(i!= md)
{
	printf("%d round fail\n",md);
	return 0;
}
}
				return 0;	
				*/
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
		printf("100:test AES\n");
		printf("101:Gen RGB Bmp\n");
		printf("102:test RGBAES\n");
		printf("103:Mtest AES\n");
		printf("104:Recover RGB Bmp\n");
		printf("105:test invRGBAES\n");		
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
			case 100:
				test_aes();
				break;
			case 101:

				printf("Do Gen RGB Bmp\n");				
				/*printf("enter source BMP filename\n");
				scanf("%s",filename);
				printf("enter target BMP filename\n");
				scanf("%s",RGBname);*/				
				GenRGBbmp("lenna2.bmp","RGB.bmp");
				break;
			case 102:

				printf("Do test RGBAES\n");				
				//printf("enter source BMP filename\n");
				//scanf("%s",filename);
				//printf("enter target BMP filename\n");
				//scanf("%s",RGBAESname);
				//GenRGBbmp("lenna2.bmp","RGB.bmp");				
				test_RGBaes("RGB.bmp","RGBaes.bmp");
				RecoverRGBbmp("RGBaes.bmp","RecoverRGBaes.bmp");
				break;
			case 103:

				printf("enter SWAP type 0~15\n");
				scanf("%d", &md);				
				Mtest_aes(md);
				return 0;
				break;
			case 104:
				printf("Do Recover RGB Bmp\n");				
				/*printf("enter source BMP filename\n");
				scanf("%s",filename);
				printf("enter target BMP filename\n");
				scanf("%s",RGBname);*/				
				RecoverRGBbmp("RGB.bmp","RecoverRGB.bmp");
				break;
			case 105:

				printf("Do test invRGBAES\n");				
				//printf("enter source BMP filename\n");
				//scanf("%s",filename);
				//printf("enter target BMP filename\n");
				//scanf("%s",RGBAESname);			
				test_invRGBaes("RGBaes.bmp","decryptRGB.bmp");
				RecoverRGBbmp("decryptRGB.bmp","decrypt.bmp");
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