#ifndef _GTPA010_H_
#define _GTPA010_H_

#include "../Sensors.h"

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
private:
	static gpsData data; // Used to store the data in the gpsData struct
	static bool newData; // A private variable determining if the data has been refreshed
	static const long int timer_ticks = 1000000; // The length of ticks inbetween checks of the GPS, dependant on the devices clock speed
	static char dataBuffer; // Char buffer for the inputed data

	static volatile bool gpsValue; // The value read from the GPS 2D/3D fix pin
	static volatile bool gpsLock; // The indicator for a valid lock against a 2D/3D lock
};


bool GTPA010::newData = 0;

/* Static variable declarations */
char GTPA010::dataBuffer;
gpsData GTPA010::data;

// Inturupt values
volatile bool GTPA010::gpsValue = -1;
volatile bool GTPA010::gpsLock = 0;

gpsData* GTPA010::getData()
{
	return &data; // Returns the data pointer
}

bool GTPA010::check()
{
	return newData; // Returns the status of newData
}

void GTPA010::begin()
{
	// Start GPS - Defines in classes/Config.h
	Serial2.begin(GPS_BAUD_RATE);
	pinMode(GPS_FIX_PIN,INPUT);
	pinMode(GPS_ENABLE_PIN,OUTPUT);
	
	// Setup GPS Interrupt check - Defines in classes/Config.h
	Timer3.initialize();
	digitalWrite(GPS_ENABLE_PIN,HIGH);
	Timer3.attachInterrupt(&gpsCheck, timer_ticks);
}

void GTPA010::gpsCheck()
{
	bool oldGpsValue = gpsValue; // check the old value for the 2D/3D lock pin
	
	gpsValue = digitalRead(GPS_FIX_PIN); // refresh value with a digitalRead
	
	if(!(oldGpsValue ^ gpsValue)) // Does the {new,old} value both 0?
		gpsLock = 1; // Indicate a lock is present
	else
		gpsLock = 0; // Clear the lock variable
	
	sensorSecond = !sensorSecond; // Toggle the static sensor class variable for use of other sensors
}

void GTPA010::readData()
{
	if(gpsLock) // If we have a lock
	{
		while (Serial2.available()) // And while the Serial. is available
		{
			dataBuffer = Serial2.read(); // Read the available data
				
			if (gps.encode(dataBuffer)) // Did a new valid sentence come in?
				newData = true; // Notify the class that there is new data to show!
		}
		
		gps.get_position(&data.lat, &data.lon, &data.age); // Parse the position data
		gps.crack_datetime(&data.year, &data.month, &data.day, &data.hour, &data.minute, &data.second, &data.hundredths, &data.age); // Parse the time data
		
		if(data.lat == TinyGPS::GPS_INVALID_ANGLE || data.lon == TinyGPS::GPS_INVALID_ANGLE || gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES || gps.hdop() == TinyGPS::GPS_INVALID_HDOP) // If any component of this data is invalid, then cancel the new data decleration, and if enabled, print that the data is invalid across the serial
		{
			newData = false;
			
			#if SERIAL_PRINT_ENABLE
			Serial.println("GPS DATA INVALID");
			#endif
		}
			
	}
}

#if FAKE_GPS_DATA
void GTPA010::fakeData()
{
	// Fake the data
	#if ANTLER_LAKE
	data.lat = 5348435;
	data.lon = -11298005 - Sensors::getTime() * 3;
	#else
	data.lat = 5352712;
	data.lon = -11352953 - Sensors::getTime() * 3;
	#endif
	
	newData = true;
}
#endif

void GTPA010::printData()
{
	// Print the data via serial
	
	Serial.print("LAT=");
		Serial.print(data.lat == TinyGPS::GPS_INVALID_ANGLE ? NAN : data.lat);
	Serial.print(" LON=");
		Serial.print(data.lon == TinyGPS::GPS_INVALID_ANGLE ? NAN : data.lon);
	Serial.print(" SAT=");
		Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? NAN : gps.satellites());
	Serial.print(" PREC=");
		Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? NAN : gps.hdop());
	  
	Serial.print(" DATE=");
	  
	if (data.age == TinyGPS::GPS_INVALID_AGE)
		Serial.println("*******    *******    ");
	else
	{
		char sz[32];
		sprintf(sz, "%02d,%02d,%02d,%02d:%02d:%02d", data.month, data.day, data.year, data.hour, data.minute, data.second);
		Serial.println(sz);
	}
}

#endif