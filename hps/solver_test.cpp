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

#include <vector>

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

const uint32_t TILE_SIZE = 64;

int main(void)
{
    // Open /dev/mem
    int fd = open( "/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)     {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }

    // mmap the bus
    void* h2f_base_address = mmap(NULL, SDRAM_SPAN + 0x1000,
                                  PROT_READ | PROT_WRITE, MAP_SHARED,
                                  fd, SDRAM_BASE);
    if (h2f_base_address == MAP_FAILED) {
        printf("ERROR: mmap3() failed...\n");
        close(fd);
        return 1;
    }

    // Addresses on the bus
    volatile int16_t* out_pixel_ptr = (int16_t*)(h2f_base_address);
    volatile uint32_t* fifo_ptr = (uint32_t*) ((uint8_t*)(h2f_base_address) + SDRAM_SPAN);
    volatile uint32_t* fifo_control_ptr = fifo_ptr + 1;
    const uint32_t fifo_start = 0x01;
    const uint32_t fifo_end = 0x02;

    volatile int16_t* out_last_pixel = out_pixel_ptr + TILE_SIZE * TILE_SIZE - 1;

    // Write a sentinel into the last possible output value
    *out_last_pixel = -2;

    // Generate data for the FIFO
    const uint32_t BITST_OUT_ADDR   = (0x00 << 29);
    const uint32_t BITST_NUM_LIMBS  = (0x01 << 29);
    const uint32_t BITST_C_REAL     = (0x02 << 29);
    const uint32_t BITST_C_IMAG     = (0x03 << 29);
    const uint32_t BITST_MAX_ITER   = (0x04 << 29);
    const uint32_t BITST_LOC_SHIFT  = (0x05 << 29);

    std::vector<uint32_t> fifo_data;

    fifo_data.push_back(BITST_OUT_ADDR  | 0);
    fifo_data.push_back(BITST_NUM_LIMBS | 2);

    fifo_data.push_back(BITST_C_REAL | 0);
    fifo_data.push_back(BITST_C_REAL | 0);

    fifo_data.push_back(BITST_C_IMAG | 0);
    fifo_data.push_back(BITST_C_IMAG | 0);

    fifo_data.push_back(BITST_MAX_ITER | 16);

    // This is the number of bits to offset the x/y value of the pixel,
    // as measured from the right hand edge of the last limb.
    fifo_data.push_back(BITST_LOC_SHIFT | (27-5));

    // Write our data to the fifo
    *fifo_control_ptr = fifo_start;

    for (int i = 0; i < fifo_data.size(); i++) {
        if (i == fifo_data.size() - 1) {
            *fifo_control_ptr = fifo_end;
        }

        *fifo_ptr = fifo_data[i];
    }

    // Wait for data
    while (*out_last_pixel == -2) {
        printf("Waiting for output\n");

        struct timespec wait, out;
        wait.tv_sec = 0;
        wait.tv_nsec = 100000;
        nanosleep(&wait, &out);
    }

    for (int i = 0; i < TILE_SIZE; i++) {
        for (int j = 0; j < TILE_SIZE; j++) {
            printf("%i,", out_pixel_ptr[i * TILE_SIZE + j]);
        }
        printf("\n");
    }
}
