#include <Arduino.h>
#ifndef _CREASTEPPER_H_
#define _CREASTEPPER_H_

enum estados{acelerando = -1, vcons, decelerando};

class CreaStepper{
private:
	unsigned long T;
	byte o_A1;
	byte o_A2;
	byte o_B1;
	byte o_B2;
	byte ena;//6
	byte i;
	float angulo_inicial;
	float angulo_deseado_del_motor;

	float _velocidad;
	float _velocidad_max;
	float _aceleracion;
	
	estados estado;

	unsigned long t_0;
	unsigned long t_1;
public:
	CreaStepper(byte o_A1, byte o_A2,byte o_B1,byte o_B2, byte ena, float angulo_Inicio = 0, float lim_inf = -120, float lim_sup = 120, float velocidad = 300, float aceleracion = 10, float velocidad_max = 375);
	const static bool micropasos[8][4];
	float angulo;
	float _lim_inf;
	float _lim_sup;

	void actualizar();
	void darparo();
	void darpaso();
	void casa();

	void set_ad(float angulo_deseado_del_motor_nuevo);
	void setVelocidad(float velocidad);
	void setVelocidadMax(float velocidad_max) { this->_velocidad_max = velocidad_max > 375 ? 375 : velocidad_max; }
	void setAcc(float acc) { this->_aceleracion = acc; }
	float get_ad();
};
#endif