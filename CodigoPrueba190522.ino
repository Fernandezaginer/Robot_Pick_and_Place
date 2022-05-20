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


Gripper MPG = Gripper(15,14);
Mpu9250 MPU;

EncoderAbsoluto enc = EncoderAbsoluto(30);

PID::PIDParameters<double> parameters(0.2, 0.0, 0.0);
PID::PIDController<double> pidController(parameters);

float err = 0;
float integral_prev = 0;

float kp = 0.1, ki = 0.2;
float Tp = 0.0, Ti = 0.0;

unsigned long t_0 = millis();
unsigned long t_1 = millis();


int cont_sec_1 = 0;
int cont_sec_2 = 0;
int cont_sec_3 = 0;
byte paso = 1;
float th_ad = 180;

bool MPG_abierto = false;

bool emergencia = false;

void setup() {

  interfaz.begin();
  interfaz.angulo = 180.0;
  Serial.begin(115200);
  pinMode(fin_de_carrera,INPUT_PULLUP);
  MPU.MPU9250_init();
  TCCR0A |= 0b00000011;
  TCCR3B &= 0b11111001;
  TCCR3B |= 0b00001001;

  pinMode(A15, OUTPUT);
  digitalWrite(A15, 1);
  delay(200);
  digitalWrite(A15, 0);

  /*INICIALIZACIÓN EJE Z*/
  eje_Z.set_ad(0);
  eje_Z.setVelocidad(300);
  while(eje_Z.angulo != 0){
    eje_Z.actualizar();
    if(!digitalRead(fin_de_carrera)){
      eje_Z.angulo = 0;
    }
  }
  //analogWrite(M2_PWM, 127);
  delay(1000);
  //eje_Z.set_ad(1650);
  //eje_Z.set_ad(1900);



  /*INICIALIZACIÓN EJE R*/
  eje_R.setVelocidad(500);
  eje_R.set_ad(50);



  /*INICIALIZACIÓN EJE Th*/

  //MPG.abrir_gripper();
}
void loop() {
  if(!emergencia){
    /*CONTROL EJE Z*/
    eje_Z.actualizar();
    
  
  
  
    /*CONTROL EJE R*/
    eje_R.angulo = enc.getPos_cm();
    eje_R.actualizar();
  
  
  
    /*CONTROL EJE Th*/
    MPU.MPU9250_read();
    //Controlador PI DIY
    /*if(millis() > t_0 + 100){
      err = (MPU.read_angle_z()-th_ad);
      err = abs(err)>5.0?err:0;
      Tp = kp*err;
      Ti = integral_prev + err*ki*0.1;
  
      vel = Tp + Ti;
    }
    /*if(abs(vel < 0.1)){
      vel = 0.1;
    }*//*
    eje_Th.setVelocidad(abs(vel));
    eje_Th.set_ad(vel<0?999:-999);
    eje_Th.angulo = 180;
    
    eje_Th.actualizar();
    //Serial.println(err);
    Serial.print(th_ad); Serial.print(",");
    Serial.println(MPU.read_angle_z());*/
    eje_Th.angulo = MPU.read_angle_z();
    eje_Th.actualizar();

  }
  loop_andres();

  /*
  secuencia1();
  if(millis() > t_1 + 7000){
    paso++;
    t_1 = millis();
  }
  */
}

bool code_start = 1;

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
    eje_R.set_ad(interfaz.radio);
//    eje_Z.set_ad(float altura en *unidades*); {0, 2200}
    eje_Z.set_ad(interfaz.altura);
    th_ad = interfaz.angulo;
    if(interfaz.gripper){
      if(MPG_abierto){
        MPG.cerrar_gripper();
        MPG_abierto = false;
      }
    }
    else{
      if(!MPG_abierto){
        if(!code_start){
          MPG.abrir_gripper();
        }
        MPG_abierto = true;      
        code_start = 0;
      }
    }
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

//void sec_1();

void funcion_secuencia(byte numero){
  if(numero == 1){
       if(sec_1()){
         interfaz.secuencia = 0;
         cont_sec_1 = 0;        
       }
  }

  if(numero == 2){
       if(sec_2()){
         interfaz.secuencia = 0;
         cont_sec_2 = 0;        
       }
  }

  if(numero == 3){
       if(sec_3()){
         interfaz.secuencia = 0;
         cont_sec_3 = 0;        
       }
  }
}

float pick_1[2] = {37.0, 6.0};
float pick_2[2] = {31.0, 5.5};
float pick_3[2] = {24.0, 7.0};
float place[2] = {14.0, 5.0};



bool sec_1(){
  Serial.println(cont_sec_1);
  switch(cont_sec_1){
    case 0:
      if(eje_z(0.0, 4000)){
        cont_sec_1++;
      }
    break;
    case 1:
      if(eje_r(pick_1[0], 8000)){
        cont_sec_1++;
      }
    break;
    case 2:
      if(eje_z(pick_1[1], 3000)){
        cont_sec_1++;
      }
    break;
    case 3:
      MPG.cerrar_gripper();
      cont_sec_1++;
    break;    
    case 4:
      if(eje_z(0.0, 3000)){
        cont_sec_1++;
      }
    break;
    case 5:
      if(eje_r(place[0], 8000)){
        cont_sec_1++;
      }    
    break;
    case 6:
      if(eje_z(place[1], 3000)){
        cont_sec_1++;
      }
    break; 
    case 7:
      MPG.abrir_gripper();
        cont_sec_1++;
    break;
    case 8:
      if(eje_z(0.0, 3000)){
        cont_sec_1++;
        return 1;
      }
    break;
  }
  return 0;
}





bool sec_2(){
  Serial.println(cont_sec_2);
  switch(cont_sec_2){
    case 0:
      if(eje_z(0.0, 3000)){
        cont_sec_2++;
      }
    break;
    case 1:
      if(eje_r(pick_2[0], 8000)){
        cont_sec_2++;
      }
    break;
    case 2:
      if(eje_z(pick_2[1], 3000)){
        cont_sec_2++;
      }
    break;
    case 3:
      MPG.cerrar_gripper();
      cont_sec_2++;
    break;    
    case 4:
      if(eje_z(0.0, 3000)){
        cont_sec_2++;
      }
    break;
    case 5:
      if(eje_r(place[0], 8000)){
        cont_sec_2++;
      }    
    break;
    case 6:
      if(eje_z(place[1], 3000)){
        cont_sec_2++;
      }
    break; 
    case 7:
      MPG.abrir_gripper();
        cont_sec_2++;
    break;
    case 8:
      if(eje_z(0.0, 3000)){
        cont_sec_2++;
        return 1;
      }
    break;
  }
  return 0;
}






bool sec_3(){
  Serial.println(cont_sec_1);
  switch(cont_sec_3){
    case 0:
      if(eje_z(0.0, 4000)){
        cont_sec_3++;
      }
    break;
    case 1:
      if(eje_r(pick_3[0], 8000)){
        cont_sec_3++;
      }
    break;
    case 2:
      if(eje_z(pick_3[1], 3000)){
        cont_sec_3++;
      }
    break;
    case 3:
      MPG.cerrar_gripper();
      cont_sec_3++;
    break;    
    case 4:
      if(eje_z(0.0, 3000)){
        cont_sec_3++;
      }
    break;
    case 5:
      if(eje_r(place[0], 8000)){
        cont_sec_3++;
      }    
    break;
    case 6:
      if(eje_z(place[1], 3000)){
        cont_sec_3++;
      }
    break; 
    case 7:
      MPG.abrir_gripper();
        cont_sec_3++;
    break;
    case 8:
      if(eje_z(0.0, 3000)){
        cont_sec_3++;
        return 1;
      }
    break;
  }
  return 0;
}





uint32_t inicio_1 = 0;
bool activa_1 = 0;
bool eje_z(float objetivo, int tiempo){
  if(activa_1 == 0){
    inicio_1 = millis();
    activa_1 = 1;
  }

  eje_Z.actualizar();
  eje_Z.set_ad(objetivo * 100.0);
  Serial.print(millis()); Serial.print(", ");
  Serial.println(inicio_1 + tiempo);
  if(millis() > (inicio_1 + tiempo)){
    Serial.println("FIN");
    activa_1 = 0;
    inicio_1 = 0;
    return true;
  }
    
  return false;
}


uint32_t inicio_2 = 0;
bool activa_2 = 0;
bool eje_r(float objetivo, int tiempo){
  if(activa_2 == 0){
    inicio_2 = millis();
    activa_2 = 1;
  }

  eje_R.actualizar();  
  eje_R.set_ad(objetivo);

  if(millis() > (inicio_2 + tiempo)){
    Serial.println("FIN");
    activa_2 = 0;
    inicio_2 = 0;
    return true;
  }  
  return false;
}


      





void secuencia1(){
  Serial.print(paso);
  switch(paso){
    case 1:
      eje_R.set_ad(0);
      eje_Z.set_ad(0);
      //th_ad = 180;
      eje_Th.set_ad(180);
      analogWrite(M2_PWM, 255);
      break;
    case 2:
      eje_R.set_ad(22);
      eje_Z.set_ad(0);
      MPG.abrir_gripper();
      analogWrite(M2_PWM, 127);
      break;
    case 3:
      eje_Z.set_ad(2200);
      analogWrite(M2_PWM, 255);
      break;
    case 4:
      MPG.cerrar_gripper();
      analogWrite(M2_PWM, 127);
      break;
    case 5:
      eje_Z.set_ad(0);
      analogWrite(M2_PWM, 255);
      break;
    case 6:
      eje_R.set_ad(25);
      //th_ad = 125;
      eje_Th.set_ad(125);
      analogWrite(M2_PWM, 127);
      break;
    case 7:
      eje_Z.set_ad(2200);
      analogWrite(M2_PWM, 255);
      break;
    case 8:
      MPG.abrir_gripper();
      analogWrite(M2_PWM, 127);
      break;
    case 9:
      eje_Z.set_ad(0);
      analogWrite(M2_PWM, 255);
      break;
    case 10:
      MPG.cerrar_gripper();
      analogWrite(M2_PWM, 127);
      break;
    default:
      analogWrite(M2_PWM, 127);
      paso = 1;
      break;
  }
}
