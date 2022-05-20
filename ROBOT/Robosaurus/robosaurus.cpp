#include <robosaurus.h>

Robosaurus::Robosaurus(CreaStepper eje_R, CreaStepper eje_Z, CreaStepper eje_Th, Gripper MPG, Mpu9250 MPU, EncoderAbsoluto ENC, byte fin_de_carrera, double kp, double ki){
	this->_eje_R = eje_R;
	this->_eje_Z = eje_Z;
	this->_eje_Th = eje_Th;
	
	this->_MPG = MPG;
	this->_MPU = MPU;
	this->_ENC = ENC;
	
	this->_fin_de_carrera = fin_de_carrera;
	this->_err = 0.0;
	this->_integral_prev = 0.0;

	this->_kp = kp;
	this->_ki = ki;
	this->_Tp = 0.0;
	this->_Ti = 0.0;
	this->_vel = 0;

	this->_t_0 = millis();
	this->_th_ad = 180;
	
	this->_MPG_abierto = false;
	
	pinMode(this->_fin_de_carrera, INPUT_PULLUP);
}

void Robosaurus::begin(){
	/*INICIALIZACIÓN MPU*/
	_MPU.MPU9250_init();
	
	/*INICIALIZACIÓN MOTORES*/
	TCCR0A |= 0b00000011;
	TCCR3B &= 0b11111001;
	TCCR3B |= 0b00001001;
	
	/*INICIALIZACIÓN EJE Z*/
	_eje_Z.set_ad(0);
	_eje_Z.setVelocidad(300);
	_eje_Z.angulo = -9999;
	while(_eje_Z.angulo != 0){
		_eje_Z.actualizar();
		if(!digitalRead(_fin_de_carrera)){
			_eje_Z.angulo = 0;
		}
	}
	
	/*INICIALIZACIÓN EJE R*/
	_eje_R.setVelocidad(500);
	_eje_R.set_ad(0);
	
	/*INICIALIZACIÓN EJE Th*/
}

void Robosaurus::actualizar(){
	/*CONTROL EJE Z*/
    _eje_Z.actualizar();
	if(_eje_Z.get_ad() == 0 && _eje_Z.angulo > 0){
		_eje_Z.angulo = -9999;
	}
	if(!digitalRead(_fin_de_carrera)){
		_eje_Z.angulo = 0;
	}
	
	
	/*CONTROL EJE R*/
    _eje_R.angulo = _ENC.getPos_cm();
    _eje_R.actualizar();
	
	
	/*CONTROL EJE Th*/
    _MPU.MPU9250_read();
    //Controlador PI DIY
    if(millis() > _t_0 + 500){
      this->_err = (this->_MPU.read_angle_z() - this->_th_ad);
      this->_err = abs(this->_err)>5.0?this->_err:0;
      this->_Tp = this->_kp*this->_err;
      this->_Ti = this->_integral_prev + this->_err*this->_ki*0.5;
  
      this->_vel = this->_Tp + this->_Ti;
    }
    this->_eje_Th.setVelocidad(abs(this->_vel));
    this->_eje_Th.set_ad(this->_vel<0?999:-999);
    this->_eje_Th.angulo = 180;
    
    this->_eje_Th.actualizar();
    //Serial.println(this->_err);
    //Serial.print(this->_th_ad); Serial.print(",");
    //Serial.println(this->_MPU.read_angle_z());

}

void Robosaurus::setR(float R){
	this->_eje_R.set_ad(R);
}
void Robosaurus::setZ(float Z){
	this->_eje_Z.set_ad(Z);
}
void Robosaurus::setTh(float Th){
	this->_th_ad = Th;
}
void Robosaurus::setGripper(bool Open){
	if(Open){
		this->abrirGripper();
	}
	else{
		this->cerrarGripper();
	}
}

void Robosaurus::abrirGripper(){
	if(!this->_MPG_abierto){
		this->_MPG.abrir_gripper();
		this->_MPG_abierto = true;
	}
}
void Robosaurus::cerrarGripper(){
	if(this->_MPG_abierto){
		this->_MPG.cerrar_gripper();
		this->_MPG_abierto = false;
	}
}
void Robosaurus::toggleGripper(){
	if(this->_MPG_abierto){
		this->_MPG.cerrar_gripper();
		this->_MPG_abierto = false;
	}
	else{
		this->_MPG.abrir_gripper();
		this->_MPG_abierto = true;
	}
}