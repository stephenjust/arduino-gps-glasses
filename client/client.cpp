#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ST7735.h> 
#include <SD.h>
#include <mem_syms.h>

#include "TimerThree.h"
#include "Sensors.h"
#include "TinyGPS.h"
#include "GTPA010.h"
#include "LSM303.h"

#include "joystick.h"
#include "map.h"
#include "path.h"
#include "serial_handling.h"
#include "ledon.h"

// #define DEBUG_SCROLLING
// #define DEBUG_PATH
// #define DEBUG_MEMORY

TinyGPS gps;
LSM303 compass;

// the pins used to connect to the AdaFruit display
const uint8_t sd_cs = 5;
const uint8_t tft_cs = 6;
const uint8_t tft_dc = 7;
const uint8_t tft_rst = 8;    

// forward function declarations
void initialize_sd_card();
void initialize_screen();
void initialize_joystick();
uint8_t process_joystick(int16_t *dx, int16_t *dy);
void status_msg(char *msg);
void clear_status_msg();

// Interrupt routines for zooming in and out.
void handle_zoom_in();
void handle_zoom_out();

// global state variables

// globally accessible screen
Adafruit_ST7735 tft = Adafruit_ST7735(tft_cs, tft_dc, tft_rst);

// Map number (zoom level) currently selected.
extern uint8_t current_map_num;

// First time flag for loop, set to cause actions for the first time only
uint8_t first_time;

void setup() {
    Sensors::Sensors();
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.flush();    // There can be nasty leftover bits.

    GTPA010::begin();
    Serial.println("GPS initialized!");

    compass.init(LSM303DLH_DEVICE);
    compass.enableDefault();
    compass.setMagGain(LSM303::magGain_47);
    Serial.println("Compass initialized!");

    compass.read();
#ifdef DEBUG
    if (!compass.timeoutOccurred()) {
        Serial.print("Compass heading: ");
        Serial.println(compass.heading());
    } else {
        Serial.println("Timed out!"); 
    }
#endif

#ifdef GLASSES_DEBUG
    while (1) {
        compass.reaqd();
        map_to_glasses(compass.heading());
    }
#endif

    initialize_screen();

    initialize_sd_card();

    initialize_joystick();

    initialize_map();

    // Want to start viewing window in the center of the map
    move_window(-11353058,5352706);

    // with cursor in the middle of the window
    move_cursor_to(
                   screen_map_x + display_window_width / 2, 
                   screen_map_y + display_window_height / 2);

    // Draw the initial screen and cursor
    first_time = 1;

#ifdef DEBUG_MEMORY
    Serial.print("Available mem:");
    Serial.println(AVAIL_MEM);
#endif
}

const uint16_t screen_scroll_delta = 64;
const uint16_t screen_left_margin = 10;
const uint16_t screen_right_margin = 117;
const uint16_t screen_top_margin = 10;
const uint16_t screen_bottom_margin = 117;

// the path request, start and stop lat and lon
uint8_t request_state = 0;  // 0 - wait for start, 1 - wait for stop point
int32_t start_lat;
int32_t start_lon;
int32_t stop_lat;
int32_t stop_lon;

uint32_t g_lat;
uint32_t g_lon;

// the most recent path to display, length = 0 means no path
uint16_t path_length = 0;
coord_t *path;


char * prev_loc_msg = 0;
/**
 * Print the current GPS position to the LCD
 */
void pos_msg(char * msg) {
    if (prev_loc_msg != msg) {
        prev_loc_msg = msg;
        
        // Draw background
        tft.fillRect(0, 136, 128, 12, WHITE);
        
        // Set text options
        tft.setTextSize(1);
        tft.setTextColor(BLACK);
        tft.setCursor(0, 138);

        // Draw text
        tft.println(msg);
    }
}

/**
 * Redraw the screen
 */
void refresh_display() {
#ifdef DEBUG_SCROLLING
    Serial.println("Screen update");
    Serial.print(current_map_num);
    Serial.print(" ");
    Serial.print(cursor_lon);
    Serial.print(" ");
    Serial.print(cursor_lat);
    Serial.println();
#endif

    draw_map_screen();
    draw_cursor();

    // Need to redraw any other things that are on the screen
    if ( path_length > 0 ) {
        draw_path(path_length, path);
    }

    // force a redisplay of status message
    clear_status_msg();
    draw_compass();
    draw_gps_dot();

    char * pos_str = 0;
#if FAKE_GPS_DATA
    pos_str = "USING FAKE DATA";
#else
    if (GTPA010::gpsLock)
        pos_str = "USING GPS COORDS";
    else
        pos_str = "SEARCHING SATELLITES";
#endif
    pos_msg("");
    pos_msg(pos_str);
}

/**
 * Send a path request to the server and wait for a response
 */
void query_path(int32_t s_lat, int32_t s_lon, int32_t e_lat, int32_t e_lon) {

    // send out the start and stop coordinates to the server
    Serial.print(s_lat);
    Serial.print(" "); 
    Serial.print(s_lon);
    Serial.print(" "); 
    Serial.print(e_lat);
    Serial.print(" "); 
    Serial.print(e_lon);
    Serial.println();

    // free any existing path
    if ( path_length > 0 ) {
        free(path);
    }

    // read the path from the serial port
    status_msg("WAITING");
    if ( read_path(&path_length, &path) ) {
#ifdef DEBUG_PATH
        uint8_t is_visible;
        for (uint16_t i=0; i < path_length; i++) {
            is_visible = is_coord_visible(path[i]);
            Serial.print(i);
            Serial.print(": ");
            Serial.print(path[i].lat);
            Serial.print(",");
            Serial.print(path[i].lon);
            Serial.print(is_visible ? "V": "");
            Serial.println();
        }
#endif
    } else {
        // should display this error on the screen
        pos_msg("Path error!");
    }
    refresh_display();
}

        int path_time = 0;

void loop() {

    // Make sure we don't update the map tile on screen when we don't need to!
    uint8_t update_display_window = 0;

    if ( first_time ) {
        first_time = 0;
        update_display_window = 1;
    }

    // Joystick displacement.
    int16_t dx = 0;
    int16_t dy = 0;
    uint8_t select_button_event = 0;

    // Update glasses heading
    if (path_length > 0) {
        //Serial.print("Compass: ");
        //Serial.println(compass.heading());
        compass.read();
        map_to_glasses((int)(target_dir - compass.heading()) % 360);
    }

    // See if the joystick has moved, in which case we want to
    // also want to move the visible cursor on the screen.

    // Process joystick input.
    select_button_event = process_joystick(&dx, &dy);

    // the joystick routine filters out small changes, so anything non-0
    // is a real movement
    if ( abs(dx) > 0 || abs(dy) > 0 ) {
        // Is the cursor getting near the edge of the screen?  If so
        // then scroll the map over by re-centering the window.

        uint16_t new_screen_map_x = screen_map_x;
        uint16_t new_screen_map_y = screen_map_y;
        uint8_t need_to_move = 0;

        uint16_t cursor_screen_x;
        uint16_t cursor_screen_y;
        if ( get_cursor_screen_x_y(&cursor_screen_x, &cursor_screen_y) ) {
            // if the cursor is visible, then adjust the display to 
            // to scroll if near the edge.

            if ( cursor_screen_x < screen_left_margin ) {
                new_screen_map_x = screen_map_x - screen_scroll_delta;
                need_to_move = 1;
            }
            else if ( cursor_screen_x > screen_right_margin ) {
                new_screen_map_x = screen_map_x + screen_scroll_delta;
                need_to_move = 1;
            }

            if ( cursor_screen_y < screen_top_margin ) {
                new_screen_map_y = screen_map_y - screen_scroll_delta;
                need_to_move = 1;
            }
            else if ( cursor_screen_y > screen_bottom_margin ) {
                new_screen_map_y = screen_map_y + screen_scroll_delta;
                need_to_move = 1;
            }

            if ( need_to_move ) {
                // move the display window, leaving cursor at same lat-lon
                move_window_to(new_screen_map_x, new_screen_map_y);
                update_display_window = 1;
            } 
            else {
                // erase old cursor, move, and draw new one, no need to 
                // redraw the underlying map tile
                erase_cursor();
                move_cursor_by(dx, dy);
                draw_cursor();
            }
        }

    }

    // at this point the screen is updated, with a new tile window and
    // cursor position if necessary

    // will only be down once, then waits for a min time before allowing
    // pres again.
    if (select_button_event) {
        // Button was pressed, we are selecting a point!
#ifdef DEBUG_PATH
        Serial.print("x ");
        Serial.print(cursor_map_x);
        Serial.print(" y ");
        Serial.print(cursor_map_y);
        Serial.println();
#endif

        // which press is this, the start or the stop selection?

        // If we are making a request to find a shortest path, we will send out
        // the request on the serial port and then wait for a response from the
        // server.  While this is happening, the client user interface is
        // suspended.

        gpsData * gdata;
        GTPA010::readData();
        gdata = GTPA010::getData();
        g_lat = gdata->lat;
        g_lon = gdata->lon;

        start_lat = g_lat;
        start_lon = g_lon;
        stop_lat = cursor_lat;
        stop_lon = cursor_lon;


        query_path(start_lat, start_lon, stop_lat, stop_lon);
        path_time = Sensors::getTime();
    } // end of select_button_event processing

    // do we have to redraw the map tile?  
    if (update_display_window) {
        refresh_display();
    }

    // Spam the server for a new path
    if (path_length > 0 && Sensors::getTime() - path_time >= 5) {
        gpsData * gData = GTPA010::getData();
        // If we've moved, request a new path
        if (abs(gData->lat - start_lat) > 5 || abs(gData->lon - start_lon) > 5) {
            start_lat = gData->lat;
            start_lon = gData->lon;

            query_path(start_lat, start_lon, stop_lat, stop_lon);
        }
        path_time = Sensors::getTime();
    }

    // Refresh compass display
    draw_compass();
    // Refresh gps dot
    draw_gps_dot();

    // always update the status message area if message changes
    // Indicate which point we are waiting for
    if ( request_state == 0 ) {
        status_msg("DESTINATION?");
    }
}

char* prev_status_msg = 0;

void clear_status_msg() {
    status_msg("");
}

void status_msg(char *msg) {
    // messages are strings, so we assume constant, and if they are the
    // same pointer then the contents are the same.  You can force by
    // setting prev_status_msg = 0

    if ( prev_status_msg != msg ) {
        prev_status_msg = msg;
        tft.fillRect(0, 148, 128, 12, GREEN);

        tft.setTextSize(1);
        tft.setTextColor(MAGENTA);
        tft.setCursor(0, 150);
        tft.setTextSize(1);

        tft.println(msg);
    }
}

void initialize_screen() {

    tft.initR(INITR_REDTAB);

    tft.setRotation(0);

    tft.setCursor(0, 0);
    tft.setTextColor(0x0000);
    tft.setTextSize(1);
    tft.fillScreen(BLUE);    
}

void initialize_sd_card() {
    if (!SD.begin(sd_cs)) {
#ifdef DEBUG_SERIAL
        Serial.println("Initialization has failed. Things to check:");
        Serial.println("* Is a card inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* Is the chipSelect pin the one for your shield or module?");

        Serial.println("SD card could not be initialized");
#endif

        while (1) {};    // Just wait, stuff exploded.
    }
    else {
#ifdef DEBUG_SERIAL

        Serial.println("Wiring is correct and a card is present.");

#endif
    }
}


