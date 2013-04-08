/*
 * Routine for drawing an image patch from the SD card to the LCD display.
 */

#ifndef _LCD_IMAGE_H
#define _LCD_IMAGE_H


// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE	0x001F
#define	RED	 0xF800
#define	GREEN   0x07E0
#define CYAN	0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef struct {
  char *file_name;
  uint16_t ncols;
  uint16_t nrows;
} lcd_image_t;

/* Draws the referenced image to the LCD screen.
 *
 * img           : the image to draw
 * tft           : the initialized tft struct
 * icol, irow    : the upper-left corner of the image patch to draw
 * scol, srow    : the upper-left corner of the screen to draw to
 * width, height : controls the size of the patch drawn.
 */
void lcd_image_draw(lcd_image_t *img, Adafruit_ST7735 *tft,
		    uint16_t icol, uint16_t irow, 
		    uint16_t scol, uint16_t srow, 
		    uint16_t width, uint16_t height);

#endif
