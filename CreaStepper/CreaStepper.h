#include <Arduino.h>
#ifndef _CREASTEPPER_H_
#define _CREASTEPPER_H_

class CreaStepper{
  private: 
	friend class Mpu9250;
    unsigned long T;	//poner una condicion para T<400
    byte o_A1;
    byte o_A2;
    byte o_B1;
    byte o_B2;
    byte ena;//6
    byte i;
    float angulo_inicial;
    float angulo_deseado_del_motor;
	
	
	
	unsigned long t_0;
  public:
    CreaStepper(byte o_A1, byte o_A2,byte o_B1,byte o_B2, byte ena, float angulo_Inicio = 0, float lim_inf = -120, float lim_sup = 120);
    const static bool micropasos[8][4];
	float angulo;
  void actualizar();
  void darparo();
  void darpaso();
  void casa();
  void set_ad(float angulo_deseado_del_motor_nuevo);
  float get_ad();
  void setVelocidad(float velocidad);
  float _lim_inf;
  float _lim_sup;
};
#endif