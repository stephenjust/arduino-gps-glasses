arduino-gps-glasses
===================

Arduino-powered GPS with a python map-finding server

This was a group project for CMPUT 297 (now called CMPUT 275) at the University of Alberta.

The project featured a set of plastic glasses frames with eight LED's affixed to the frame around one eye. The LED's were wired such that they could be selected using a 3-to-8 multiplexer. A switch was connected to one of the multiplexer's enable pins to turn the lights on or off.

The multiplexer was to connect to an Arduino Mega 2560, which also had a GPS receiver and a tilt-compensated compass connected to it. Through this, the Arduino could gather its position and orientation on a map of Edmonton, which was loaded on to an SD card.

The Arduino communicated with a Raspberry Pi, which ran a pathfinding program. This would generate a path for a person to follow. Based on the GPS and compass readings, this would allow the user to turn on the glasses to see which direction they should be moving.

The system could successfully retrieve a path and direct a user, however the digital compass was not effective on the university campus. As such, its accuracy was limited where it was tested.

The hardware assembled to run this project has since been decomissioned and disassembled.
