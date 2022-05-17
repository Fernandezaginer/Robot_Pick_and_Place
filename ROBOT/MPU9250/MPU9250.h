#include <Arduino.h>
#ifndef _MPU9250_H_
#define _MPU9250_H_
#include <Wire.h>

// Direcciones I2C
#define _MPU9250 0x68
#define _AK8963  0x0C

	// Offset a calibrar por cada acelerometro
#define OFFSET_X 0.00
#define OFFSET_Y 0.08
#define OFFSET_Z 0.00


// Configuraci�n giroscopio:
#define OFFSET_GX  429460.464
#define OFFSET_GY  638.605
#define OFFSET_GZ  5.16            //51.754


class Mpu9250 {
public:
// Variables
	float Ax, Ay, Az;
	int16_t RGx, RGy, RGz;
	float Mx = 0.0, My = 0.0, Mz = 0.0;
	float Gx, Gy, Gz;
// Configuraci�n giroscopio:
	float val_calibrado;
	//
	float read_angle_z();
	float read_magnetom_angle();
	void MPU9250_read();
	void MPU9250_init();

};
#endif