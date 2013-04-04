#ifndef _CONFIG_H_
#define _CONFIG_H_

// Features
#define SERIAL_PRINT_ENABLE 1
#define DEBUG_INTERFACE 0
#define DEBUG_SDCARD 0

#define ALL_LABELS 0

#define DISABLE_SDCARD_WRITING 0
#define DELETE_DATAFILE_ON_STARTUP 0

#define FAKE_GPS_DATA 0
#define ANTLER_LAKE 0
#define UOFA_2 0

#define MEM_DEBUG 0

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE	0x001F
#define	RED	 0xF800
#define	GREEN   0x07E0
#define CYAN	0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// GPS Setup
#define GPS_FIX_PIN 11
#define GPS_ENABLE_PIN 12
#define GPS_BAUD_RATE 4800

#endif
