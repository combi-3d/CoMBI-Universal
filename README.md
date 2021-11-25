# CoMBI-Universal (PCB type)
This CoMBI controller is compatible with any microtomes. A Hall sensor detects handle position and syncs camera shutter release using ATmega328P and optocoupler.

The codes (.ino) were written by Arduino IDE. The circuit (.png) was made using KiCAD for PDB. Of course, you can make it with a breadboard. Parts are listed in the csv file.

Attach a neodymium magnet (10 to 15 mm in diameter and 3 mm thick) to the handle of the microtome using double-sided tape or a rubber magnet with tape.

Place the Hall sensor beside the handle with a distance of 5 to 15 mm apart from neodymium magnet. If you pile two or more neodymium magnet, you will get more magnetic power and longer distance between sensor and magnet.

I verified the operation using the followeing microtomes and cameras; LEICA CM3050 rotary cryostat (2010), YAMATO REM710 sliding microtome (2015), YAMATO rotary microtome in 1963 (an era without model number), Sony a7RIII (2017), Nikon D810 (2014), Canon 60D (2010).
