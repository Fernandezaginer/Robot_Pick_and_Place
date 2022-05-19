#include <EncoderAbsoluto.h>

byte EncoderAbsoluto::getPos(){
  //byte valor = 0b00111111&read_register;
  byte valor = ((analogRead(A5)>500)<<5) + ((analogRead(A4)>500)<<4) + ((analogRead(A3)>500)<<3) + ((analogRead(A2)>500)<<2) + ((analogRead(A1)>500)<<1) + ((analogRead(A0)>500<<0));
  byte bin = 0b00100000 & valor;
  bin += ((0b00100000 & bin) >> 1) ^ 0b00010000 & valor;
  bin += ((0b00010000 & bin) >> 1) ^ 0b00001000 & valor;
  bin += ((0b00001000 & bin) >> 1) ^ 0b00000100 & valor;
  bin += ((0b00000100 & bin) >> 1) ^ 0b00000010 & valor;
  bin += ((0b00000010 & bin) >> 1) ^ 0b00000001 & valor;
  return bin;
}

float EncoderAbsoluto::getPos_cm(){
  return 11.5 + (longitud-((float)this->getPos()*longitud/63));
}