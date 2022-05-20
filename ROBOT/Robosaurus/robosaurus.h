#include <Arduino.h>
#include <PIDController.hpp>
#include <CreaStepper.h>
#include <Gripper.h>
#include <MPU9250.h>

#ifndef _ROBOSAURUS_H

class Robosaurus{
private:
	CreaStepper _eje_R;
	CreaStepper _eje_Z;
	CreaStepper _eje_Th;
	
	Gripper _MPG;
	Mpu9250 _MPU;
	PID::PIDController<double> _control_eje_Th;
	
	byte _fin_de_carrera;
public:
	Robosaurus(CreaStepper eje_R, CreaStepper eje_Z, CreaStepper eje_Th, Gripper MPG, Mpu9250 MPU, byte fin_de_carrera, double kp, double ki, double kd, );
	setR(float R);
	setZ(float Z);
	setTh(float Th);
	
	setup();
	actualizar();
}

#endif