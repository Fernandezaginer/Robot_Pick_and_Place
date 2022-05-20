#include <PIDController.hpp>
#include <CreaStepper.h>
#include <Gripper.h>
#include <MPU9250.h>
#include <EncoderAbsoluto.h>
#include <interfaz.h>
#include <robosaurus.h>

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

Robosaurus paco = Robosaurus(CreaStepper(M1_A1, M1_A2, M1_B1, M1_B2, M1_PWM, 0, 12, 39), CreaStepper(M2_A1, M2_A2, M2_B1, M2_B2, M2_PWM, 9999, 0, 1900), CreaStepper(M3_A1, M3_A2, M3_B1, M3_B2, M3_PWM, 180, 30, 330), Gripper(15,14), Mpu9250(), EncoderAbsoluto(30), fin_de_carrera, 0.1, 0.05);


unsigned long t_1 = millis();
byte paso = 1;

bool emergencia = false;

void setup() {

  paco.begin();

  interfaz.begin();
  interfaz.angulo = 180.0;
  Serial.begin(115200);

  pinMode(A15, OUTPUT);
  digitalWrite(A15, 1);
  delay(200);
  digitalWrite(A15, 0);
}
void loop() {
  paco.actualizar();
  loop_andres();

  /*
  secuencia1();
  if(millis() > t_1 + 7000){
    paso++;
    t_1 = millis();
  }
  */
}


// --------------------------------------------------------
//                 FUNCIONAMIENTO NORMAL
// --------------------------------------------------------

byte secuencia_ = 0;

void loop_andres(){

  if(interfaz.available()){
    secuencia_ = interfaz.secuencia;
  }

  if(secuencia_ > 0){
    funcion_secuencia(secuencia_);
  }
  else if(secuencia_ == 0){
    paco.setR(interfaz.radio);
    paco.setZ(interfaz.altura);
    paco.setTh(interfaz.angulo);
    paco.setGripper(interfaz.gripper);
  }

  

  if(interfaz.emergencia){
    emergencia = true;
    while(!interfaz.rearme){
      interfaz.available();
    }
    emergencia = false;
  }
  
}




// --------------------------------------------------------
//                      SECUENCIAS
// --------------------------------------------------------

void funcion_secuencia(byte numero){
  // ...
}
void secuencia1(){
  
  Serial.print(paso);
  switch(paso){
    case 1:
      paco.setR(0);
      paco.setZ(0);
      paco.setTh(180);
      break;
    case 2:
      paco.setR(22);
      paco.setZ(0);
      paco.abrirGripper();
      break;
    case 3:
      paco.setZ(2200);
      break;
    case 4:
      paco.cerrarGripper();
      break;
    case 5:
      paco.setZ(0);
      break;
    case 6:
      paco.setR(25);
      paco.setTh(125);
      break;
    case 7:
      paco.setZ(2200);
      break;
    case 8:
      paco.abrirGripper();
      break;
    case 9:
      paco.setZ(0);
      break;
    case 10:
      paco.cerrarGripper();
      break;
    default:
      paso = 1;
      break;
  }
}
