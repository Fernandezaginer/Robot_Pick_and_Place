#include <Arduino.h>
#include <MPU9250.h> 
#include <Wire.h>

float Mpu9250::read_angle_z() {
	static unsigned long m_o = 0;
	Gz = Gz + (((float)RGz) - OFFSET_GZ) * 0.007907 * (millis() - m_o) * 0.0075;
	m_o = millis();
	return Gz;
}
float Mpu9250::read_magnetom_angle() {
	static float orientacion_o = 0.0;
	const float al = 0.1;
	float aux;
	float orientacion = atan2(My, Mx) * (180.0 / 3.1416);
	if (orientacion < 0.0) {
		orientacion = 360 + orientacion;
	}

	aux = orientacion_o * (1 - al) + orientacion * al;
	orientacion_o = aux;

	return aux;
}

void Mpu9250::MPU9250_read() {
	// Lectura de la velocidad angular:
	Wire.beginTransmission(_MPU9250);
	Wire.write(0x43);
	Wire.endTransmission(false);
	Wire.requestFrom(_MPU9250, 6, true);
	RGx = (float)((Wire.read() << 8) | Wire.read());
	RGy = (float)((Wire.read() << 8) | Wire.read());
	RGz = (float)((Wire.read() << 8) | Wire.read());

	// Lectura de la aceleración lineal:
	Wire.beginTransmission(_MPU9250);
	Wire.write(0x3B);
	Wire.endTransmission(false);
	Wire.requestFrom(_MPU9250, 6, true);
	Ax = (float)((Wire.read() << 8) | Wire.read()) / 16384.0;
	Ay = (float)((Wire.read() << 8) | Wire.read()) / 16384.0;
	Az = (float)((Wire.read() << 8) | Wire.read()) / 16384.0;


	// Lectura del magnetometro
	Wire.beginTransmission(_AK8963);
	Wire.write(0x02);
	Wire.requestFrom(_AK8963, 1, true);
	if ((Wire.read() & 0x01) == 0x01) {
		Wire.beginTransmission(_AK8963);
		Wire.write(0x03);
		//Wire.endTransmission(false);
		Wire.requestFrom(_AK8963, 7, true);
		Mx = (float)(Wire.read() | (Wire.read() << 8));
		My = (float)(Wire.read() | (Wire.read() << 8));
		Mz = (float)(Wire.read() | (Wire.read() << 8));
		if ((Wire.read() & 0x08)) {
			Mx = 0.0;
			My = 0.0;
			Mz = 0.0;
		}
	}

}

void Mpu9250::MPU9250_init() {

	Wire.begin();

	// Rango de salida acelerometro
	/*
	  #define MPU9250_RANGE2G  0x00
	  #define MPU9250_RANGE4G  0x01
	  #define MPU9250_RANGE8G  0x02
	  #define MPU9250_RANGE16G 0x03
	*/
	byte tempRegVal = 0;
	tempRegVal |= (0x03 << 3);    // Cambiar aquí el rango de lectura
	Wire.beginTransmission(_MPU9250);
	Wire.write(0x1C);
	Wire.write(tempRegVal);
	Wire.endTransmission();

	// Filtro paso bajo accelerometro:
	Wire.beginTransmission(_MPU9250);
	Wire.write(0x1D);
	Wire.write(B0000000);
	Wire.endTransmission();

	// Rango del Giroscopio:
	Wire.beginTransmission(_MPU9250);
	Wire.write(27);
	Wire.write(B0011000);
	Wire.endTransmission();

	// Activar multiplexor del magnetometro
	Wire.beginTransmission(_MPU9250);
	Wire.write(0x6A);
	Wire.write(0x00);
	Wire.endTransmission();

	Wire.beginTransmission(_MPU9250);
	Wire.write(0x37);  // BYPASS_CONFIG_AD
	Wire.write(0x02);
	Wire.endTransmission();

	// Salida 100Hz 16bit magnetometro
	Wire.beginTransmission(_AK8963);
	Wire.write(0x0A);       // CNTL1_AD
	Wire.write(B00010110);
	Wire.endTransmission();

	delay(100);

	// Lecturas de precalentamiento giroscopio:
	for (int i = 0; i < 4000; i++) {
		MPU9250_read();
	}

}