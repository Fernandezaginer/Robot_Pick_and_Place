#include <CreaStepper.h>
#include <Gripper.h>
/*byte i = 0;
float angulo=0.0;
float angulo_deseado_del_motor=90.0;
bool pasos[4][4] = {
  {1, 0, 0, 0},
  {0, 0, 1, 0},
  {0, 1, 0, 0},
  {0, 0, 0, 1}
};*/


CreaStepper MPS = CreaStepper(2,3,4,5,6,7,0);
Gripper MPG= Gripper(8,9);
void setup() {
  MPS.set_ad(120);
  
  
}
void loop() {
  MPS.actualizar();
}
