#include <PIDController.hpp>
#include <CreaStepper.h>
#include <Gripper.h>
#include <MPU9250.h>
#include <EncoderAbsoluto.h>
#include <interfaz.h>

// Comunicacion con la interfaz:
COM_Interfaz interfaz;


// Pines motores paso a paso
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

float vel;

CreaStepper eje_R = CreaStepper(M1_A1, M1_A2, M1_B1, M1_B2, M1_PWM, 0, 12, 39);
CreaStepper eje_Z = CreaStepper(M2_A1, M2_A2, M2_B1, M2_B2, M2_PWM, 9999, 0, 1900);
CreaStepper eje_Th = CreaStepper(M3_A1, M3_A2, M3_B1, M3_B2, M3_PWM, 180, 30, 330);


Gripper MPG = Gripper(8,9);
Mpu9250 MPU;

EncoderAbsoluto enc = EncoderAbsoluto(30);

PID::PIDParameters<double> parameters(0.2, 0.0, 0.0);
PID::PIDController<double> pidController(parameters);

float err = 0;
float integral_prev = 0;

float kp = 0.25, ki = 0.0;
float Tp = 0.0, Ti = 0.0;

unsigned long t_0 = millis();

void setup() {
  Serial.begin(115200);
  pinMode(fin_de_carrera,INPUT_PULLUP);
  MPU.MPU9250_init();
  TCCR0A |= 0b00000011;
  TCCR3B &= 0b11111001;
  TCCR3B |= 0b00001001;

  /*INICIALIZACIÓN EJE Z*/
  eje_Z.set_ad(0);
  eje_Z.setVelocidad(300);
  while(eje_Z.angulo != 0){
    eje_Z.actualizar();
    if(!digitalRead(fin_de_carrera)){
      eje_Z.angulo = 0;
    }
  }
  analogWrite(M2_PWM, 127);
  delay(1000);
  //eje_Z.set_ad(1650);
  //eje_Z.set_ad(1900);



  /*INICIALIZACIÓN EJE R*/
  eje_R.setVelocidad(500);
  eje_R.set_ad(20);



  /*INICIALIZACIÓN EJE Th*/
  /*pidController.SetOutputLimits(-5,5);
  pidController.Input = MPU.read_magnetom_angle();
  pidController.Setpoint = 200;
  pidController.SetSampleTime(500);
  pidController.TurnOn();*/

  //eje_Th.setVelocidad(20);
  //eje_Th.set_ad(150);

  MPG.abrir_gripper();
}
void loop() {
  /*CONTROL EJE Z*/
  //eje_Z.actualizar();
  analogWrite(M2_PWM, 127);



  /*CONTROL EJE R*/
  eje_R.angulo = enc.getPos_cm();
  eje_R.actualizar();



  /*CONTROL EJE Th*/
  MPU.MPU9250_read();
  /*pidController.Input = MPU.read_angle_z();
  if(pidController.Update()){
    
    Serial.print(pidController.Setpoint); Serial.print(", ");
    Serial.print(pidController.Input); Serial.print(", ");
    Serial.print(pidController.Output); Serial.print(", ");
    Serial.println(MPU.read_angle_z());
    
    eje_Th.setVelocidad(abs(pidController.Output));
    eje_Th.set_ad(pidController.Output>0?999:-999);
  }*/
  /*if(abs(pidController.Input - pidController.Setpoint) < 4.0){
    pidController.TurnOff();
    eje_Th.setVelocidad(0);
  }
  else{ 
    //pidController.TurnOn();
  }
  //Serial.println(MPU.read_magnetom_angle());
  eje_Th.angulo = 180;*/

  //Controlador PI DIY
  if(millis() > t_0 + 500){
    err = (MPU.read_angle_z()-160);
    err = abs(err)>3.0?err:0;
    Tp = kp*err;
    Ti = integral_prev - err*ki*0.5;

    vel = Tp + Ti;
  }
  /*Serial.print(err); Serial.print(", ");
  Serial.print(MPU.read_angle_z()); Serial.print(", ");
  Serial.println(vel);*/
  eje_Th.setVelocidad(abs(vel));
  eje_Th.set_ad(vel<0?999:-999);
  eje_Th.angulo = 180;
  
  eje_Th.actualizar();
  //eje_Th.angulo = MPU.read_angle_z();
  eje_Th.actualizar();
  Serial.println(err);
}


// --------------------------------------------------------
//                 FUNCIONAMIENTO NORMAL
// --------------------------------------------------------

byte secuencia = 0;

void loop_andres(){

  if(interfaz.available()){
    //secuencia = interfaz.secuencia();
  }

  if(secuencia > 0){
    funcion_secuencia(secuencia);
  }
  else if(secuencia == 0){
    
  }

  

  if(interfaz.emergencia){
    // Detener todo
    while(!interfaz.rearme){
      interfaz.available();
    }
    
  }
  
}




// --------------------------------------------------------
//                      SECUENCIAS
// --------------------------------------------------------

void funcion_secuencia(byte numero){
  // ...
}
