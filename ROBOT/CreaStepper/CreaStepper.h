#include <Arduino.h>
#ifndef _CREASTEPPER_H_
#define _CREASTEPPER_H_

class CreaStepper{
  private: 
    //int T; //poner una condicion para T<400
    byte o_A1;
    byte o_A2;
    byte o_B1;
    byte o_B2;
    byte ena;//6
    byte enb;//7
    byte i;
    float angulo_inicial;
    float angulo_deseado_del_motor;
  public: 
    CreaStepper(byte o_A1, byte o_A2,byte o_B1,byte o_B2, byte ena, byte enb, float angulo_Inicio = 0);
    float angulo;
	int T; //poner una condicion para T<400
    const static inline bool micropasos[8][4] = {
    {1, 0, 0, 0}, //dependiendo de la pol será los dos primeros numeros 1 o 0 (uno es 1 y el otro es 0)=>igual //para el otro puente
    {1, 0, 1, 0}, // 1 0 pol 1, 00 despolarizado, 01 pol 2
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
    };
  void actualizar();
  void darparo();
  void darpaso();
  void casa();
  void set_ad(float angulo_deseado_del_motor_nuevo);
  float get_ad();
  void setVelocidad(float velocidad);
};
#endif