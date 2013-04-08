#ifndef _GTPA010_H_
#define _GTPA010_H_

#include "Sensors.h"
#include "TinyGPS.h"
#include "TimerThree.h"


// GPS Setup
#define GPS_FIX_PIN 11
#define GPS_ENABLE_PIN 12
#define GPS_BAUD_RATE 4800

#define FAKE_GPS_DATA 1

typedef struct gpsData
{
	long int lat; // Stores the latitude as a long int as specified by the TinyGPS librairy
	long int lon; // Stores the longitude as a long int as specified by the TinyGPS librairy
	unsigned long int age; // Checksum information
	
	// Date/Time information
	int year;
	byte month;
	byte day;
	byte hour;
	byte minute;
	byte second;
	byte hundredths;
};

class GTPA010 : Sensors
{
public:
	static void printData(); // Designed to print the data onto serial
	static void readData(); // Executes the reading procedures from the sensor
	static gpsData* getData(); // Gets the address of the data struct
	
	#if FAKE_GPS_DATA
	static void fakeData(); // Used in testing, designed to replicate GPS data for the purpose of validating other functions, not included in production class, defined in Config
	#endif
	
	static void begin(); // Begin sensor initilization routines
	static bool check(); // A validity check for the sensor, if true, its reporting a valid response, if false, then response is invalid
	
	static void gpsCheck(); // A function called by an inturput service to check GPS lock status

	static volatile bool gpsLock; // The indicator for a valid lock against a 2D/3D lock
private:
	static gpsData data; // Used to store the data in the gpsData struct
	static bool newData; // A private variable determining if the data has been refreshed
	static const long int timer_ticks = 1000000; // The length of ticks inbetween checks of the GPS, dependant on the devices clock speed
	static char dataBuffer; // Char buffer for the inputed data

	static volatile bool gpsValue; // The value read from the GPS 2D/3D fix pin
};

#endif
