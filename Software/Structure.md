# Program Structure

The program flow starts in boot.c.  From here, the board is initialized and the 'test' entry button is checked to see if we should jump to the test routine or to the user app.

The test routine is in test.c.  The user app is in app.c

Functions that abstract hardware are in their own files, such as lcd.c, led.c, etc

system.h should be included in all files.  It pulls in the headers for all the hardware abstractions to make it easy for you to interact with the hardware.

If you add a new source file, I suggest the follwing:

 - Create both a .c and a .h
 - Use the .h to declare your functions and any enums or defines you want to share
 - Include the system.h first in your .c
 - Add the .h to system.h if and only if other modules will want to call the functions in your code - that is, if you are creating a re-usable routine as opposed to say a game or something
 - Add the .c to the Makefile with the other .c files or else it will not get compiled
 
 