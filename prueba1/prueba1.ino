#include <CreaStepper.h>
//#include <Gripper.h>
#include <MPU9250.h>



CreaStepper MPS = CreaStepper(2,3,4,5,6,7,0);
//Gripper MPG= Gripper(8,9);
Mpu9250 MPU;
void setup() {
  Serial.begin(115200);
   
  MPS.set_ad(120);
  
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
  
}
void loop() {
  MPU.Gz=MPS.get_ad();
  MPS.actualizar();
  
  MPU.MPU9250_read();

  /* Serial.print(read_angle_z());
  Serial.write('\t');
  Serial.println(read_magnetom_angle());
  delay(5);*/
}
