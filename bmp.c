#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
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


unsigned long *h2p_lw_axi_addr=NULL;
unsigned long *h2p_vip_frame_reader0_addr=NULL;
unsigned long *h2p_memory_addr=NULL;
unsigned long *h2p_vip_mix_addr=NULL;
void *lw_axi_virtual_base=NULL;
void *axi_virtual_base=NULL;
int fd;
void *h2p_lw_h2f_addr=NULL;




/////////////////////////////////////////////////////////
// VIP Frame Reader: configure

void    VIP_FR_Config(int Width, int Height){
	int word = Width*Height;
	int cycle = Width*Height;
	int interlace = 0;
	
	// stop
	h2p_vip_frame_reader0_addr[0]=0x00; // stop
	printf("Width=%d\r\n",Width);
	printf("Height=%d\r\n",Height);
	// configure frame 0
	h2p_vip_frame_reader0_addr[4]=DEMO_VGA_FRAME0_ADDR+FR0_FRAME0_OFFSET; // // frame0 base address
	h2p_vip_frame_reader0_addr[5]=word; // frame0 word
	h2p_vip_frame_reader0_addr[6]=cycle; //  The number of single-cycle color patterns to read for the frame
	h2p_vip_frame_reader0_addr[8]=Width; // frame0 width  
	h2p_vip_frame_reader0_addr[9]=Height; // frame0 height
	h2p_vip_frame_reader0_addr[10]=interlace; // frame0 interlace

	h2p_vip_frame_reader0_addr[0]=0x01; //start

	// select active frame
	h2p_vip_frame_reader0_addr[3]=0; // active frame 0 was set
		
}

void    VIP_FR_Start(int Width, int Height){
	int word = Width*Height;
	int cycle = Width*Height;
	int interlace = 0;
	
	// stop
	h2p_vip_frame_reader0_addr[0]=0x00; // stop
	printf("Width=%d\r\n",Width);
	printf("Height=%d\r\n",Height);
	// configure frame 0
	h2p_vip_frame_reader0_addr[4]=DEMO_VGA_FRAME0_ADDR+FR0_FRAME0_OFFSET; // // frame0 base address
	h2p_vip_frame_reader0_addr[5]=word; // frame0 word
	h2p_vip_frame_reader0_addr[6]=cycle; //  The number of single-cycle color patterns to read for the frame
	h2p_vip_frame_reader0_addr[8]=Width; // frame0 width  
	h2p_vip_frame_reader0_addr[9]=Height; // frame0 height
	h2p_vip_frame_reader0_addr[10]=interlace; // frame0 interlace

	h2p_vip_frame_reader0_addr[0]=0x01; //start

	// select active frame
	h2p_vip_frame_reader0_addr[3]=0; // active frame 0 was set
		
}

void    VIP_FR_Stop(){
	h2p_vip_frame_reader0_addr[0]=0x00; // stop	
}

/////////////////////////////////////////////////////////
// VIP MIX
void    VIP_MIX_Config(void){
 	h2p_vip_mix_addr[0]=0x00; //stop   	
 	// din0 is layer 0, background, fixe	
	// layer 2 (log)
	h2p_vip_mix_addr[2]=130; 
	h2p_vip_mix_addr[3]=770;
	h2p_vip_mix_addr[4]=0x00;
	
	h2p_vip_mix_addr[5]=0;//45;//0;//(SCREEN_WIDTH-VIDEO_WIDTH)/2;//layer1 x offset
	h2p_vip_mix_addr[6]=0;//0;//(SCREEN_HEIGHT-VIDEO_HEIGHT)/2;//layer1 y offset
	h2p_vip_mix_addr[7]=0x01;//set layer 1 active	
	
    h2p_vip_mix_addr[0]=0x01; //start
}
void    VIP_MIX_Start(void){
 	h2p_vip_mix_addr[0]=0x00; //stop
	// din0 is layer 0, background, fixe	
	// layer 2 (log)
	h2p_vip_mix_addr[2]=130; 
	h2p_vip_mix_addr[3]=770;
	h2p_vip_mix_addr[4]=0x00;
	
	h2p_vip_mix_addr[5]=0;//45;//0;//(SCREEN_WIDTH-VIDEO_WIDTH)/2;//layer1 x offset
	h2p_vip_mix_addr[6]=0;//0;//(SCREEN_HEIGHT-VIDEO_HEIGHT)/2;//layer1 y offset
	h2p_vip_mix_addr[7]=0x01;//set layer 1 active	
	
    h2p_vip_mix_addr[0]=0x01; //start

}

void    VIP_MIX_Stop(void){
    h2p_vip_mix_addr[0]=0x00; //stop
}



void  VIP_MIX_Move(int nLayer, int x, int y){
 	h2p_vip_mix_addr[0]=0x00; //stop   	
	
	h2p_vip_mix_addr[nLayer*3-1]=x;//layer1 x offset
	h2p_vip_mix_addr[nLayer*3]=y;//layer1 y offset
	printf("x = %d, y = %d\n",x,y);
        h2p_vip_mix_addr[0]=0x01; //start
}


static int fr0_x = 0;
static int fr0_y = 0;

static int fr1_x = (SCREEN_WIDTH );//-VIDEO_WIDTH
static int fr1_y = (SCREEN_HEIGHT-VIDEO_HEIGHT );	


void PIC_Move(void){

	static bool bX_Add = true;
	static bool bY_Add = true;
	const int nDelta = 5;
	const int nlayer = 2;
	
	
	int abs_x,abs_y;
	
	abs_x = abs(fr1_x - fr0_x);
	abs_y = abs(fr1_y - fr0_y);

    if( abs_x <= VIDEO_WIDTH && abs_y <= VIDEO_HEIGHT){
	    if(abs_x == abs_y){
             	if (bX_Add)		bX_Add = false;
	        	else			bX_Add = true;
		        if (bY_Add)		bY_Add = false;
	        	else			bY_Add = true;

		}else if (abs_x < abs_y)
		{
				if (bY_Add)		bY_Add = false;
	        	else			bY_Add = true;
		}else{
		        if (bX_Add)		bX_Add = false;
	        	else			bX_Add = true;
		
		}
	}
     

	// X direction
	if (bX_Add){
		if ((fr0_x + nDelta) >= SCREEN_WIDTH){ //+VIDEO_WIDTH
			fr0_x = SCREEN_WIDTH-1;//-VIDEO_WIDTH
			bX_Add = false;
		}else{
			fr0_x += nDelta;
		}	
	}else{
		if ((fr0_x-nDelta) < 0){
			fr0_x= 0;
			bX_Add = true;
		}else{
			fr0_x -= nDelta;
		}
	}
	
	// Y direction
	if (bY_Add){
		if ((fr0_y + nDelta)+VIDEO_HEIGHT >= SCREEN_HEIGHT){
			fr0_y = SCREEN_HEIGHT-VIDEO_HEIGHT-1;
			bY_Add = false;
		}else{
			fr0_y += nDelta;
		}	
	}else{
		if ((fr0_y-nDelta) < 0){
			fr0_y= 0;
			bY_Add = true;
		}else{
			fr0_y -= nDelta;
		}
	}
		
   VIP_MIX_Move(nlayer, fr0_x, fr0_y);
}
	
int resetDisplay(){
	unsigned char pixbitcount;
	unsigned int width,height;
	
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);
	setH2FcontrolBit(BIT_SDRAM_FPGA,0);
	
	VIP_MIX_Config();
	VIP_FR_Config(VIDEO_WIDTH, VIDEO_HEIGHT);
	GetBmpData(&pixbitcount,&width,&height, "black.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET);
	return 0;
}

int showBMP() {

	unsigned char pixbitcount;
	unsigned int width,height;
	
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);
	setH2FcontrolBit(BIT_SDRAM_FPGA,0);

	VIP_MIX_Config();
	VIP_FR_Config(VIDEO_WIDTH, VIDEO_HEIGHT);

    usleep(500*1000);
	
	 //GetBmpData(&pixbitcount,&width,&height, "demo1.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET);
	 GetBmpData(&pixbitcount,&width,&height, "lenna.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET);


	 /*
	 while(1)
	 {
		PIC_Move();
		usleep(50000);
	 }
	*/	 
	 return( 0 );
}


int saveBMP() 
{
	FILE *fp;
	int i;
	unsigned char *pAddr=NULL;
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);
	setH2FcontrolBit(BIT_SDRAM_FPGA,0);
	
	GenBmpFile(h2p_memory_addr+FR0_FRAME0_OFFSET,"lenna_after.bmp");
	pAddr = h2p_memory_addr;
	fp = fopen("ram.txt","wb");		
	if( NULL == fp)
	{
		printf(" open file fail");
		return 1;
	}
	for(i = 0; i<1024*1024*15/16;i++)
	{		
		fprintf(fp,"0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\r\n",*pAddr,*(pAddr+1),*(pAddr+2),*(pAddr+3)
		,*(pAddr+4),*(pAddr+5),*(pAddr+6),*(pAddr+7),*(pAddr+8),*(pAddr+9),*(pAddr+10),*(pAddr+11),*(pAddr+12),*(pAddr+13),*(pAddr+14),*(pAddr+15));
		pAddr += 16;
	}
	fclose(fp);
	return( 0 );
}


int initBMP()
{
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}
	lw_axi_virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );	
	if( lw_axi_virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	axi_virtual_base  = mmap( NULL, HW_FPGA_AXI_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd,ALT_AXI_FPGASLVS_OFST  );	
	if( axi_virtual_base == MAP_FAILED ) {
		printf( "ERROR: axi mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	
	h2p_lw_axi_addr=lw_axi_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST) & ( unsigned long)( HW_REGS_MASK ) );;
	h2p_vip_frame_reader0_addr= lw_axi_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ALT_VIP_VFR_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) );	
	h2p_memory_addr=axi_virtual_base + ( ( unsigned long  )( DEMO_VGA_FRAME0_ADDR) & ( unsigned long)( HW_FPGA_AXI_MASK ) );
	h2p_vip_mix_addr=lw_axi_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ALT_VIP_MIX_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) );		
 	h2p_lw_h2f_addr=lw_axi_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_SHIFT_BASE ) & ( unsigned long)( HW_REGS_MASK ) );

	setH2FcontrolBitToZero();
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);
	setH2FcontrolBit(BIT_SDRAM_FPGA,0);
	
	printf( "H2FcontrolBit = %d\n",*(uint32_t *)h2p_lw_h2f_addr );

	return 0;
}

int Ram4HPS()
{
	VIP_MIX_Stop();
	VIP_FR_Stop();
	
	setH2FcontrolBitToZero();
	resetSDRAM();
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);
	setH2FcontrolBit(BIT_SDRAM_FPGA,0);

	return 0;
}

int Ram4FPGA()
{
	unsigned char pixbitcount;
	unsigned int width,height;
	int i,n;
	unsigned char *pAddress=NULL;
	pAddress = h2p_memory_addr;
	//GetBmpData(&pixbitcount,&width,&height, "black.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET);
	/*
	for(i=0;i<1024*1024*10;i++)
	{
		*pAddress = 0x0;
		pAddress +=1;
	}
	*/
	srand(time(NULL));
	/*
	memset((void *)pAddress,0,1024*1024*60);
	
	for(i=0;i<1024*1024*10/128;i++)
	{
		
		n = (rand()%100+1);
		memset((void *)pAddress,n,128);
		pAddress = pAddress + 128;
	}
	
	 //StoreBmpData(&pixbitcount,&width,&height, "black.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET+0x100000);
	 //StoreBmpData(&pixbitcount,&width,&height, "black.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET);
	 memset((void *)pAddress,0,1024*1024*60);
	 for(i=0;i<1024*1024*4/BMP_RAW_2048BYTE;i++)
	{
		
		//n = (rand()%100+1);
		
		memset((void *)pAddress,i,BMP_RAW_2048BYTE);
		pAddress = pAddress + BMP_RAW_2048BYTE;
	}*/
	memset((void *)pAddress,0,1024*1024*60);
	 StoreBmpData(&pixbitcount,&width,&height, "lenna.bmp",h2p_memory_addr+FR0_FRAME0_OFFSET/*+0x4*/);	
	VIP_MIX_Stop();
	VIP_FR_Stop();
	
	usleep(500*1000);
	setH2FcontrolBit(BIT_SDRAM_FPGA,1);
	usleep(50);
	
	return 0;
}


int resetSDRAM()
{
	VIP_MIX_Stop();
	VIP_FR_Stop();

	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);
	usleep(50);
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,0);
	usleep(50);
	setH2FcontrolBit(BIT_RESET_HPS_SDRAM,1);

	
	return 0;
}



int deinitBMP()
{
	h2p_lw_axi_addr = NULL;
	h2p_vip_frame_reader0_addr = NULL;
	h2p_memory_addr = NULL;
	h2p_vip_mix_addr = NULL;
	h2p_lw_h2f_addr = NULL;
	
	 if( munmap( axi_virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	 }
	 if( munmap( lw_axi_virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	 }
	 close( fd );
	 return 0;
}
