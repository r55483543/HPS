#ifndef _INC_CONFIG_H
#define _INC_CONFIG_H


//#define VGA_WIDTH									1920 //			1024
//#define VGA_HEIGHT									1080 //		768
//#define VGA_WIDTH									1024
//#define VGA_HEIGHT									768

// dimenstion for full screen
#define SCREEN_WIDTH									(1280)//1024			
#define SCREEN_HEIGHT									(1024 - 249)//768

// dimension for mpeg video
#define VIDEO_WIDTH									    640
#define VIDEO_HEIGHT									480
#define VIDEO_BYTE_PER_PIXEL							4   //changed to sit one color 
#define VIDEO_FRAME_PIXEL_NUM							(VIDEO_WIDTH*VIDEO_HEIGHT)
#define VIDEO_FRAME_MEM_SIZE							(VIDEO_FRAME_PIXEL_NUM*VIDEO_BYTE_PER_PIXEL)



#define SDRAM_BASE_ADDR 0
#define ALT_VIP_SOFTWARE_RESET_N_BASE 0x00000200   //

//#define ALT_STM_OFST (0xfc000000)
//#define ALT_LWFPGASLVS_OFST (0xff200000)  // axi_lw_master

#define ALT_AXI_FPGASLVS_OFST (0xC0000000)  // axi_master
#define HW_FPGA_AXI_SPAN (0x40000000)  // Bridge span
#define HW_FPGA_AXI_MASK ( HW_FPGA_AXI_SPAN - 1 )

#define ALT_GPIO1_BASE_OFST   (0xFF709000)

#define HW_REGS_BASE (ALT_STM_OFST )
#define HW_REGS_SPAN (0x04000000 )
#define HW_REGS_MASK (HW_REGS_SPAN - 1 )


#define DEMO_VGA_FRAME0_ADDR					                0x00000000//0x00080000 //0x00100000  //on chip memory base
#define FR0_FRAME0_OFFSET							(0x00000000)


#define BMP_ORG_RAW_ADDRESS	(0x500000) //(0xA01000)
#define BMP_ENC_RAW_ADDRESS	(0x500000)

#define MASK_BIT(bit) (1 << (bit)) 


#define BIT_H2F_SOFTWARE_RESET		(int)0 //1 0 1
#define BIT_H2F_GLOBAL_RESET		(int)1//1 0 1
#define BIT_H2F_FPGA_START			(int)2	//1 0 1
#define BIT_H2F_DECRYPT				(int)3
#define BIT_H2F_ENCRYPT				(int)4
#define BIT_SDRAM_FPGA				(int)5
#define BIT_RESET_HPS_SDRAM			(int)6

#define BIT_F2H_AES_D_DONE 			(int)3
#define BIT_F2H_AES_E_DONE 			(int)4

extern unsigned long *h2p_lw_axi_addr;
extern unsigned long *h2p_vip_frame_reader0_addr;
extern unsigned long *h2p_memory_addr;
extern unsigned long *h2p_vip_mix_addr;
extern void *lw_axi_virtual_base;
extern void *axi_virtual_base;
extern void *h2p_lw_h2f_addr;


extern int showBMP();
extern int genChaos();
extern int socket_server();
extern int saveBMP();
extern int initBMP();
extern int deinitBMP();
extern int resetDisplay();
extern void VIP_FR_Start(int Width, int Height);
extern void VIP_FR_Stop();
extern void VIP_MIX_Start(void);
extern void VIP_MIX_Stop(void);
extern int Ram4HPS();
extern int Ram4FPGA();
extern int resetFPGA();
extern int Enable_encrypt();
extern int resetSDRAM();
extern void setH2FcontrolBit(int bit, int value);
extern void setH2FcontrolBitToZero();


#endif //_INC_CONFIG_H