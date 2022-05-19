#include <Arduino.h>

#ifndef _ENCODERABSOLUTO_H_

#define _ENCODERABSOLUTO_H_

class EncoderAbsoluto{
private:
  float longitud;
public:
  EncoderAbsoluto(float largo = 30): longitud(largo){}
  byte getPos();
  float getPos_cm();
};

#endif