#include <PIDController.hpp>
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

#define M3_A1   22
#define M3_A2   23
#define M3_B1   24
#define M3_B2   25
#define M3_PWM   2

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
CreaStepper eje_Th = CreaStepper(M3_A1, M3_A2, M3_B1, M3_B2, M3_PWM, 180, 30, 330);


Gripper MPG = Gripper(8,9);
//Mpu9250 MPU;

EncoderAbsoluto enc = EncoderAbsoluto(30);

PID::PIDParameters<double> parameters(4.0, 0, 0);
PID::PIDController<double> pidController(parameters);

void setup() {
  Serial.begin(115200);
  pinMode(fin_de_carrera,INPUT_PULLUP);
  MPU.MPU9250_init();

  /*INICIALIZACIÓN EJE Z*/
  eje_Z.set_ad(0);
  eje_Z.setVelocidad(300);
  while(eje_Z.angulo != 0){
    eje_Z.actualizar();
    if(!digitalRead(fin_de_carrera)){
      eje_Z.angulo = 0;
    }
    Serial.println(eje_Z.angulo);
  }
  delay(1000);
  //eje_Z.set_ad(1650);
  //eje_Z.set_ad(1900);



  /*INICIALIZACIÓN EJE R*/
  eje_R.set_ad(50);
  eje_R.setVelocidad(500);



  /*INICIALIZACIÓN EJE Th*/
  pidController.SetOutputLimits(-300,300);
  pidController.Input = enc.getPos_cm();
  pidController.Setpoint = 100;
}
void loop() {
  /*CONTROL EJE Z*/
  eje_Z.actualizar();



  /*CONTROL EJE R*/
  eje_R.angulo = enc.getPos_cm();
  eje_R.actualizar();



  /*CONTROL EJE Th*/
  MPU.MPU9250_read();
  pidController.Input = MPU.read_angle_z();
  pidController.Update();
  eje_Th.setVelocidad(abs(pidController.Output));
  eje_Th.set_ad(pidController.Output>0?999:-999);
  eje_Th.angulo = 0;
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
