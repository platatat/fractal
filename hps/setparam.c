///////////////////////////////////////
/// Audio
/// compile with
/// gcc media_brl4_7_audio.c -o testA -lm -O3
/// works up to about drum size 30 or so for NO multiplies case
/// 
///////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>
// interprocess comm
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/mman.h>
#include <time.h>
// network stuff
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "address_map_arm_brl4.h"

// fixed point
#define float2fix30(a) ((int)((a)*1073741824)) // 2^30

#define SWAP(X,Y) do{int temp=X; X=Y; Y=temp;}while(0) 

/* function prototypes */
void VGA_text (int, int, char *);
void VGA_box (int, int, int, int, short);
void VGA_line(int, int, int, int, short) ;

// virtual to real address pointers

volatile unsigned int * red_LED_ptr = NULL ;
//volatile unsigned int * res_reg_ptr = NULL ;
//volatile unsigned int * stat_reg_ptr = NULL ;

// audio stuff
volatile unsigned int * audio_base_ptr = NULL ;
volatile unsigned int * audio_fifo_data_ptr = NULL ; //4bytes
volatile unsigned int * audio_left_data_ptr = NULL ; //8bytes
volatile unsigned int * audio_right_data_ptr = NULL ; //12bytes
// phase accumulator
// drum-specific multiply macros simulated by shifts
#define times0pt5(a) ((a)>>1) 
#define times0pt25(a) ((a)>>2) 
#define times2pt0(a) ((a)<<1) 
#define times4pt0(a) ((a)<<2) 
#define times0pt9998(a) ((a)-((a)>>12)) //>>10
#define times0pt9999(a) ((a)-((a)>>13)) //>>10
#define times0pt999(a) ((a)-((a)>>10)) //>>10
#define times_rho(a) (((a)>>4)) //>>2

// drum size paramenters
// drum will FAIL if size is too big
#define drum_size 30
#define drum_middle 15
int copy_size = drum_size*drum_size*4 ;

// fixed pt macros suitable for 32-bit sound
typedef signed int fix28 ;
//multiply two fixed 4:28
#define multfix28(a,b) ((fix28)(((( signed long long)(a))*(( signed long long)(b)))>>28)) 
//#define multfix28(a,b) ((fix28)((( ((short)((a)>>17)) * ((short)((b)>>17)) )))) 
#define float2fix28(a) ((fix28)((a)*268435456.0f)) // 2^28
#define fix2float28(a) ((float)(a)/268435456.0f) 
#define int2fix28(a) ((a)<<28)
#define fix2int28(a) ((a)>>28)
// shift fraction to 32-bit sound
#define fix2audio28(a) (a<<4)

// some fixed point values
#define FOURfix28 0x40000000 
#define SIXTEENTHfix28 0x01000000
#define ONEfix28 0x10000000

// drum state variable arrays
fix28 drum_n[drum_size][drum_size] ;
// drup amp at last time
fix28 drum_n_1[drum_size][drum_size] ;
// drum updata
fix28 new_drum[drum_size][drum_size] ;
fix28 new_drum_temp ;

clock_t note_time ;

// the light weight buss base
void *h2p_lw_virtual_base;

// pixel buffer
volatile unsigned int * vga_pixel_ptr = NULL ;
void *vga_pixel_virtual_base;

// character buffer
volatile unsigned int * vga_char_ptr = NULL ;
void *vga_char_virtual_base;

// /dev/mem file descriptor
int fd;

// shared memory 
key_t mem_key=0xf0;
int shared_mem_id; 
int *shared_ptr;
int audio_time;

// width of gaussian initial condition
float alpha = 64;
 
int main(void)
{
	// Declare volatile pointers to I/O registers (volatile 	// means that IO load and store instructions will be used 	// to access these pointer locations, 
	// instead of regular memory loads and stores) 
	
	// === need to mmap: =======================
	// FPGA_CHAR_BASE
	// FPGA_ONCHIP_BASE      
	// HW_REGS_BASE        
  
	// === get FPGA addresses ==================
    // Open /dev/mem
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) 	{
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

    volatile int* start_x = h2p_lw_virtual_base + 0x00;
    volatile int* start_y = h2p_lw_virtual_base + 0x10;

    volatile int* dx = h2p_lw_virtual_base + 0x20;
    volatile int* dy = h2p_lw_virtual_base + 0x30;

    volatile int* reset = h2p_lw_virtual_base + 0x40;

    *start_x = -(2 << 20);
    *start_y = -(1 << 20);
    *dx = 4915;
    *dy = 4369;

} // end main

