#include <Arduino.h>
#include <CreaStepper.h>

CreaStepper::CreaStepper(byte o_A1, byte o_A2,byte o_B1,byte o_B2, byte ena, byte enb, float angulo_Inicio){
  this->o_A1 = o_A1;
  this->o_A2 = o_A2;
  this->o_B1 = o_B1;
  this->o_B2 = o_B2;
  this->ena=ena;
  this->enb=enb;

  this->angulo = this->angulo_inicial = angulo_Inicio;
  this->angulo_deseado_del_motor=90;
  this-> i=0;
  this->T = 500;

  pinMode(this->o_A1, OUTPUT);
  pinMode(this->o_A2, OUTPUT);
  pinMode(this->o_B1, OUTPUT);
  pinMode(this->o_B2, OUTPUT);
  
  //Serial.begin(9600);
}

void CreaStepper::actualizar(){
  if(angulo - angulo_deseado_del_motor > 0.9){
      i--;
      darpaso();
      angulo -= 0.9;
    }
    else if(angulo - angulo_deseado_del_motor < -0.9){
      i++;
      darpaso();
      angulo += 0.9;
    }
    else{
      darparo();
    }
    //Serial.print(angulo); Serial.print(", ");
    //Serial.print(angulo_deseado_del_motor); Serial.print(", ");
    //Serial.println(angulo - angulo_deseado_del_motor);
}

void CreaStepper::darpaso(){
    digitalWrite(this->ena, HIGH);
    digitalWrite(this->enb, HIGH);
    digitalWrite(this->o_A1,CreaStepper::micropasos[i %8][0]);
    digitalWrite(this->o_A2, CreaStepper::micropasos[i %8][1]);
    digitalWrite(this->o_B1, CreaStepper::micropasos[i %8][2]);
    digitalWrite(this->o_B2, CreaStepper::micropasos[i %8][3]);
    
    delayMicroseconds(this->T);
}

void CreaStepper::darparo(){
    analogWrite(this->ena, 110);
    analogWrite(this->enb, 110);
    digitalWrite(this->o_A1, CreaStepper::micropasos[i %8][0]);
    digitalWrite(this->o_A2, CreaStepper::micropasos[i %8][1]);
    digitalWrite(this->o_B1, CreaStepper::micropasos[i %8][2]);
    digitalWrite(this->o_B2, CreaStepper::micropasos[i %8][3]);

    //delayMicroseconds(this->T);
    delay(1000);
    casa();
}
void CreaStepper::casa(){
  this->angulo_deseado_del_motor = this->angulo_inicial;
}
void CreaStepper::set_ad(float angulo_deseado_del_motor_nuevo){
  
  this->angulo_deseado_del_motor = angulo_deseado_del_motor_nuevo;
  
  if(this->angulo_deseado_del_motor>120){
   this->angulo_deseado_del_motor=120;
  }
  else if(this->angulo_deseado_del_motor<-120){
    this->angulo_deseado_del_motor=-120;
  }
  else{
    this->angulo_deseado_del_motor=angulo_deseado_del_motor_nuevo;
  }
}
float CreaStepper::get_ad(){
  return this->angulo_deseado_del_motor;
}
void CreaStepper::setVelocidad(float velocidad){
	float gp=0.9;
	float f = (velocidad *360)/(gp*60);
	this->T = 1000000/f;
	Serial.println(this->T);
}