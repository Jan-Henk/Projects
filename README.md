# Atari disk drive emulator
"The Seeeduino Xiao board, in collaboration with the CH376S board, operates in unison to emulate a disk drive compatible with Atari 8-bit computers dating from the 1970s, 1980s, and early 1990s. The system is capable of both reading from and writing to USB memory devices, such as thumb drives, providing seamless compatibility with modern storage solutions."
# About
"This project, initiated a few years ago, was a way for me to learn C programming for embedded systems while connecting an Atari 8-bit computer to a USB memory device. The idea came from my childhood, when I was obsessed with my Atari 600XL. I upgraded its memory, played around with electronics, and messed with Basic and assembly language.

In the mid-90s, I got curious about C programming but found it intimidating, so I tried Pascal instead. Programming took a backseat until COVID-19 hit. That's when I bought a secondhand Atari 600XL and decided to upgrade it with more memory and video enhancements.

But finding an original disk drive was a challenge, so I turned to USB. Figuring out the USB data storage protocol was tough, but I stumbled upon the CH376S board, which acts as an interface between the Atari and USB storage. Pairing it with the Seeeduino Xiao microcontroller board was a no-brainer.

Transitioning from Pascal to C was difficult at first, but with so many online resources, I managed to adapt to it fairly well. The Atari 8-bit hardware documentation was still available online and it fascinated me, because back then I never got the chance to look into it."
# The code
At the moment, there is just one file that has it all. It can be uploaded to a Seeeduino Xiao board with the Arduino IDE and from there on it should work fine.
Remember that this still is work in progress. For everything to work out of the box, you have to connect and/or solder everything as mentioned in the first part of the code.
The code can load a program or a game that has an ATR extension. Other extensions are not actively supported yet. The header of the ATR file has to be removed. In the future the code will do it for you. There is no menu to choose from programs or games. That is something that will be implemented in future versions. The code cannot run programs or games that depend on disk drive timings, skewing, piracy countermeasures, etcetera for obvious reasons.
