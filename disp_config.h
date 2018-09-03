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

#define BMP_ORG_RAW_ADDRESS	(0x500000*2)
#define BMP_ENC_RAW_ADDRESS	(0xA00000*2)

#define MASK_BIT(bit) (1 << (bit)) 

#endif //_INC_CONFIG_H