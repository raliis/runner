#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "util.h"
#include "hidapi.h"
#include "parse_data.h"

#define DEFAULT_VID 0x0da4
#define DEFAULT_PID 0x0006

