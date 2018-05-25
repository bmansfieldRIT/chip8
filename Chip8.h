/*
* chip8.cpp
* By Brian Mansfield
*/

class Chip8 {

public:
    unsigned char drawFlag;
    void initialize();
    void loadGame(std::string);
    void emulateCycle();
    void setKeys();
};
