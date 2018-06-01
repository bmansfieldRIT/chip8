# chip8
chip8 game system emulator

#### To Run
When linking, include options `-framework OpenGL -framework GLUT`

#### What Is Chip8?
Chip8 is essentially a virtual machine, designed in the 70s, and game designers could write games in Chip8 and executed on any computer with a Chip8 emulator/interpreter.

#### Current State of Emulator
* emulator loop implemented
* all opcodes implemented
* Chip8 header file and driver code (GLUT init, keyboard init, etc.) implemented in file from Laurence Mueller's tutorial
* test suite needs to be written for opcodes, initializations

#### Does It Work?
* Essentially, the emulator will start up, display something on the screen, and apparently freeze up, no longer refreshing the screen. The emulateCycle() function is running repeatedly, so i suspect the actual problem may lie in the GLUT implementation. Given how the GLUT code is largely taken from LM's tutorial, and comprised of many deprecated functions, it may take a while to parse and fix. Therefore, I will be taking a break from this project for a while. Hopefully to return and finally get it working!

#### So Why Do It?
* I've had this project in my to-do list since i started college in 2012
* It proved to be a good refresher/exercise on bitwise operations, hex/binary concepts, and C++ program concepts.

#### Resources
* Laurence Meuller's fantastic beginner's guide on explaining the chip8 requirements, and walking through some of the implementation.
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

* Wikipedia
https://en.wikipedia.org/wiki/CHIP-8


#### Notes:
* to initialize variables in a case statement, must surround with {}
* suggested keyboard layout:
```
Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+

```

* How bytes represent a sprite:

```
HEX    BIN        Sprite
0x3C   00111100     ****
0xC3   11000011   **    **
0xFF   11111111   ********

```
