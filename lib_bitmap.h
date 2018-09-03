#ifndef _LI_BITMAP_H_
#define _LI_BITMAP_H_
 
#pragma pack(push, 1)

//typedef unsigned char  unsigned char;
//typedef unsigned short unsigned short;
//typedef unsigned int   unsigned int;
int GenBmpFile(unsigned char *pData, const char *filename);
unsigned char GetBmpData(unsigned char *bitCountPerPix, unsigned int *width, unsigned int *height, const char* filename, unsigned long *memory_vip_frame);
//zxf unsigned char* GetBmpData();
void FreeBmpData(unsigned char *pdata);
unsigned char StoreBmpData(unsigned char *bitCountPerPix, unsigned int *width, unsigned int *height, const char* filename,unsigned long *memory_vip_frame); 

typedef struct tagBITMAPFILEHEADER
{
  unsigned short bfType;
 unsigned int bfSize;
 unsigned short bfReserved1;
 unsigned short bfReserved2;
 unsigned int bfOffBits;
} BITMAPFILEHEADER;
 
typedef struct tagBITMAPINFOHEADER
{
 unsigned int biSize;
 unsigned int biWidth;
 unsigned int biHeight;
 unsigned short biPlanes;
 unsigned short biBitCount;
 unsigned int biCompression;
 unsigned int biSizeImage;
 unsigned int biXPelsPerMeter;
 unsigned int biYPelsPerMeter;
 unsigned int biClrUsed;
 unsigned int biClrImportant;
} BITMAPINFOHEADER;
 
typedef struct tagRGBQUAD
{
 unsigned char rgbBlue;
 unsigned char rgbGreen;
 unsigned char rgbRed;
 unsigned char rgbReserved;
} RGBQUAD;
 
typedef struct tagBITMAPINFO
{
 BITMAPINFOHEADER bmiHeader;
 RGBQUAD bmiColors[1];
} BITMAPINFO;
 
 
typedef struct tagBITMAP
{
 BITMAPFILEHEADER bfHeader;
 BITMAPINFO biInfo;
}BITMAPFILE;
 
#pragma pack(pop)
 
#endif
