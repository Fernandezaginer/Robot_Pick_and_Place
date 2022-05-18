#include <CreaStepper.h>
#include <Gripper.h>
#include <MPU9250.h>


#define M1_A1   12
#define M1_A2   11
#define M1_B1   10
#define M1_B2   9
#define M1_PWM  4


CreaStepper MPS = CreaStepper(M1_A1, M1_A2, M1_B1, M1_B2, M1_PWM, 0);
Gripper MPG = Gripper(8,9);
Mpu9250 MPU;
void setup() {
  //Serial.begin(115200);
   
  MPS.set_ad(1000);
  
  MPU.MPU9250_init();


  MPU.Gz = 0.0;


  // Calibrado:
  /*
    for (int i = 0; i < 1; i++) {
    uint32_t suma;
    suma = 0;
    for (int i = 0; i < 10000; i++) {
      MPU9250_read();
      suma = suma + RGz;
    }
    val_calibrado = (float) ((float)suma / 10000.0);
    Serial.println(val_calibrado);
    }
  */
  MPS.setVelocidad(20);
}
void loop() {
  MPS.angulo = MPU.Gz;
  MPS.actualizar();
  
  //MPU.MPU9250_read();

  /* Serial.print(read_angle_z());
  Serial.writ e('\t');
  Serial.println(read_magnetom_angle());
  delay(5);*/
}
