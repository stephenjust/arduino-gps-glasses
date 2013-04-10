#include "GTPA010.h"

#include "Sensors.h"
#include "TinyGPS.h"
#include "TimerThree.h"


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
        Timer3.initialize(timer_ticks);
	digitalWrite(GPS_ENABLE_PIN,HIGH);
	Timer3.attachInterrupt(&gpsCheck);
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
#if FAKE_GPS_DATA
    GTPA010::fakeData();
    return;
#endif
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
			
	} else {
#ifdef DEBUG_GPS
            Serial.println("No lock!");
#endif
        }
}

#if FAKE_GPS_DATA
#import "gps_fake_data.h"

int tstart_time = 0;

/**
 * Provide some fake data in case we are demoing in a location
 * where it is not possible to get a GPS fix
 */
void GTPA010::fakeData() {
    // Set the start time of the path
    if (!tstart_time) {
        tstart_time = Sensors::getTime();
    }

    // Calculate how far into the path we are
    int index = (Sensors::getTime() - tstart_time) % fd_len;

    // Set the fake data
    data.lat = fd_lat[index];
    data.lon = fd_lon[index];

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
