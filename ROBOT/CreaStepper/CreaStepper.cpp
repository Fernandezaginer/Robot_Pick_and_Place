#include <Arduino.h>
#include <CreaStepper.h>

CreaStepper::CreaStepper(byte o_A1, byte o_A2,byte o_B1,byte o_B2, byte ena, float angulo_Inicio, float lim_inf, float lim_sup)
{
	this->o_A1 = o_A1;
	this->o_A2 = o_A2;
	this->o_B1 = o_B1;
	this->o_B2 = o_B2;
	this->ena=ena;
	
	this->angulo = this->angulo_inicial = angulo_Inicio;
	this->angulo_deseado_del_motor=90;
	this-> i=0;
	this->T = 500;
	
	this->_lim_inf = lim_inf;
	this->_lim_sup = _lim_sup;
	
	this->t_0 = micros();
	
	pinMode(this->o_A1, OUTPUT);
	pinMode(this->o_A2, OUTPUT);
	pinMode(this->o_B1, OUTPUT);
	pinMode(this->o_B2, OUTPUT);
}

void CreaStepper::actualizar(){
	if(micros() >= this->t_0 + this->T)
	{
		if(angulo - angulo_deseado_del_motor > 0.9){
			i--;
			darpaso();
			//angulo -= 0.9;
		}
		else if(angulo - angulo_deseado_del_motor < -0.9){
			i++;
			darpaso();
			//angulo += 0.9;
		}
		else{
			darparo();
		}
	}
}

void CreaStepper::darpaso(){
    digitalWrite(this->ena, HIGH);
    digitalWrite(this->o_A1,CreaStepper::micropasos[i %8][0]);
    digitalWrite(this->o_A2, CreaStepper::micropasos[i %8][1]);
    digitalWrite(this->o_B1, CreaStepper::micropasos[i %8][2]);
    digitalWrite(this->o_B2, CreaStepper::micropasos[i %8][3]);
}

void CreaStepper::darparo(){
    analogWrite(this->ena, 110);
    digitalWrite(this->o_A1, CreaStepper::micropasos[i %8][0]);
    digitalWrite(this->o_A2, CreaStepper::micropasos[i %8][1]);
    digitalWrite(this->o_B1, CreaStepper::micropasos[i %8][2]);
    digitalWrite(this->o_B2, CreaStepper::micropasos[i %8][3]);
    //delay(1000);
	//casa();
}
void CreaStepper::casa(){
  this->angulo_deseado_del_motor = this->angulo_inicial;
}
void CreaStepper::set_ad(float angulo_deseado_del_motor_nuevo){  
  if(this->angulo_deseado_del_motor > this->_lim_sup){
   this->angulo_deseado_del_motor = 120;
  }
  else if(this->angulo_deseado_del_motor < this->_lim_inf){
    this->angulo_deseado_del_motor = -120;
  }
  else{
    this->angulo_deseado_del_motor = angulo_deseado_del_motor_nuevo;
  }
}
float CreaStepper::get_ad(){
  return this->angulo_deseado_del_motor;
}
void CreaStepper::setVelocidad(float velocidad){
	float gp=0.9;
	float f = (velocidad *360)/(gp*60);
	this->T = 1000000/f;
	if(this->T < 400){
		this->T = 400;
	}
}