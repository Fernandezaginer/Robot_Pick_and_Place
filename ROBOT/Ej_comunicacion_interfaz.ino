
// Código a usar por el robot para leer datos de la interfaz (Teach Pendant)


// Clase de comunicación con la interfaz
class COM_Interfaz {
  public:

    float radio = 0.0;
    float angulo = 0.0;
    float altura = 0.0;
    bool gripper = 0.0;
    byte secuencia = 0;
    bool emergencia = 0;
    bool rearme = 0;

    // Inicio de la comunicación:
    void begin(){
      Serial2.begin(9600);
    }

    // Lectura de la inferfaz
    bool available() {

      if(!Serial2.available()){
        return 0;
      }

      while (Serial2.available() > 0) {
        char identificador;
        identificador = Serial2.read();

        switch (identificador) {
          case 'r': // Radio
            radio = lectura_float();
            rearme = 0;
            secuencia = 0;
            break;
          case 'a': // Angulo
            angulo = lectura_float();
            rearme = 0;
            secuencia = 0;
            break;
          case 'z': // Altura
            altura =lectura_float();
            rearme = 0;
            secuencia = 0;
            break;
          case 'g': // Gripper
            while(!Serial2.available());
            gripper = Serial2.read();
            rearme = 0;
            secuencia = 0;
            break;
          case 's': // Secuencia
            while(!Serial2.available());
            secuencia = Serial2.read();
            rearme = 0;
            break;
          case 'E': // Emergencia
            emergencia = 1;
            secuencia = 0;
            break;
          case 'R': // Rearme
            rearme = 1;
            emergencia = 0;
            break;

        }
        delay(5);
      }
      return 1;
    }

  private:

    // Lectura de un flotante
    float lectura_float() {
      byte cont = 0;
      float res;
      byte buffer[4];
      while (cont < sizeof(buffer)) {
        if (Serial2.available() > 0) {
          buffer[cont] = Serial2.read();
          cont++;
        }
      }
      _4byte_to_float(buffer, &res);
      return res;
    }

    // Conversión de los bytes a float
    void _4byte_to_float(byte* aux, float *out) {
      uint32_t mem_aux = 0;
      mem_aux |= aux[3];
      mem_aux |= (uint32_t)(aux[2]) << 8;
      mem_aux |= (uint32_t)(aux[1]) << 16;
      mem_aux |= (uint32_t)(aux[0]) << 24;
      *(out) = *((float*)&mem_aux);
    }

};


COM_Interfaz interfaz;



// Test unitario:
void setup() {
  Serial.begin(9600);
  interfaz.begin();
}

void loop(){

  if(interfaz.available()){
    Serial.println(interfaz.radio);
    Serial.println(interfaz.angulo);
    Serial.println(interfaz.altura);
    Serial.println(interfaz.gripper);
    Serial.println(interfaz.secuencia);
    Serial.println(interfaz.emergencia);
    Serial.println(interfaz.rearme);
  }
}
