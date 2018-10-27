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


int genChaos() {
	FILE *fp;
	int num, stop;
	float shift;
	unsigned u;
	void *virtual_base;
	int fd;
	float x,y,z,w;

	void *h2p_lw_chaos_done_addr, *h2p_lw_chaos_reset_addr, /**h2p_lw_chaos_shift_addr,*/ *h2p_lw_chaos_step_addr;
	void *h2p_lw_chaos_x_addr, *h2p_lw_chaos_y_addr, *h2p_lw_chaos_z_addr, *h2p_lw_chaos_w_addr;
	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	h2p_lw_chaos_step_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_STEP_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_chaos_done_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_DONE_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_chaos_reset_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_RESET_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	//h2p_lw_chaos_shift_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_SHIFT_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_chaos_x_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_X_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_chaos_y_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_Y_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_chaos_z_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_Z_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_chaos_w_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + PIO_CHAOS_W_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	x = y = z = w = stop = 1;
	fp = fopen("test.txt","wb");		
	if( NULL == fp)
	{
		printf(" open file fail");
		return 1;
	}
	fprintf(fp,"%1.7f %1.7f %1.7f %1.7f\r\n",x,y,z,w);		
while(1) {
	
		printf("h2p_lw_chaos_step_addr = %x\n",h2p_lw_chaos_step_addr);
		printf("h2p_lw_chaos_done_addr = %x\n",h2p_lw_chaos_done_addr);
		printf("h2p_lw_chaos_reset_addr = %x\n",h2p_lw_chaos_reset_addr);
		//printf("h2p_lw_chaos_shift_addr = %x\n",h2p_lw_chaos_shift_addr);
		printf("h2p_lw_chaos_x_addr = %x\n",h2p_lw_chaos_x_addr);
		printf("h2p_lw_chaos_y_addr = %x\n",h2p_lw_chaos_y_addr);
		printf("h2p_lw_chaos_z_addr = %x\n",h2p_lw_chaos_z_addr);
		printf("h2p_lw_chaos_w_addr = %x\n",h2p_lw_chaos_w_addr);
		// 檢視 Chaos_Key Generator 的狀態
		printf("step %d\n", *(int*)h2p_lw_chaos_step_addr);
		printf("reset %d\n", *(int*)h2p_lw_chaos_reset_addr);
		//printf("shift %X\n", *(int*)h2p_lw_chaos_shift_addr);
		printf("done %d\n", *(int*)h2p_lw_chaos_done_addr);
		printf("x %1.7f\n", *(float*)h2p_lw_chaos_x_addr);
		printf("y %1.7f\n", *(float*)h2p_lw_chaos_y_addr);
		printf("z %1.7f\n", *(float*)h2p_lw_chaos_z_addr);
		printf("w %1.7f\n", *(float*)h2p_lw_chaos_w_addr);
		printf("Entry 0 to stop\n");
		scanf("%d", &stop);
		if(stop == 0)
			break;
		
		// 取得要計算的 STEP 次數，以及 SHIFT 值
		printf("Insert Number of Chaos Key Steps\n");
		scanf("%d", &num);
		//printf("Insert Shift of Chaos Key\n");
		//scanf("%f", &shift);
		shift = 1;
		// 將 SHFIT 的 Float 值轉為 16 進制值使 Chaos_Key Generator 可以辨識
		//u = *(unsigned*)&shift;
		//printf("shift hex = %x\n", u);
		// 重置 Chaos_Key Generator 的狀態
		*(uint8_t *)h2p_lw_chaos_step_addr = 0;
		*(uint8_t *)h2p_lw_chaos_reset_addr = 1;
		*(uint8_t *)h2p_lw_chaos_reset_addr = 0;
		// 檢視 Chaos_Key Generator 的狀態
		printf("step %d\n", *(int*)h2p_lw_chaos_step_addr);
		printf("reset %d\n", *(int*)h2p_lw_chaos_reset_addr);
		printf("done %d\n", *(int*)h2p_lw_chaos_done_addr);
		// 向 Chaos_Key Generator 指定 SHIFT 值
		//*(uint8_t *)h2p_lw_chaos_shift_addr = u;
		// 檢視指定給 Chaos_Key Generator 的 SHIFT 值是否正確
		//printf("shift %x\n", *(unsigned*)h2p_lw_chaos_shift_addr);
		// 啟動 Chaos_Key Generator
		*(uint8_t *)h2p_lw_chaos_step_addr = 1;
		printf("start step %d\n", *(int*)h2p_lw_chaos_step_addr);
		// 創建愈接收數量金鑰的動態陣列
		//alt_u8 *list = malloc(num * sizeof(alt_u8));
		int i = 0;


		// 循環啟動 Chaos_Key Generator 運算金鑰，並存入動態陣列
		while(1) {
			//printf("enter while i = %d\n",i);
			if(*(int*)h2p_lw_chaos_done_addr == 0)
				continue;
			x = *(float*)h2p_lw_chaos_x_addr;
//			i++;
//			if(i == num)
//				break;
			y = *(float*)h2p_lw_chaos_y_addr;
//			i++;
//			if(i == num)
//				break;			
			z = *(float*)h2p_lw_chaos_z_addr;
//			i++;
//			if(i == num)
//				break;
			w = *(float*)h2p_lw_chaos_w_addr;
			printf("round %d\n",i);
			i++;
			if(i == num)
				break;
			printf("x = %1.7f \n", x);
			printf("y = %1.7f \n", y);
			printf("z = %1.7f \n", z);
			printf("w = %1.7f \n", w);	
			fprintf(fp,"%1.7f %1.7f %1.7f %1.7f\r\n",x,y,z,w);			
			/*
			list[i] = IORD_ALTERA_AVALON_PIO_DATA(PIO_CHAOS_X_BASE);
			i ++;
			if(i == num)
				break;
			list[i] = IORD_ALTERA_AVALON_PIO_DATA(PIO_CHAOS_Y_BASE);
			i ++;
			if(i == num)
				break;
			list[i] = IORD_ALTERA_AVALON_PIO_DATA(PIO_CHAOS_Z_BASE);
			i ++;
			if(i == num)
				break;
			list[i] = IORD_ALTERA_AVALON_PIO_DATA(PIO_CHAOS_W_BASE);
			i ++;
			if(i == num)
				break;
			*/
			*(uint8_t *)h2p_lw_chaos_step_addr = 0;
			*(uint8_t *)h2p_lw_chaos_step_addr = 1;

		}
		*(uint8_t *)h2p_lw_chaos_step_addr = 0;
		printf("done\n");
		// 列出最後 4 個金鑰
		//printf("%u, ", list[num-4]);
		//printf("%u, ", list[num-3]);
		//printf("%u, ", list[num-2]);
		//printf("%u\n", list[num-1]);
		//free(list);
	}
	

	// clean up our memory mapping and exit
	
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	fclose(fp);
	close( fd );

	return( 0 );
}
