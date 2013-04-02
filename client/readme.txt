Arduino Pathfinder Client
CMPUT 297

by Stephen Just

This assignment is a solo effort, although discussion took place with
Stefan Martynkiw, Anders Muskens
==========================================================================

This map client is designed to be run on the Arduino Mega 2550 with an
Adafruit ST7735-based display.

Use the joystick to move around the map. Click the joystick button to
select paths. Use the two zoom buttons to zoom in and out.

Limitations:
When moving the cursor, you can erase a path if you move the cursor over
it. The path will be redrawn after the window moves.

Any time the window moves, the cursor will jump to the middle of the
window to prevent getting stuck at the map edges or being put in an
unpredictable location when zooming the map.
