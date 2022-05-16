#include <Arduino.h>
#ifndef _GRIPPER_H_
#define _GRIPPER_H_

class Gripper{
private:
  byte o_O1;
  byte o_O2;
public:
  Gripper(byte o_O1,byte o_O2);
  void abrir_gripper();
  void cerrar_gripper();
};
#endif