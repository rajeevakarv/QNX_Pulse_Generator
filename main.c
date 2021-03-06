#include <stdio.h>
#include <unistd.h>       /* for sleep() */
#include <stdint.h>       /* for uintptr_t */
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/neutrino.h> /* for ThreadCtl() */
#include <sys/mman.h>     /* for mmap_device_io() */
#include <time.h>

/* The Neutrino IO port used here corresponds to a single register, which is
 * one byte long */
#define PORT_LENGTH 1

/* The first parallel port usually starts at 0x378. Each parallel port is
 * three bytes wide. The first byte is the Data register, the second byte is
 * the Status register, the third byte is the Control register. */
#define DATA_ADDRESS 0x378
#define CTRL_ADDRESS 0x37a
 /* bit 2 = printer initialisation (high to initialise)
  * bit 4 = hardware IRQ (high to enable) */
#define INIT_BIT 0x04

#define LOW 0x00
#define HIGH 0xFF

#define MAX_COUNT 1000

int main(int argc, char *argv[]) {
	int privity_err;
	uintptr_t ctrl_handle;
	uintptr_t data_handle;
	int count;

	struct timespec my_timer_value;
    my_timer_value.tv_nsec = 500000;

	/* Give this thread root permissions to access the hardware */
	privity_err = ThreadCtl( _NTO_TCTL_IO, NULL );
	if ( privity_err == -1 )
	{
		fprintf( stderr, "can't get root permissions\n" );
		return -1;
	}

	/* Get a handle to the parallel port's Control register */
	ctrl_handle = mmap_device_io( PORT_LENGTH, CTRL_ADDRESS );
	/* Initialise the parallel port */
	out8( ctrl_handle, INIT_BIT );

	/* Get a handle to the parallel port's Data register */
	data_handle = mmap_device_io( PORT_LENGTH, DATA_ADDRESS );

	while(1)
	{
		/* Output a byte of lows to the data lines */
		out8( data_handle, HIGH );
		nanospin( &my_timer_value );

		/* Output a byte of highs to the data lines */
		out8( data_handle, LOW );
		nanospin( &my_timer_value );
	}

	return 0;
}
