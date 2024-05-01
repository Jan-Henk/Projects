# Atari disk drive emulator
The Seeeduino Xiao board, combined with the CH376S board, works together to emulate a disk drive that Atari 8-bit computers from the home computer era can recognize. This setup allows the system to read & write data from & to USB memory devices like thumb drives, making it easy to use modern storage options with older Atari computers.
# About
"This project was a way for me to learn C programming for embedded systems while connecting an Atari 8-bit computer to a USB memory device. The idea came from my childhood love for the Atari 600XL. Back then I taught myself the Basic and assembly programming languages.

In the mid-90s, I got curious about C programming but found it intimidating, so I tried Pascal instead. Programming took a backseat until a few years ago. That's when I bought a secondhand Atari 600XL and decided to upgrade it with more memory and video enhancements.

But finding an original disk drive was a challenge, so I turned to USB. Figuring out the USB data storage protocol was tough, but I stumbled upon the CH376S board, which acts as an interface between the Atari and USB storage. Pairing it with the Seeeduino Xiao microcontroller board was the obvious choice, because it is small, has a fast microcontroller and can do SPI communication.

Transitioning from Pascal to C was difficult at first, but with so many online resources, I managed to adapt to it fairly well. The Atari 8-bit hardware documentation was still available online and it fascinated me, because back then I never got the chance to look into it."
# The code
At the moment, there is just one file that has it all. It can be uploaded to a Seeeduino Xiao board with the Arduino IDE and from there on it should work fine.
Remember that this still is work in progress. For everything to work out of the box, you have to connect and/or solder everything as mentioned in the first part of the code.
The code can load a program or a game that has an ATR extension. Other extensions are not actively supported yet. The header of the ATR file has to be removed. In the future the code will do it for you. There is no menu to choose from programs or games. That is something that will be implemented in future versions. The code cannot run programs or games that depend on disk drive timings, skewing, piracy countermeasures, etcetera for obvious reasons.
