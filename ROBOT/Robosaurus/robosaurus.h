#include <Arduino.h>
//#include <PIDController.hpp>
#include <CreaStepper.h>
#include <Gripper.h>
#include <MPU9250.h>
#include <EncoderAbsoluto.h>

#ifndef _ROBOSAURUS_H

class Robosaurus{
private:
	//Actuadores
	CreaStepper _eje_R;
	CreaStepper _eje_Z;
	CreaStepper _eje_Th;
	
	//Sensores
	Gripper _MPG;
	Mpu9250 _MPU;
	EncoderAbsoluto _ENC;
	
	byte _fin_de_carrera;
	
	//PID
	float _err = 0;
	float _integral_prev = 0;

	float _kp;
	float _ki;
	float _Tp;
	float _Ti;
	float _vel;

	unsigned long _t_0;
	float _th_ad;
	
	//Miscelaneo
	bool _MPG_abierto;
public:
	Robosaurus(CreaStepper eje_R, CreaStepper eje_Z, CreaStepper eje_Th, Gripper MPG, Mpu9250 MPU, EncoderAbsoluto ENC, byte fin_de_carrera, double kp = 0.1, double ki = 0.05);
	//Robosaurus(CreaStepper eje_R, CreaStepper eje_Z, CreaStepper eje_Th, Gripper MPG, Mpu9250 MPU, EncoderAbsoluto ENC, byte fin_de_carrera);
	
	void begin();
	void actualizar();
	
	void setR(float R);
	void setZ(float Z);
	void setTh(float Th);
	void setGripper(bool Open);
	
	void abrirGripper();
	void cerrarGripper();
	void toggleGripper();
};

#endif