///////////////////////////////////////
/// 640x480 version! 16-bit color
/// This code will segfault the original
/// DE1 computer
/// compile with
/// gcc graphics_video_16bit.c -o gr -O2 -lm
///
///////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/mman.h>
#include <sys/time.h> 
#include <math.h>
#include <cairo/cairo.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
//#include "address_map_arm_brl4.h"

// video display
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define SDRAM_SPAN              0x04000000
// characters
#define FPGA_CHAR_BASE        0xC9000000 
#define FPGA_CHAR_END         0xC9001FFF
#define FPGA_CHAR_SPAN        0x00002000
/* Cyclone V FPGA devices */
#define HW_REGS_BASE          0xff200000
//#define HW_REGS_SPAN        0x00200000
#define HW_REGS_SPAN          0x00005000

// the light weight buss base
void *h2p_lw_virtual_base;

// pixel buffer
void *vga_pixel_virtual_base;

// character buffer
volatile unsigned int * vga_char_ptr = NULL;
void *vga_char_virtual_base;

// /dev/mem file id
int fd;

int main(void)
{
      
    // === need to mmap: =======================
    // FPGA_CHAR_BASE
    // FPGA_ONCHIP_BASE      
    // HW_REGS_BASE        
  
    // === get FPGA addresses ==================
    // Open /dev/mem
    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 )     {
        printf( "ERROR: could not open \"/dev/mem\"...\n" );
        return( 1 );
    }
    
    // get virtual addr that maps to physical
    h2p_lw_virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );    
    if( h2p_lw_virtual_base == MAP_FAILED ) {
        printf( "ERROR: mmap1() failed...\n" );
        close( fd );
        return(1);
    }
    
/*
    // === get VGA char addr =====================
    // get virtual addr that maps to physical
    vga_char_virtual_base = mmap( NULL, FPGA_CHAR_SPAN, (     PROT_READ | PROT_WRITE ), MAP_SHARED, fd, FPGA_CHAR_BASE );    
    if( vga_char_virtual_base == MAP_FAILED ) {
        printf( "ERROR: mmap2() failed...\n" );
        close( fd );
        return(1);
    }
    
    // Get the address that maps to the FPGA LED control 
    vga_char_ptr =(unsigned int *)(vga_char_virtual_base);
*/

    // === get VGA pixel addr ====================
    // get virtual addr that maps to physical
    vga_pixel_virtual_base = mmap( NULL, SDRAM_SPAN + 0x1000, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, SDRAM_BASE);    
    if( vga_pixel_virtual_base == MAP_FAILED ) {
        printf( "ERROR: mmap3() failed...\n" );
        close( fd );
        return(1);
    }
    
    // Get the address that maps to the FPGA pixel buffer
    volatile signed short * out_pixel_ptr = (unsigned short *)(vga_pixel_virtual_base);
    volatile uint32_t * fifo_ptr = (unsigned char *)(vga_pixel_virtual_base) + SDRAM_SPAN;

    *out_pixel_ptr = -2;

    // Write our data to the fifo

    *fifo_ptr = (0x00 << 29) | 0; // Output address
    *fifo_ptr = (0x00 << 29) | 0; // Output address
    *fifo_ptr = (0x00 << 29) | 0; // Output address
    *fifo_ptr = (0x01 << 29) | 2; // Zoom level

    // c_real
    *fifo_ptr = (0x02 << 29) | 1;
    *fifo_ptr = (0x02 << 29) | 0;
    *fifo_ptr = (0x02 << 29) | 0;

    // c_im
    *fifo_ptr = (0x03 << 29) | 1;
    *fifo_ptr = (0x03 << 29) | 0;
    *fifo_ptr = (0x03 << 29) | 0;

    // Start computation
    *fifo_ptr = (0x04 << 29);

    // Wait for data
    while (*out_pixel_ptr == -2) {
        printf("Waiting for output %i\n", *out_pixel_ptr);
        sleep(1);
    }

    for (int i = 0; i < 0x10; i++) {
        printf("%i\n", out_pixel_ptr[i]);
    }
} // end main
