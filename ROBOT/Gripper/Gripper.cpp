#include <Arduino.h>
#include <CreaStepper.h>
#include <Gripper.h>


Gripper::Gripper(byte o_O1,byte o_O2){
  this->o_O1= o_O1;
  this->o_O2= o_O2;

  pinMode(this->o_O1, OUTPUT);
  pinMode(this->o_O2, OUTPUT);
}
void Gripper::abrir_gripper(){
  digitalWrite(this->o_O1, LOW);
  digitalWrite(this->o_O2, HIGH);
  delay(2000);
  digitalWrite(this->o_O2, LOW);
}
void Gripper::cerrar_gripper(){
  digitalWrite(this->o_O2, LOW);
  digitalWrite(this->o_O1, HIGH);
  delay(2000);
  digitalWrite(this->o_O1, LOW);  
}






     