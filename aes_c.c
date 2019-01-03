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
#include "aes.h"

void print(const char *msg, const uint8_t *buf)
{
	printf("%s", msg);
	int i;
	for(i=0; i<16; ++i)
		printf("%02x ", buf[i]);
	printf("\n");
}

int test_aes()
{
	uint8_t ret_text[16] = {0};
	uint8_t text[16] = {
		/*0x01,0x23,0x45,0x67,
		0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,
		0x76,0x54,0x32,0x10*/0
	};
	uint8_t cipher_text[16] = {0};
	uint8_t key[32] = {
		/*0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98,
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98*/0
	};
	
	uint32_t key_bit[1] = {128};
	
	aes_context ctx;
	uint32_t i;
	for (i = 0; i < sizeof(key_bit)/sizeof(key_bit[0]); ++i)
	{
		if (aes_set_key(&ctx, key, key_bit[i]) != SUCCESS)
		{
			perror("aes_set_key error.");
			return -1;
		}
		if(aes_encrypt_block(&ctx, cipher_text, text) != SUCCESS)
		{
			perror("aes_encrypt_block error.");
			return -1;
		}
		if(aes_decrypt_block(&ctx, ret_text, cipher_text) != SUCCESS)
		{
			perror("aes_decrypt_block error.");
			return -1;
		}
		printf("key_bit %d: \n", key_bit[i]);
		print("\tinput  :  ", text);
		print("\tencrypt:  ", cipher_text);
		print("\tdecrypt:  ", ret_text);
	}
	return 0;
}

int Mtest_aes(uint8_t type)
{
	uint8_t ret_text[16] = {0};
	uint8_t text[16] = {
		/*0x01,0x23,0x45,0x67,
		0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,
		0x76,0x54,0x32,0x10*/0xe2 ,0x79 ,0x91 ,0xe2 ,0x79 ,0x91 ,0xe2 ,0x79 ,0x91 ,0xe2 ,0x69 ,0x79 ,0xda ,0x59 ,0x5d ,0xc6
		

	};
	uint8_t cipher_text[16] = {0};
	uint8_t key[16] = {
		/*0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98,
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98*/0
	};
	
	uint32_t key_bit[1] = {128};
	
	aes_context ctx;
	uint32_t i;
	//uint8_t md = 0;
	uint8_t j;
	for (i = 0; i < sizeof(key_bit)/sizeof(key_bit[0]); ++i)
	{
		if (aes_set_key(&ctx, key, key_bit[i]) != SUCCESS)
		{
			perror("aes_set_key error.");
			return -1;
		}
		printf("enter SWAP type 0~15\n");
		
		//scanf("%d", &md);
		//for(j = 0;j<16;j++)
			//text[j] = type;
		//printf("enter SWAP type %d\n", md);
		setSWAP_Type(type);
		printf("1\n");
		if(Maes_encrypt_block(&ctx, cipher_text, text) != SUCCESS)
		{
			perror("aes_encrypt_block error.");
			return -1;
		}
		printf("2\n");
		if(Maes_decrypt_block(&ctx, ret_text, cipher_text) != SUCCESS)
		{
			perror("aes_decrypt_block error.");
			return -1;
		}
		printf("3\n");		
		printf("key_bit %d: \n", key_bit[0]);
		print("\tinput  :  ", text);
		print("\tencrypt:  ", cipher_text);
		print("\tdecrypt:  ", ret_text);
	}
	return ret_text[0];
}


int test_RGBaes(const char* filename, char* AESname)
{
	unsigned char *ptAddr=NULL;
	uint8_t j;
	uint8_t ret_text[16] = {0};
	uint8_t text[16] = {
		/*0x01,0x23,0x45,0x67,
		0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,
		0x76,0x54,0x32,0x10*/0
	};
	uint8_t cipher_text[16] = {0};
	uint8_t key[16] = {
		/*0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98,
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98*/0
	};
	
	uint32_t key_bit[1] = {128};
	
	aes_context ctx;
	getChaosKey(key);
	unsigned int bmpsize = 0;
	int i;
    FILE *pf = fopen(filename, "rb"); 
	FILE *fp = fopen(AESname, "wb");
	if (aes_set_key(&ctx, key, key_bit[0]) != SUCCESS)
	{
		perror("aes_set_key error.");
		return -1;
	}	
    if(!pf)  
    {  
        printf("fopen %s failed \n",filename);  
        return 1;  
    }
    if(!fp)  
    {  
        printf("fopen %s failed \n",AESname);  
        return 1;  
    }	
  
    BITMAPFILE bmpfile;
	memset((void *)&(bmpfile),0,sizeof(BITMAPFILE));	
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);  
    fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, pf);  
	 printf("sizeof(BITMAPFILEHEADER) = %d\n",sizeof(BITMAPFILEHEADER));  
	 printf("sizeof(BITMAPINFOHEADER) = %d\n",sizeof(BITMAPINFOHEADER));
        
	bmpsize = bmpfile.biInfo.bmiHeader.biSizeImage;
	printf("bmpsize = %d\n",bmpsize);
/* 
    char *pEachBMPBuf = (char*)malloc(bmpsize);
	char *pEachBBuf = (char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	char *pEachGBuf = (char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	char *pEachRBuf = (char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	
	memset(pEachBMPBuf, 0, bmpsize);
	memset(pEachBBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	memset(pEachGBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	memset(pEachRBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
*/	
    printf("pdata start \n");
    fwrite(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);  
    fwrite(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);	
	//getlastChaosAddr(ptAddr);
	ptAddr = lastChaosAddr;
	printf("encrypt lastChaosAddr = %x \n",lastChaosAddr);
	printf("encrypt ptAddr = %x \n",ptAddr);
	printf("encrypt *ptAddr = %d \n",*((uint8_t*)ptAddr));

	//i = 0;
	//while(!feof(pf))
	for(i=0; i<bmpsize/16;i++)
	{
		memset(text, 0, sizeof(text));
		memset(cipher_text, 0, sizeof(cipher_text));
		fread(text, sizeof(text), 1, pf);
		//printf("encrypt ptAddr = %x \n",ptAddr);
		if(i == (bmpsize/16)-1){
			printf("encrypt *((uint8_t*)ptAddr) = %d \n",*((uint8_t*)(ptAddr-i)));
		}	
		setSWAP_Type(*((uint8_t*)(ptAddr-i)));
		if(Maes_encrypt_block(&ctx, cipher_text, text) != SUCCESS)
		{
			perror("Maes_encrypt_block error.");
			return -1;
		}
		fwrite(cipher_text, sizeof(cipher_text), 1, fp);
		//fwrite(text, sizeof(text), 1, fp);
		//i++;
		//ptAddr = ptAddr - 1;
		//printf("round %d\n",i);
	}	
	
    fclose(pf);
	fclose(fp);
    printf("pdata end \n");   
 

/*
		if(aes_decrypt_block(&ctx, ret_text, cipher_text) != SUCCESS)
		{
			perror("aes_decrypt_block error.");
			return -1;
		}
*/		
		printf("key_bit %d: \n", key_bit[0]);
		print("\tinput  :  ", text);
		print("\tencrypt:  ", cipher_text);
		//print("\tdecrypt:  ", ret_text);

	return 0;
}
int test_invRGBaes(const char* filename, char* AESname)
{
	unsigned char *ptAddr=NULL;
	uint8_t j;
	uint8_t ret_text[16] = {0};
	uint8_t text[16] = {
		/*0x01,0x23,0x45,0x67,
		0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,
		0x76,0x54,0x32,0x10*/0
	};
	uint8_t cipher_text[16] = {0};
	uint8_t key[16] = {
		/*0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98,
		0x0f,0x15,0x71,0xc9,
		0x47,0xd9,0xe8,0x59,
		0x0c,0xb7,0xad,0xd6,
		0xaf,0x7f,0x67,0x98*/0
	};
	
	uint32_t key_bit[1] = {128};
	
	aes_context ctx;
	getChaosKey(key);
	unsigned int bmpsize = 0;
	int i;
    FILE *pf = fopen(filename, "rb"); 
	FILE *fp = fopen(AESname, "wb");
	if (aes_set_key(&ctx, key, key_bit[0]) != SUCCESS)
	{
		perror("aes_set_key error.");
		return -1;
	}	
    if(!pf)  
    {  
        printf("fopen %s failed \n",filename);  
        return 1;  
    }
    if(!fp)  
    {  
        printf("fopen %s failed \n",AESname);  
        return 1;  
    }	
  
    BITMAPFILE bmpfile;
	memset((void *)&(bmpfile),0,sizeof(BITMAPFILE));	
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);  
    fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, pf);  
	 printf("sizeof(BITMAPFILEHEADER) = %d\n",sizeof(BITMAPFILEHEADER));  
	 printf("sizeof(BITMAPINFOHEADER) = %d\n",sizeof(BITMAPINFOHEADER));
        
	bmpsize = bmpfile.biInfo.bmiHeader.biSizeImage;
	printf("bmpsize = %d\n",bmpsize);
/* 
    char *pEachBMPBuf = (char*)malloc(bmpsize);
	char *pEachBBuf = (char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	char *pEachGBuf = (char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	char *pEachRBuf = (char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	
	memset(pEachBMPBuf, 0, bmpsize);
	memset(pEachBBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	memset(pEachGBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
	memset(pEachRBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE));
*/	
    printf("pdata start \n");
    fwrite(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);  
    fwrite(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);	
	//getlastChaosAddr(ptAddr);
	ptAddr = lastChaosAddr;
	printf("encrypt lastChaosAddr = %x \n",lastChaosAddr);
	printf("encrypt ptAddr = %x \n",ptAddr);
	printf("encrypt *ptAddr = %d \n",*((uint8_t*)ptAddr));

	//i = 0;
	//while(!feof(pf))
	for(i=0; i<bmpsize/16;i++)
	{
		memset(text, 0, sizeof(text));
		memset(cipher_text, 0, sizeof(cipher_text));
		fread(cipher_text, sizeof(cipher_text), 1, pf);
		if(i == (bmpsize/16)-1){
			printf("encrypt *((uint8_t*)ptAddr) = %d \n",*((uint8_t*)(ptAddr-i)));
		}
		setSWAP_Type(*((uint8_t*)(ptAddr-i)));
		if(Maes_decrypt_block(&ctx, text, cipher_text) != SUCCESS)
		{
			perror("Maes_decrypt_block error.");
			return -1;
		}
		fwrite(text, sizeof(text), 1, fp);
		//fwrite(text, sizeof(text), 1, fp);
		//i++;
		//ptAddr = ptAddr - 1;
		//printf("round %d\n",i);
	}	
	
    fclose(pf);
	fclose(fp);
    printf("pdata end \n");   
 

/*
		if(aes_decrypt_block(&ctx, ret_text, cipher_text) != SUCCESS)
		{
			perror("aes_decrypt_block error.");
			return -1;
		}
*/		
		printf("key_bit %d: \n", key_bit[0]);
		print("\tencrypt:  ", cipher_text);
		print("\tdecrypt  :  ", text);		
		//print("\tdecrypt:  ", ret_text);

	return 0;
}

unsigned char GenRGBbmp(const char* filename, char* RGBname)  
{  
	unsigned int bmpsize = 0;
	long pixelSum = 0;
	int i,j;
    FILE *pf = fopen(filename, "rb"); 
	FILE *fp = fopen(RGBname, "wb");	
    if(!pf)  
    {  
        printf("fopen %s failed \n",filename);  
        return 1;  
    }
    if(!fp)  
    {  
        printf("fopen %s failed \n",RGBname);  
        return 1;  
    }	
  
    BITMAPFILE bmpfile;
	memset((void *)&(bmpfile),0,sizeof(BITMAPFILE));	
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);  
    fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, pf);  
	 printf("sizeof(BITMAPFILEHEADER) = %d\n",sizeof(BITMAPFILEHEADER));  
	 printf("sizeof(BITMAPINFOHEADER) = %d\n",sizeof(BITMAPINFOHEADER));
         
	int bitCountPerPix = bmpfile.biInfo.bmiHeader.biBitCount; 
	bmpsize = bmpfile.biInfo.bmiHeader.biSizeImage;
	printf("bmpsize = %d\n",bmpsize);
	printf("bitCountPerPix = %d\n",bitCountPerPix);
	printf("BMP_RAW_1BYTE_TO_BIT = %d\n",BMP_RAW_1BYTE_TO_BIT);

    unsigned char *pEachBMPBuf = (unsigned char*)malloc((bmpfile.biInfo.bmiHeader.biWidth)*(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	unsigned char *pEachBBuf = (unsigned char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	unsigned char *pEachGBuf = (unsigned char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	unsigned char *pEachRBuf = (unsigned char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	
	memset(pEachBMPBuf, 0, (bmpfile.biInfo.bmiHeader.biWidth)*(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	memset(pEachBBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	memset(pEachGBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	memset(pEachRBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
    printf("pdata start \n");
	if(pEachBMPBuf)
	{
		i =0;
		j =0;
	//for(i = 0; i < bmpsize/3; i=i+3)
		while(!feof(pf))
		{
			fread(pEachBMPBuf, (bmpfile.biInfo.bmiHeader.biWidth)*(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, pf);		
			//printf("j=%d \n",j);
			for(i = 0; i < (bmpfile.biInfo.bmiHeader.biWidth)*(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT); i = i+3)
			{	
				*(pEachBBuf+j) = *(pEachBMPBuf+i);
				*(pEachGBuf+j) = *(pEachBMPBuf+i+1);
				*(pEachRBuf+j) = *(pEachBMPBuf+i+2);
				j++;
			}
		}
	}
	else
	{
		printf("cannot malloc pEachBMPBuf");
		return 0;
	}

	for(i = 0; i <bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT);i++)
	{
		pixelSum = pixelSum + *(pEachBBuf+i) + *(pEachGBuf+i) + *(pEachRBuf+i);
	}
	set_Pixel_sum(pixelSum);
	
	printf("2 \n");
    fwrite(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);  
    fwrite(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pEachBBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, fp);
	fwrite(pEachGBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, fp);
	fwrite(pEachRBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, fp);
	
printf("3 \n");
	free(pEachBMPBuf);
	free(pEachBBuf);
	free(pEachGBuf);
	free(pEachRBuf);
    fclose(pf);
	fclose(fp);
    printf("pdata end \n");   

    return 1;  
}

unsigned char RecoverRGBbmp(const char* filename, char* RGBname)  
{  
	unsigned int bmpsize = 0;

	int i,j;
    FILE *pf = fopen(filename, "rb"); 
	FILE *fp = fopen(RGBname, "wb");	
    if(!pf)  
    {  
        printf("fopen %s failed \n",filename);  
        return 1;  
    }
    if(!fp)  
    {  
        printf("fopen %s failed \n",RGBname);  
        return 1;  
    }	
  
    BITMAPFILE bmpfile;
	memset((void *)&(bmpfile),0,sizeof(BITMAPFILE));	
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);  
    fread(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, pf);  
	 printf("sizeof(BITMAPFILEHEADER) = %d\n",sizeof(BITMAPFILEHEADER));  
	 printf("sizeof(BITMAPINFOHEADER) = %d\n",sizeof(BITMAPINFOHEADER));
         
	int bitCountPerPix = bmpfile.biInfo.bmiHeader.biBitCount; 
	bmpsize = bmpfile.biInfo.bmiHeader.biSizeImage;
	printf("bmpsize = %d\n",bmpsize);
	printf("bitCountPerPix = %d\n",bitCountPerPix);
	printf("BMP_RAW_1BYTE_TO_BIT = %d\n",BMP_RAW_1BYTE_TO_BIT);

    unsigned char *pEachBMPBuf = (unsigned char*)malloc(bmpsize);
	unsigned char *pEachBBuf = (unsigned char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	unsigned char *pEachGBuf = (unsigned char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	unsigned char *pEachRBuf = (unsigned char*)malloc(bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	
	memset(pEachBMPBuf, 0, bmpsize);
	memset(pEachBBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	memset(pEachGBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
	memset(pEachRBuf, 0, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT));
    printf("pdata start \n");
	fread(pEachBBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, pf);
	fread(pEachGBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, pf);	
	fread(pEachRBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, pf);
			
	if(pEachBMPBuf)
	{
		i =0;
		j =0;
		for(i = 0; i < bmpsize; i=i+3)
			//while(!feof(pf))
		{	
			*(pEachBMPBuf+i) = *(pEachBBuf+j);
			*(pEachBMPBuf+i+1) = *(pEachGBuf+j);
			*(pEachBMPBuf+i+2) = *(pEachRBuf+j);
			j++;
		}
	}
	else
	{
		printf("cannot malloc pEachBMPBuf");
		return 0;
	}


    fwrite(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, fp);  
    fwrite(&(bmpfile.biInfo.bmiHeader), sizeof(BITMAPINFOHEADER), 1, fp);
	//fwrite(pEachBBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, fp);
	//fwrite(pEachGBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, fp);
	//fwrite(pEachRBuf, bmpsize/(bitCountPerPix/BMP_RAW_1BYTE_TO_BIT), 1, fp);
	fwrite(pEachBMPBuf, bmpsize, 1, fp);
	
	free(pEachBMPBuf);
	free(pEachBBuf);
	free(pEachGBuf);
	free(pEachRBuf);
    fclose(pf);
	fclose(fp);
    printf("pdata end \n");   

    return 1;  
}