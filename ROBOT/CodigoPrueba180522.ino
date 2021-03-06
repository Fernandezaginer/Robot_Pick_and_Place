#include <CreaStepper.h>
#include <Gripper.h>
#include <MPU9250.h>


#define M1_A1   12
#define M1_A2   11
#define M1_B1   10
#define M1_B2   9
#define M1_PWM  4

#define M2_A1   8
#define M2_A2   7
#define M2_B1   6
#define M2_B2   5
#define M2_PWM  3
#define fin_de_carrera 18

class EncoderAbsoluto{
private:
  //long int read_register;
  float longitud;
public:
  //EncoderAbsoluto(long int registro = PORTF, float largo = 30):read_register(registro), longitud(largo){}
  EncoderAbsoluto(float largo = 30): longitud(largo){}
  byte getPos();
  float getPos_cm();
};

CreaStepper eje_R = CreaStepper(M1_A1, M1_A2, M1_B1, M1_B2, M1_PWM, 0, 12, 39.5);
CreaStepper eje_Z = CreaStepper(M2_A1, M2_A2, M2_B1, M2_B2, M2_PWM, 9999, 0, 1900);

Gripper MPG = Gripper(15,14);
//Mpu9250 MPU;

EncoderAbsoluto enc = EncoderAbsoluto(30);

void setup() {
  Serial.begin(115200);
  pinMode(fin_de_carrera,INPUT_PULLUP);
   
  eje_R.set_ad(50);
  eje_Z.set_ad(0);
  eje_Z.setVelocidad(300);

  while(eje_Z.angulo != 0){
    eje_Z.actualizar();
    if(!digitalRead(fin_de_carrera)){
      eje_Z.angulo = 0;
    }
    Serial.println(eje_Z.angulo);
  }
  eje_R.set_ad(38);
  eje_R.angulo = enc.getPos_cm();
  while(eje_R.angulo < 37){
    eje_R.actualizar();
    eje_R.angulo = enc.getPos_cm();
  }
  delay(1000);
  MPG.abrir_gripper();
  eje_Z.set_ad(1600);
  while(eje_Z.angulo < 1598){
    eje_Z.actualizar();
  }
  MPG.cerrar_gripper();
  eje_Z.set_ad(0);
  while(eje_Z.angulo > 2){
    eje_Z.actualizar();
  }
  eje_R.set_ad(15);
  eje_R.angulo = enc.getPos_cm();
  while(eje_R.angulo > 16){
    eje_R.actualizar();
    eje_R.angulo = enc.getPos_cm();
  }
  eje_Z.set_ad(1900);
  while(eje_Z.angulo < 1895){
    eje_Z.actualizar();
  }
  MPG.abrir_gripper();
  eje_Z.set_ad(0);
  while(eje_Z.angulo > 2){
    eje_Z.actualizar();
  }
  MPG.cerrar_gripper();
  //eje_Z.set_ad(1900);
  //MPU.MPU9250_init();


  //MPU.Gz = 0.0;


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
  //eje_R.setVelocidad(0.25);
  eje_R.setVelocidad(500);
}
void loop() {
  //eje_R.angulo = MPU.read_angle_z();
  eje_R.angulo = enc.getPos_cm();
  MPG.abrir_gripper();
  MPG.cerrar_gripper();
  //eje_R.actualizar();
  //eje_Z.actualizar();
  digitalWrite(M2_PWM, LOW);
  digitalWrite(M1_PWM, LOW);
  Serial.print(eje_Z.angulo);
  Serial.print(", ");
  Serial.println(eje_Z.get_ad());
  //MPG.abrir_gripper();
  //MPG.cerrar_gripper();
  
  //MPU.MPU9250_read();
}
byte EncoderAbsoluto::getPos(){
  //byte valor = 0b00111111&read_register;
  byte valor = ((analogRead(A5)>500)<<5) + ((analogRead(A4)>500)<<4) + ((analogRead(A3)>500)<<3) + ((analogRead(A2)>500)<<2) + ((analogRead(A1)>500)<<1) + ((analogRead(A0)>500<<0));
  byte bin = 0b00100000 & valor;
  bin += ((0b00100000 & bin) >> 1) ^ 0b00010000 & valor;
  bin += ((0b00010000 & bin) >> 1) ^ 0b00001000 & valor;
  bin += ((0b00001000 & bin) >> 1) ^ 0b00000100 & valor;
  bin += ((0b00000100 & bin) >> 1) ^ 0b00000010 & valor;
  bin += ((0b00000010 & bin) >> 1) ^ 0b00000001 & valor;
  return bin;
}

void print8(byte val){
  for (int i = 0; i < 8; i++)
  {
    bool b = val & 0x80;
    Serial.print(b);
    val = val << 1;
  }
  Serial.println();
}
float EncoderAbsoluto::getPos_cm(){
  return 11.5 + (longitud-((float)this->getPos()*longitud/63));
}
