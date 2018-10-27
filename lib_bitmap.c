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

#include "math.h"
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"	
#include "socal/alt_gpio.h"
#include  "disp_config.h"
#include  "lib_bitmap.h"

/*****Global*****/
BITMAPFILE tempbmpfile;
/****************/

int GenBmpFile(unsigned char *pData, const char *filename);
unsigned char GetBmpData(unsigned char *bitCountPerPix, unsigned int *width, unsigned int *height, const char* filename,unsigned long *memory_vip_frame);
void FreeBmpData(unsigned char *pdata); 
int GenBmpFile(unsigned char *pData, const char *filename)  
{  
    FILE *fp = fopen(filename, "wb");  
    if(!fp)  
    {  
        printf("fopen failed \n");  
        return 0;  
    } 
	printf("pData = %x\n",pData);
	BITMAPFILE bmpfile; 
	memset((void *)&(bmpfile),0,sizeof(BITMAPFILE));
	memcpy((void *)&(bmpfile.bfHeader),(void *)&(tempbmpfile.bfHeader),sizeof(BITMAPFILEHEADER));
	memcpy((void *)&(bmpfile.biInfo.bmiHeader),(void *)&(tempbmpfile.biInfo.bmiHeader),sizeof(BITMAPINFO));	
    unsigned int bmppitch = (((bmpfile.biInfo.bmiHeader.biWidth)*(bmpfile.biInfo.bmiHeader.biBitCount) + 31) >> 5) << 2; 
  
  printf("bmpfile.biInfo.bmiHeader.biHeight = %d\n",bmpfile.biInfo.bmiHeader.biHeight);
  printf("bmpfile.biInfo.bmiHeader.biWidth = %d\n",bmpfile.biInfo.bmiHeader.biWidth);
  printf("bmppitch = %d\n",bmppitch);
    fwrite(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);  
    fwrite(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);  
  
    unsigned char *pEachLinBuf = (unsigned char*)malloc(bmppitch);  
    memset(pEachLinBuf, 0, bmppitch);  
    unsigned char BytePerPix = (bmpfile.biInfo.bmiHeader.biBitCount) >> 3;  
    unsigned int pitch = (bmpfile.biInfo.bmiHeader.biWidth) * BytePerPix;  
	printf("BytePerPix = %d\n",BytePerPix);
	printf("pitch = %d\n",pitch);
    if(pEachLinBuf)  
    {  
        int h,w;  
        for(h = (bmpfile.biInfo.bmiHeader.biHeight)-1; h >= 0; h--)  
        {  
            for(w = 0; w < (bmpfile.biInfo.bmiHeader.biWidth); w++)  
            {  
                //copy by a pixel  
                pEachLinBuf[w*BytePerPix+0] = pData[h*pitch + w*BytePerPix + 0];  
                pEachLinBuf[w*BytePerPix+1] = pData[h*pitch + w*BytePerPix + 1];  
                pEachLinBuf[w*BytePerPix+2] = pData[h*pitch + w*BytePerPix + 2]; 
				//if(pEachLinBuf[w*BytePerPix+0] != 0 || pEachLinBuf[w*BytePerPix+1] != 0 || pEachLinBuf[w*BytePerPix+2] != 0)
					//printf("h=%d ,w =%d ,pEachLinBuf0=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+0)));
//printf("h=%d ,w =%d ,pEachLinBuf[w*BytePerPix+0] =%x\n",h,w,pEachLinBuf[w*BytePerPix+0]);
//printf("h=%d ,w =%d ,pEachLinBuf[w*BytePerPix+1] =%x\n",h,w,pEachLinBuf[w*BytePerPix+1]);
//printf("h=%d ,w =%d ,pEachLinBuf[w*BytePerPix+2] =%x\n",h,w,pEachLinBuf[w*BytePerPix+2]);				
            }  
            fwrite(pEachLinBuf, bmppitch, 1, fp);  
              
        }  
        free(pEachLinBuf);  
    }  
    fclose(fp);  
    return 1;  
}  
 
unsigned char GetBmpData(unsigned char *bitCountPerPix, unsigned int *width, unsigned int *height, const char* filename,unsigned long *memory_vip_frame)  
{  
    FILE *pf = fopen(filename, "rb");  
    if(!pf)  
    {  
        printf("fopen failed \n");  
        return 1;  
    }  
  
    BITMAPFILE bmpfile;  
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);  
    fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, pf);  
	 printf("sizeof(BITMAPFILEHEADER) = %d\n",sizeof(BITMAPFILEHEADER));  
	 printf("sizeof(BITMAPINFOHEADER) = %d\n",sizeof(BITMAPINFOHEADER));
         
    if(bitCountPerPix)  
    {  
        *bitCountPerPix = bmpfile.biInfo.bmiHeader.biBitCount; 
		printf("*bitCountPerPix = %d\n",*bitCountPerPix);		
    }  
    if(width)  
    {  
        *width = bmpfile.biInfo.bmiHeader.biWidth; 
		printf("*width = %d\n",*width);			
    }  
    if(height)  
    {  
        *height = bmpfile.biInfo.bmiHeader.biHeight;  
		printf("*height = %d\n",*height);			
    }  
  	unsigned char *pPattern=NULL;
    unsigned char *pTemp=NULL;
    unsigned int bmppicth = (((*width)*(*bitCountPerPix) + 31) >> 5) << 2; 
	printf("bmppicth = %d\n",bmppicth);
     
    unsigned char *pdata = (unsigned char*)malloc((*height)*bmppicth);
    if (pdata == NULL)
    	{
				printf ("pdata failed to allocation memory\r\n");
				return 1;
	} 
    unsigned char *pdatatemp = (unsigned char*)malloc(VIDEO_FRAME_MEM_SIZE); 
    if (pdatatemp == NULL)
    	{
				printf ("pdatatemp failed to allocation memory\r\n");
				return 1;
		  }
       
    unsigned char *pEachLinBuf = (unsigned char*)malloc(bmppicth);  
    memset(pEachLinBuf, 0, bmppicth);  
    unsigned char BytePerPix = (*bitCountPerPix) >> 3;  
    unsigned int pitch = (*width) * BytePerPix; 
	 printf("BytePerPix = %d\n",BytePerPix); 
	  printf("pitch = %d\n",pitch);
    printf("pdata start \n"); 
  
    if(pdata && pEachLinBuf)  
    {  
        int w, h;  
        for(h = (*height) - 1; h >= 0; h--) 
		//for(h = 0; h < (*height); h++) 			
        {  
            fread(pEachLinBuf, bmppicth, 1, pf);  
            for(w = 0; w < (*width); w++)  
            {  
                pdata[h*pitch + w*BytePerPix + 0] = pEachLinBuf[w*BytePerPix+0];  
                pdata[h*pitch + w*BytePerPix + 1] = pEachLinBuf[w*BytePerPix+1];  
                pdata[h*pitch + w*BytePerPix + 2] = pEachLinBuf[w*BytePerPix+2];  
				//printf("h=%d ,w =%d ,pEachLinBuf0=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+0)));
				//printf("h=%d ,w =%d ,pEachLinBuf1=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+1)));
				//printf("h=%d ,w =%d ,pEachLinBuf2=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+2)));
            }  
        }  
        free(pEachLinBuf);  
    }  
    fclose(pf);
    printf("pdata end \n");   
    printf("pdatatemp start \n"); 
	printf("VIDEO_FRAME_MEM_SIZE = %d \n",VIDEO_FRAME_MEM_SIZE); 
    
    //BGR--xRGB
    pTemp=pdata;

    pPattern=pdatatemp;
    int j;
	int pixelCount = 1;
    for(j=0;j<VIDEO_FRAME_MEM_SIZE/4;j++)
    {
		
		if(*width < VIDEO_WIDTH)
		{

			//printf("pixelCount = %d \n",pixelCount);   
			if(pixelCount > *width)
			{
			*(pPattern+0)=0;
			*(pPattern+1)=0;
			*(pPattern+2)=0;
			*(pPattern+3)=0;				
				pPattern += 4;
				//pTemp += 3;
				if(pixelCount == VIDEO_WIDTH)
				{
					pixelCount = 1;
				}else
				{
					pixelCount += 1;	
				}
							
			}else{
				*(pPattern+0)=*(pTemp+0);
				*(pPattern+1)=*(pTemp+1);
				*(pPattern+2)=*(pTemp+2);
				*(pPattern+3)=0;
				if(j<10){
					printf("B = %x \n",*(pTemp+0)); 
					printf("G = %x \n",*(pTemp+1));
					printf("R = %x \n",*(pTemp+2));
				}
				pPattern += 4;
				pTemp += 3;
				pixelCount += 1;	
			}
			
		}
		else if(*width > VIDEO_WIDTH)
		{
			*(pPattern+0)=*(pTemp+0);
			*(pPattern+1)=*(pTemp+1);
			*(pPattern+2)=*(pTemp+2);
			*(pPattern+3)=0;
			
			if(pixelCount == VIDEO_WIDTH)
			{
				pPattern += 4;
				pTemp += 3*(*width - VIDEO_WIDTH);
				pixelCount = 1;				
			}else{
				pPattern += 4;
				pTemp += 3;
				pixelCount += 1;	
			}			
		}
		else
		{
			*(pPattern+0)=*(pTemp+0);
			*(pPattern+1)=*(pTemp+1);
			*(pPattern+2)=*(pTemp+2);
			*(pPattern+3)=0; 
	/*
			  *(pPattern+4)=*(pTemp+3);
				*(pPattern+5)=*(pTemp+4);
			  *(pPattern+6)=*(pTemp+5);
				*(pPattern+7)=0; 

			  *(pPattern+8)=*(pTemp+6);
				*(pPattern+9)=*(pTemp+7);
			  *(pPattern+10)=*(pTemp+8);
				*(pPattern+11)=0; 

			*(pPattern+12)=*(pTemp+9);
				*(pPattern+13)=*(pTemp+10);
			  *(pPattern+14)=*(pTemp+11);
				*(pPattern+15)=0; 

			*(pPattern+16)=*(pTemp+12);
				*(pPattern+17)=*(pTemp+13); 
			*(pPattern+18)=*(pTemp+14);
				*(pPattern+19)=0; 

			*(pPattern+20)=*(pTemp+15);
				*(pPattern+21)=*(pTemp+16);
			*(pPattern+22)=*(pTemp+17);
				*(pPattern+23)=0; 

			*(pPattern+24)=*(pTemp+18);
				*(pPattern+25)=*(pTemp+19);
			*(pPattern+26)=*(pTemp+20);
				*(pPattern+27)=0; 

			*(pPattern+28)=*(pTemp+21);
				*(pPattern+29)=*(pTemp+22);
		  *(pPattern+30)=*(pTemp+23);
				*(pPattern+31)=0; 
	*/
			pPattern += 4;
			  pTemp += 3;
		}

		}
		printf("pdatatemp end \n");	
		if(pdata)
			free(pdata);
    memcpy((void *)memory_vip_frame,(void *)pdatatemp,VIDEO_FRAME_MEM_SIZE);
	//memcpy((void *)(memory_vip_frame+BMP_ORG_RAW_ADDRESS),(void *)pdatatemp,VIDEO_FRAME_MEM_SIZE);
	if(pdatatemp)
		free(pdatatemp);
    return 0;  
}


unsigned char StoreBmpData(unsigned char *bitCountPerPix, unsigned int *width, unsigned int *height, const char* filename,unsigned long *memory_vip_frame)  
{  
	unsigned int bmpsize = 0;
    FILE *pf = fopen(filename, "rb");  
    if(!pf)  
    {  
        printf("fopen failed \n");  
        return 1;  
    }  
  
    BITMAPFILE bmpfile;  
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);  
    fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, pf);  
	 printf("sizeof(BITMAPFILEHEADER) = %d\n",sizeof(BITMAPFILEHEADER));  
	 printf("sizeof(BITMAPINFOHEADER) = %d\n",sizeof(BITMAPINFOHEADER));
	
	if(&(tempbmpfile) == NULL)
		printf("&(tempbmpfile) is NULL\n");
	else
	{
		printf("&(tempbmpfile) is not NULL\n");
		memset((void *)&(tempbmpfile),0,sizeof(BITMAPFILE));
		memcpy((void *)&(tempbmpfile.bfHeader),(void *)&(bmpfile.bfHeader),sizeof(BITMAPFILEHEADER));
		memcpy((void *)&(tempbmpfile.biInfo.bmiHeader),(void *)&(bmpfile.biInfo.bmiHeader),sizeof(BITMAPINFO));
		printf("tempbmpfile.biInfo.bmiHeader.biWidth = %d\n",tempbmpfile.biInfo.bmiHeader.biWidth);
	}
         
    if(bitCountPerPix)  
    {  
        *bitCountPerPix = bmpfile.biInfo.bmiHeader.biBitCount; 
		printf("*bitCountPerPix = %d\n",*bitCountPerPix);		
    }  
    if(width)  
    {  
        *width = bmpfile.biInfo.bmiHeader.biWidth; 
		printf("*width = %d\n",*width);			
    }  
    if(height)  
    {  
        *height = bmpfile.biInfo.bmiHeader.biHeight;  
		printf("*height = %d\n",*height);			
    } 
	bmpsize = bmpfile.biInfo.bmiHeader.biSizeImage;
	printf("bmpsize = %d\n",bmpsize);
    unsigned char *pTemp=NULL;
    unsigned int bmppicth = (((*width)*(*bitCountPerPix) + 31) >> 5) << 2; 
	printf("bmppicth = %d\n",bmppicth);
     
    unsigned char *pdata = (unsigned char*)malloc((*height)*bmppicth);
    if (pdata == NULL)
    	{
				printf ("pdata failed to allocation memory\r\n");
				return 1;
	} 
       
    unsigned char *pEachLinBuf = (unsigned char*)malloc(bmppicth);  
    memset(pEachLinBuf, 0, bmppicth);  
    unsigned char BytePerPix = (*bitCountPerPix) >> 3;  
    unsigned int pitch = (*width) * BytePerPix; 
    printf("pdata start \n"); 
  
    if(pdata && pEachLinBuf)  
    {  
        int w, h;  
        for(h = (*height) - 1; h >= 0; h--) 
		//for(h = 0; h < (*height); h++) 			
        {  
            fread(pEachLinBuf, bmppicth, 1, pf);  
            for(w = 0; w < (*width); w++)  
            {  
                pdata[h*pitch + w*BytePerPix + 0] = pEachLinBuf[w*BytePerPix+0];  
                pdata[h*pitch + w*BytePerPix + 1] = pEachLinBuf[w*BytePerPix+1];  
                pdata[h*pitch + w*BytePerPix + 2] = pEachLinBuf[w*BytePerPix+2];  
				//printf("h=%d ,w =%d ,pEachLinBuf0=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+0)));
				//printf("h=%d ,w =%d ,pEachLinBuf1=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+1)));
				//printf("h=%d ,w =%d ,pEachLinBuf2=%x\n",h,w,*(pEachLinBuf+(w*BytePerPix+2)));
				//if(pdata[h*pitch + w*BytePerPix + 0] != 0 || pdata[h*pitch + w*BytePerPix + 1] != 0 || pdata[h*pitch + w*BytePerPix + 2] != 0)
					//printf("h=%d ,w =%d \n",h,w);
            }  
        }  
        free(pEachLinBuf);  
    }  
    fclose(pf);
    printf("pdata end \n");   
    
    pTemp=pdata;

    memcpy((void *)memory_vip_frame,(void *)pTemp,bmpsize);
	printf("memory_vip_frame = %x\n",memory_vip_frame);
	if(pdata)
		free(pdata);
    return 0;  
}

void FreeBmpData(unsigned char *pdata)  
{  
    if(pdata)  
    {  
        free(pdata);  
        pdata = NULL;  
    }  
}  
  
typedef struct _LI_RGB  
{  
    unsigned char b;  
    unsigned char g;  
    unsigned char r;  
}LI_RGB; 
