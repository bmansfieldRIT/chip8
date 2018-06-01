# chip8
chip8 game system emulator

#### What Is Chip8?
Chip8 is essentially a virtual machine, designed in the 70s, and game designers could write games in Chip8 and executed on any computer with a Chip8 emulator/interpreter.

#### Current State of Emulator
* emulator loop implemented
* all opcodes implemented
* Chip8 header file and driver code (GLUT init, keyboard init, etc.) implemented in file from Laurence Mueller's tutorial
* test suite needs to be written for opcodes, initializations

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
