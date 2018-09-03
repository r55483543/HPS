#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int showBMP();
int genChaos();
int socket_server();
int saveBMP();
void initBMP();
void deinitBMP();

int main()
{
	int enter;
	initBMP();
	while(1)
	{
		printf("===================\n");
		printf("1:show bmp\n");
		printf("2:Gen chaos\n");
		printf("3:trasfer bmp\n");
		printf("4:save bmp\n");
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