#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>

#include "address_map_arm_brl4.h"

int main(int argc, char** argv)
{
	// === get FPGA addresses ==================
    // Open /dev/mem
    int fd_mem;
	if( ( fd_mem = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) 	{
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}
    
    // get virtual addr that maps to physical
	volatile void* h2p_lw_virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd_mem, HW_REGS_BASE );	
	if( h2p_lw_virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap1() failed...\n" );
		close( fd_mem );
		return(1);
	}

    volatile int* start_x = h2p_lw_virtual_base + 0x00;
    volatile int* start_y = h2p_lw_virtual_base + 0x10;

    volatile int* dx = h2p_lw_virtual_base + 0x20;
    volatile int* dy = h2p_lw_virtual_base + 0x30;

    volatile int* reset = h2p_lw_virtual_base + 0x40;
    volatile int* done = h2p_lw_virtual_base + 0x50;

    const int init_dxy = 4915;

    int screen_x = 0;
    int screen_y = 0;
    int screen_dxy = init_dxy;

    //// Mouse things

    int fd, bytes;
    unsigned char data[3];

    const char *pDevice = "/dev/input/mice";

    // Open Mouse
    fd = open(pDevice, O_RDWR);
    if(fd == -1)
    {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }

    bool prev_left = false;
    bool prev_right = false;
    bool prev_middle = false;
    bool left, middle, right;
    signed char x, y;
    while(1)
    {
        // Write to the screen
        *start_x = screen_x - screen_dxy * (640 / 2);
        *start_y = screen_y - screen_dxy * (480 / 2);
        *dx = screen_dxy;
        *dy = screen_dxy;

        *reset = 1;
        *reset = 0;

        while (!*done){}

        // Read Mouse     
        bytes = read(fd, data, sizeof(data));

        if (bytes > 0) {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = data[1];
            y = data[2];
            printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);

            screen_x += -x * screen_dxy;
            screen_y +=  y * screen_dxy;

            if (left && !prev_left) {
                screen_dxy /= 2;

                if (screen_dxy <= 0) {
                    screen_dxy = 1;
                }
            }

            if (right && !prev_right) {
                screen_dxy *= 2;
            }

            if (middle && !prev_middle) {
                screen_x = 0;
                screen_y = 0;
                screen_dxy = init_dxy;
            }

            prev_left = left;
            prev_right = right;
            prev_middle = middle;
        }   
    }
    return 0; 
}
