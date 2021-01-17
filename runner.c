#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "usb/runner_hid.h"

int main (int argc, char** argv)
{
	int c;                              // holds options
	int flag = 0;                       // holds selected features
	
	// get parameters
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while ((c = getopt (argc, argv, "adgn:")) != -1) 
	{
		switch (c)
		{
			case 'a':
				flag |= 0x01;
				break;

			case 'd':
				flag |= 0x02;
				break;

			case 'g':
				flag |= 0x04;
				break;

			case 'n':
				flag |= 0x08;
				break;

			default:
				//usage (argv[0]);
				break;
		}
	}
    printf ("Flags: %d\n", flag);

	

    return 0;
}