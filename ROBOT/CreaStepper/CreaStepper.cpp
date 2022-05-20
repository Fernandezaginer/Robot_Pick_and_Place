#include <Arduino.h>
#include <CreaStepper.h>

CreaStepper::CreaStepper(byte o_A1, byte o_A2,byte o_B1,byte o_B2, byte ena, float angulo_Inicio, float lim_inf, float lim_sup, float velocidad, float aceleracion, float velocidad_max)
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
	this->_lim_sup = lim_sup;
	
	this->_velocidad = velocidad;
	this->_velocidad_max = velocidad_max;
	this->_aceleracion = aceleracion;
	
	this->estado = 0;
	
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
			angulo -= 0.9;  //Control en cadena abierta, overriden en lazo cerrado;
		}
		else if(angulo - angulo_deseado_del_motor < -0.9){
			i++;
			darpaso();
			angulo += 0.9; //Control en cadena abierta, overriden en lazo cerrado;
		}
		else{
			darparo();
		}
		this->t_0 = micros();
	}
	//Aceleraciones:
	if(this->estado < decelerando){
		if(2*this->_aceleracion*(1-((this->angulo_deseado_del_motor - this->angulo)*this->_aceleracion/(this->_velocidad*this->_velocidad))) >= this->_aceleracion){
			estado = decelerando;
		}
	}
	else if(this->estado == acelerando && this->_velocidad >= this->_velocidad_max){
		estado = vcons;
	}
	this->_velocidad += this->estado*this->_aceleracion*(micros()-this->t_1);
	this->setVelocidad(this->_velocidad);
	this->t_1 = micros();
}

void CreaStepper::darpaso(){
	if(this->T > 127000){
		darparo();
		return;
	}
    digitalWrite(this->ena, HIGH);
    digitalWrite(this->o_A1,CreaStepper::micropasos[i %8][0]);
    digitalWrite(this->o_A2, CreaStepper::micropasos[i %8][1]);
    digitalWrite(this->o_B1, CreaStepper::micropasos[i %8][2]);
    digitalWrite(this->o_B2, CreaStepper::micropasos[i %8][3]);
}

void CreaStepper::darparo(){
    analogWrite(this->ena, 90);
    digitalWrite(this->o_A1, CreaStepper::micropasos[i %8][0]);
    digitalWrite(this->o_A2, CreaStepper::micropasos[i %8][1]);
    digitalWrite(this->o_B1, CreaStepper::micropasos[i %8][2]);
    digitalWrite(this->o_B2, CreaStepper::micropasos[i %8][3]);
}
void CreaStepper::casa(){
  this->angulo_deseado_del_motor = this->angulo_inicial;
}
void CreaStepper::set_ad(float angulo_deseado_del_motor_nuevo){  
  if(angulo_deseado_del_motor_nuevo > this->_lim_sup){
   this->angulo_deseado_del_motor = _lim_sup;
  }
  else if(angulo_deseado_del_motor_nuevo < this->_lim_inf){
    this->angulo_deseado_del_motor = _lim_inf;
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
}

const bool CreaStepper::micropasos[8][4] = {
    {1, 0, 0, 0}, //dependiendo de la pol será los dos primeros numeros 1 o 0 (uno es 1 y el otro es 0)=>igual //para el otro puente
    {1, 0, 1, 0}, // 1 0 pol 1, 00 despolarizado, 01 pol 2
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};