
#include <Adafruit_GFX.h>    // Libreria que incluye las especificaciones de la pantalla tft con el controlador ILI9341
#include <TouchScreen.h>     // Libreria para trabajar con el panel tactil de la pantalla
#include <MCUFRIEND_kbv.h>   // Libreria para trabajar con la pantalla


// Pulsador de emergencia
#define P_EMER 22

// Pines de la pantalla táctil
#define TFT_XP 8
#define TFT_XM A2
#define TFT_YP A3
#define TFT_YM 9

// Configuraciones de la TFT
MCUFRIEND_kbv tft;
#define BLACK   0x0000
#define WHITE   0xFFFF
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define YELLOW  0xFFE0
#define PINK    0xF85C

// Pantalla tactil:
#define MINPRESSURE 160
#define MAXPRESSURE 1000
TouchScreen ts = TouchScreen(TFT_XP, TFT_YP, TFT_XM, TFT_YM, 300);
Adafruit_GFX_Button on_btn, off_btn;


// PARAMETROS:
#define T_MIN_REARME        10
#define ANGULO_MAX          360.0
#define ANGULO_MIN          0.0
#define ANGULO_INCREMENTO   5.0
#define ANGULO_INCREMENTOL  5.0
#define ALTURA_MAX          20.0
#define ALTURA_MIN          0.0
#define ALTURA_INCREMENTO   1.0
#define ALTURA_INCREMENTOL  1.0
#define RADIO_MAX           35.0
#define RADIO_MIN           10.0
#define RADIO_INCREMENTO    1.0
#define RADIO_INCREMENTOL    1.0
#define X_INCREMENTO  0.5
#define X_INCREMENTOL 0.5
#define Y_INCREMENTO  0.5
#define Y_INCREMENTOL 0.5
#define Z_INCREMENTO  0.5
#define Z_INCREMENTOL 0.5


// Calse botón pantalla
class boton_t {
  public:
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    boton_t(int v1, int v2, int v3, int v4): x_min(v1), x_max(v2), y_min(v3), y_max(v4) {};
    bool is_pressed(int x, int y) const {
      if ( x >= x_min && x <= x_max && y >= y_min && y <= y_max) {
        return 1;
      }
      return 0;
    }
};


// Clase coordenadas
class coordenadas {
  public:
    coordenadas() : x(RADIO_MIN), y(0.0), z(ALTURA_MAX), g(0.0) {}
    float x;
    float y;
    float z;
    bool g;

    bool ver_si_ok() {
      return (getR() <= RADIO_MAX && getR() >= RADIO_MIN && getA() <= ANGULO_MAX && getA() >= ANGULO_MIN && getZ() <= ALTURA_MAX && getZ() >= ALTURA_MIN);
    }

    void convert_q_to_x(float radio, float angulo, float altura, boolean gripper) {
      x = radio * cos(angulo * (3.1416 / 180.0));
      y = radio * sin(angulo * (3.1416 / 180.0));
      z = altura;
      g = gripper;
    }

    float getR() {
      return sqrt(x * x + y * y);
    }
    float getA() {
      float aux = atan2(y, x) * (180 / 3.1416);
      if (y < 0) {
        aux += 360.0;
      }
      return aux;
    }
    float getZ() {
      return z;
    }
};


// DoFs
float radio = 10.0;
float angulo = 180.0;
float altura = 20.0;
coordenadas cord;
enum modo_gripper {PLACE = 0, PICK = 1};
bool gripper = PICK;


void tft_control_manual_print_xi(coordenadas cc);


// Modo interfaz:
enum modo_t {MENU, EMERGENCIA, MANUAL_Q, MANUAL_X, SECUENCIAS};
modo_t mode = MENU;
boolean salir = 0;
boolean emergencia = 0;


void setup() {

  // Inicialización
  Serial.begin(9600);  // Debug
  Serial1.begin(9600); // Comunicación con robot
  pinMode(P_EMER, INPUT_PULLUP);
  tft_init();

  cord.convert_q_to_x(radio, angulo, altura, gripper);
}




void loop() {

  // Interfaz segun modo:
  switch (mode) {
    case MENU:
      modo_menu();
      break;
    case EMERGENCIA:
      send_emergencia();
      tft_parada_emergencia();
      send_rearme();
      mode = MENU;
      break;
    case MANUAL_Q:
      modo_manual_q();
      break;
    case MANUAL_X:
      modo_manual_x();
      break;
    case SECUENCIAS:
      modo_secuencias();
      break;
  }


  // Cambio de modo desde fuera del menu:
  if (salir == true) {
    mode = MENU;
    salir = false;
  }
  if (emergencia == true) {
    mode = EMERGENCIA;
    emergencia = false;
  }


}



//------------------------------------------------------------------------
//                            MENU INTERFAZ
//------------------------------------------------------------------------

const boton_t Q(27, 97, 85, 155);
const boton_t X(125, 195, 85, 155);
const boton_t S(222, 292, 85, 155);


void modo_menu() {

  tft_menu();
  do {

    int x, y;
    while (se_presiona_pantalla(&x, &y) == 0) {
      if (emergencia) {
        break;
      }
    }
    while (se_presiona_pantalla(&x, &y) == 1) {
      if (emergencia) {
        break;
      }
    }

    if (Q.is_pressed(x, y)) {
      mode = MANUAL_Q;
    }

    if (X.is_pressed(x, y)) {
      mode = MANUAL_X;
    }

    if (S.is_pressed(x, y)) {
      mode = SECUENCIAS;
    }

    if (emergencia) {
      break;
    }

  } while (mode == MENU);

}


void tft_menu() {

  tft_clear();
  tft_formato_de_texto(BLUE, 2, 10, 10);
  tft.print("Menu robot:");

  // Imprimir botones en pantalla:
  tft.fillRoundRect(Q.x_min, Q.y_min, 70, 70, 5, GREEN);
  tft.fillRoundRect(Q.x_min + 5, Q.y_min + 5, 60, 60, 4, WHITE);
  tft_formato_de_texto(BLUE, 1, Q.x_min - 5, Q.y_min - 15);
  tft.print("Articulaciones:");
  tft_formato_de_texto(BLACK, 2, Q.x_min + 23, Q.y_min + 25);
  tft.print("qi");

  tft.fillRoundRect(X.x_min, X.y_min, 70, 70, 5, GREEN);
  tft.fillRoundRect(X.x_min + 5, X.y_min + 5, 60, 60, 5, WHITE);
  tft_formato_de_texto(BLUE, 1, X.x_min + 1, X.y_min - 15);
  tft.print("Coordenadas:");
  tft_formato_de_texto(BLACK, 2, X.x_min + 23, X.y_min + 25);
  tft.print("xi");

  tft.fillRoundRect(S.x_min, S.y_min, 70, 70, 5, GREEN);
  tft.fillRoundRect(S.x_min + 5, S.y_min + 5, 60, 60, 5, WHITE);
  tft_formato_de_texto(BLUE, 1, S.x_min + 3, S.y_min - 15);
  tft.print("Secuencias:");
  tft_formato_de_texto(BLACK, 2, S.x_min + 23, S.y_min + 25);
  tft.print("si");

}




#define T_MIN_REARME        10
#define ANGULO_MAX          360.0
#define ANGULO_MIN          0.0
#define ANGULO_INCREMENTO   5.0
#define ANGULO_INCREMENTOL  5.0
#define ALTURA_MAX          20.0
#define ALTURA_MIN          0.0
#define ALTURA_INCREMENTO   1.0
#define ALTURA_INCREMENTOL  1.0
#define RADIO_MAX           35.0
#define RADIO_MIN           10.0
#define RADIO_INCREMENTO    1.0
#define RADIO_INCREMENTOL    1.0


void modo_manual_x() {

  tft_control_manual_xi();
  tft_control_manual_print_xi(cord);

  do {
    set_xi_manual();
  } while (!salir && !emergencia);

  radio = cord.getR();
  angulo = cord.getA();
  altura = cord.getZ();
  gripper = cord.g;
}


void modo_manual_q() {

  tft_control_manual_qi();
  tft_control_manual_print_qi(radio, angulo, altura, gripper);
  do {
    set_qi_manual();
  } while (!salir && !emergencia);

  cord.convert_q_to_x(radio, angulo, altura, gripper);

}


void modo_secuencias() {
  tft_secuencias();

  do {
    set_secuencias();
  } while (!salir && !emergencia);

}


//------------------------------------------------------------------------
//                           ENVIO COORDENADAS
//------------------------------------------------------------------------

void send_qi(coordenadas c) {

  float aux;
  byte paquete[17];

  // Radio
  paquete[0] = 'r';
  aux = c.getR();
  float_to_4byte(&aux, &paquete[1]);

  // Angulo
  paquete[5] = 'a';
  aux = c.getA();
  float_to_4byte(&aux, &paquete[6]);

  // Altura
  paquete[10] = 'z';
  aux = c.getZ();
  float_to_4byte(&aux, &paquete[11]);

  // Gripper:
  paquete[15] = 'g';
  paquete[16] = c.g;

  for (int i = 0; i < sizeof(paquete); i++) {
    Serial1.write(paquete[i]);
    Serial1.flush();
  }

}


void send_sec(byte numero) {
  Serial1.write('s');
  Serial1.flush();
  Serial1.write(numero);
  Serial1.flush();
}


void send_emergencia() {
  Serial1.write('E');
  Serial1.flush();
}


void send_rearme() {
  Serial1.write('R');
  Serial1.flush();
}


// Guardar el float en &aux, &aux+1, &aux+2, &aux+3
void float_to_4byte(float* var, byte* aux) {
  byte* p = (byte*)var;
  for (char i = 3; i >= 0; i--) {
    *(aux + i) = *p;
    p++;
  }
}




//------------------------------------------------------------------------
//                    DEFINICION DE LAS COORDENADAS
//------------------------------------------------------------------------


void set_qi_manual() {

  // Marcas de flanco y de pulso, detectar el flanco negativo de la pulsacion
  static bool mf[7] = {0, 0, 0, 0, 0, 0, 0};
  static bool mp[7] = {0, 0, 0, 0, 0, 0, 0};

  // Botones presionables:
  const boton_t RM(285, 310, 62, 87);
  const boton_t Rm(185, 210, 62, 87);
  const boton_t AM(285, 310, 112, 137);
  const boton_t Am(185, 210, 112, 137);
  const boton_t HM(285, 310, 162, 187);
  const boton_t Hm(185, 210, 162, 187);
  const boton_t Gp(180, 305, 210, 235);
  const boton_t botones[7] = {RM, Rm, AM, Am, HM, Hm, Gp};

  // Detectar si hay algún boton presionado
  int x, y;
  x = 0;
  y = 0;

  // Comprobar varias veces si se presiona el boton con un or
  if (se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y)) {

    for (int i = 0; i < 7; i++) {
      if (mp[i] != botones[i].is_pressed(x, y) && botones[i].is_pressed(x, y) == 0) {
        mf[i] = 1;
      }
      else {
        mf[i] = 0;
      }
    }

    for (int i = 0; i < 7; i++) {
      mp[i] = botones[i].is_pressed(x, y);
    }
  }

  else {

    for (int i = 0; i < 7; i++) {
      if (mp[i] != botones[i].is_pressed(x, y) && botones[i].is_pressed(x, y) == 0) {
        mf[i] = 1;
      }
      else {
        mf[i] = 0;
      }
    }

    for (int i = 0; i < 7; i++) {
      mp[i] = 0;
    }
  }




  // UNA PULSACION

  if (mf[0] == 1) {
    if ((radio + RADIO_INCREMENTO) > RADIO_MAX) {
      radio = RADIO_MAX;
    }
    else {
      radio = radio + RADIO_INCREMENTO;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[1] = 0;
  }

  if (mf[1] == 1) {
    if ((radio - RADIO_INCREMENTO) < RADIO_MIN) {
      radio = RADIO_MIN;
    }
    else {
      radio = radio - RADIO_INCREMENTO;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[1] = 0;
  }

  if (mf[2] == 1) {
    if ((angulo + ANGULO_INCREMENTO) > ANGULO_MAX) {
      angulo = ANGULO_MAX;
    }
    else {
      angulo = angulo + ANGULO_INCREMENTO;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[2] = 0;
  }

  if (mf[3] == 1) {
    if ((angulo - ANGULO_INCREMENTO) < ANGULO_MIN) {
      angulo = ANGULO_MIN;
    }
    else {
      angulo = angulo - ANGULO_INCREMENTO;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[3] = 0;
  }

  if (mf[4] == 1) {
    if ((altura + ALTURA_INCREMENTO) > ALTURA_MAX) {
      altura = ALTURA_MAX;
    }
    else {
      altura = altura + ALTURA_INCREMENTO;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[4] = 0;
  }

  if (mf[5] == 1) {
    if ((altura - ALTURA_INCREMENTO) < ALTURA_MIN) {
      altura = ALTURA_MIN;
    }
    else {
      altura = altura - ALTURA_INCREMENTO;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[5] = 0;
  }

  if (mf[6] == 1) {
    if (gripper == PLACE) {
      gripper = PICK;
    }
    else {
      gripper = PLACE;
    }
    tft_control_manual_print_qi(radio, angulo, altura, gripper);
    mf[6] = 0;
  }

}



void set_xi_manual() {

  // Marcas de flanco y de pulso, detectar el flanco negativo de la pulsacion
  static bool mf[7] = {0, 0, 0, 0, 0, 0, 0};
  static bool mp[7] = {0, 0, 0, 0, 0, 0, 0};

  // Botones presionables:
  const boton_t XM(285, 310, 62, 87);
  const boton_t Xm(185, 210, 62, 87);
  const boton_t YM(285, 310, 112, 137);
  const boton_t Ym(185, 210, 112, 137);
  const boton_t ZM(285, 310, 162, 187);
  const boton_t Zm(185, 210, 162, 187);
  const boton_t Gp(180, 305, 210, 235);
  const boton_t botones[7] = {XM, Xm, YM, Ym, ZM, Zm, Gp};

  // Detectar si hay algún boton presionado
  int x, y;
  x = 0;
  y = 0;

  // Comprobar varias veces si se presiona el boton con un or
  if (se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y)) {

    for (int i = 0; i < 7; i++) {
      if (mp[i] != botones[i].is_pressed(x, y) && botones[i].is_pressed(x, y) == 0) {
        mf[i] = 1;
      }
      else {
        mf[i] = 0;
      }
    }
    for (int i = 0; i < 7; i++) {
      mp[i] = botones[i].is_pressed(x, y);
    }
  }

  else {

    for (int i = 0; i < 7; i++) {
      if (mp[i] != botones[i].is_pressed(x, y) && botones[i].is_pressed(x, y) == 0) {
        mf[i] = 1;
      }
      else {
        mf[i] = 0;
      }
    }

    for (int i = 0; i < 7; i++) {
      mp[i] = 0;
    }
  }


  // UNA PULSACION
  if (mf[0] == 1) {
    coordenadas aux;
    aux = cord;
    aux.x += X_INCREMENTO;
    if (aux.ver_si_ok()) {
      cord = aux;
      tft_control_manual_print_xi(cord);
    }
    mf[0] = 0;
  }

  if (mf[1] == 1) {
    coordenadas aux;
    aux = cord;
    aux.x -= X_INCREMENTO;
    if (aux.ver_si_ok()) {
      cord = aux;
      tft_control_manual_print_xi(cord);
    }
    mf[0] = 0;
  }

  if (mf[2] == 1) {
    coordenadas aux;
    aux = cord;
    aux.y += Y_INCREMENTO;
    if (aux.ver_si_ok()) {
      cord = aux;
      tft_control_manual_print_xi(cord);
    }
    mf[0] = 0;
  }

  if (mf[3] == 1) {
    coordenadas aux;
    aux = cord;
    aux.y -= Y_INCREMENTO;
    if (aux.ver_si_ok()) {
      cord = aux;
      tft_control_manual_print_xi(cord);
    }
    mf[0] = 0;
  }

  if (mf[4] == 1) {
    coordenadas aux;
    aux = cord;
    aux.z += Z_INCREMENTO;
    if (aux.ver_si_ok()) {
      cord = aux;
      tft_control_manual_print_xi(cord);
    }
    mf[0] = 0;
  }

  if (mf[5] == 1) {
    coordenadas aux;
    aux = cord;
    aux.z -= Z_INCREMENTO;
    if (aux.ver_si_ok()) {
      cord = aux;
      tft_control_manual_print_xi(cord);
    }
    mf[0] = 0;
  }

  if (mf[6] == 1) {
    if (cord.g == PLACE) {
      cord.g = PICK;
    }
    else {
      cord.g = PLACE;
    }
    tft_control_manual_print_xi(cord);
    mf[6] = 0;
  }



}



//------------------------------------------------------------------------
//              PANTALLAS DE LA INTERFAZ MANUAL r, h, a, g
//------------------------------------------------------------------------


void tft_control_manual_qi() {
  tft_clear();
  tft_print_boton_salir();
  tft_formato_de_texto(BLUE, 2, 10, 10);
  tft.print("Control manual qi:");

  // Botones de interación
  tft_formato_de_texto(BLUE, 2, 180, 40);
  tft.print("Radio:");
  tft_print_boton_qi(180, 60);
  tft_formato_de_texto(BLUE, 2, 180, 90);
  tft.print("Angulo:");
  tft_print_boton_qi(180, 110);
  tft_formato_de_texto(BLUE, 2, 180, 140);
  tft.print("Altura:");
  tft_print_boton_qi(180, 160);
  tft_formato_de_texto(BLUE, 2, 180, 190);
  tft.print("Gripper:");

  // Cajas separadoras:
  tft.drawFastHLine(177, 37, 131, BLACK);
  tft.drawFastHLine(177, 87, 131, BLACK);
  tft.drawFastHLine(177, 137, 131, BLACK);
  tft.drawFastHLine(177, 187, 131, BLACK);
  tft.drawFastHLine(177, 237, 131, BLACK);
  tft.drawFastVLine(177, 37, 200, BLACK);
  tft.drawFastVLine(308, 37, 200, BLACK);
}


void tft_control_manual_print_qi(float q1, float q2, float q3, boolean gripper) {

  // Envío de las coordenadas
  coordenadas aux;
  aux.convert_q_to_x(q1, q2, q3, gripper);
  send_qi(aux);

  tft.fillRoundRect(210, 62, 65, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 66);
  tft.print(q1, 1);
  tft.fillRoundRect(210, 112, 65, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 116);
  tft.print(q2, 1);
  tft.fillRoundRect(210, 162, 65, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 166);
  tft.print(q3, 1);

  if (gripper == PICK) {
    tft.fillRoundRect(180, 210, 125, 25, 3, RED);
    tft_formato_de_texto(BLACK, 2, 220, 215);
    tft.print("PICK");
  }
  if (gripper == PLACE) {
    tft.fillRoundRect(180, 210, 125, 25, 3, GREEN);
    tft_formato_de_texto(BLACK, 2, 215, 215);
    tft.print("PLACE");
  }

}



//------------------------------------------------------------------------
//              PANTALLAS DE LA INTERFAZ MANUAL x, y, z, g
//------------------------------------------------------------------------


void tft_control_manual_xi() {
  tft_clear();
  tft_print_boton_salir();
  tft_formato_de_texto(BLUE, 2, 10, 10);
  tft.print("Control manual xi:");

  // Botones de interación
  tft_formato_de_texto(BLUE, 2, 180, 40);
  tft.print("X:");
  tft_print_boton_qi(180, 60);
  tft_formato_de_texto(BLUE, 2, 180, 90);
  tft.print("Y:");
  tft_print_boton_qi(180, 110);
  tft_formato_de_texto(BLUE, 2, 180, 140);
  tft.print("Z:");
  tft_print_boton_qi(180, 160);
  tft_formato_de_texto(BLUE, 2, 180, 190);
  tft.print("Gripper:");

  // Cajas separadoras:
  tft.drawFastHLine(177, 37, 131, BLACK);
  tft.drawFastHLine(177, 87, 131, BLACK);
  tft.drawFastHLine(177, 137, 131, BLACK);
  tft.drawFastHLine(177, 187, 131, BLACK);
  tft.drawFastHLine(177, 237, 131, BLACK);
  tft.drawFastVLine(177, 37, 200, BLACK);
  tft.drawFastVLine(308, 37, 200, BLACK);
}


void tft_control_manual_print_xi(coordenadas cc) {

  // Envío de las coordenadas
  send_qi(cc);

  tft.fillRoundRect(210, 60, 70, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 208, 66);
  tft.print(cc.x, 2);
  tft.fillRoundRect(210, 110, 70, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 208, 116);
  tft.print(cc.y, 2);
  tft.fillRoundRect(210, 160, 70, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 166);
  tft.print(cc.z, 1);

  if (cc.g == PICK) {
    tft.fillRoundRect(180, 208, 125, 25, 3, RED);
    tft_formato_de_texto(BLACK, 2, 220, 215);
    tft.print("PICK");
  }
  if (cc.g == PLACE) {
    tft.fillRoundRect(180, 208, 125, 25, 3, GREEN);
    tft_formato_de_texto(BLACK, 2, 215, 215);
    tft.print("PLACE");
  }

}




//------------------------------------------------------------------------
//              PANTALLAS DE LA INTERFAZ SECUENCIAS
//------------------------------------------------------------------------


const boton_t S1(27, 97, 40, 110);
const boton_t S2(125, 195, 40, 110);
const boton_t S3(222, 292, 40, 110);
const boton_t S4(27, 97, 140, 210);
const boton_t S5(125, 195, 140, 210);
const boton_t S6(222, 292, 140, 210);


void tft_secuencias() {

  tft_clear();
  tft_print_boton_salir();
  tft_formato_de_texto(BLUE, 2, 10, 10);
  tft.print("Sec. Pick and Place:");

  // Print botones
  // Imprimir botones en pantalla:
  const boton_t b[6] = {S1, S2, S3, S4, S5, S6};
  uint16_t Colores[6] = {RED, BLUE, GREEN, TFT_YELLOW, PINK, TFT_ORANGE};

  for (int i = 0; i < 6; i++) {
    tft.fillRoundRect(b[i].x_min, b[i].y_min, 70, 70, 5, Colores[i]);
    tft.fillRoundRect(b[i].x_min + 5, b[i].y_min + 5, 60, 60, 4, WHITE);
    tft_formato_de_texto(BLACK, 2, b[i].x_min + 23, b[i].y_min + 25);
    tft.print("S");
    tft.print(i + 1);
  }



}

void set_secuencias() {

  // Marcas de flanco y de pulso, detectar el flanco negativo de la pulsacion
  static bool mf[6] = {0, 0, 0, 0, 0, 0};
  static bool mp[6] = {0, 0, 0, 0, 0, 0};

  // Botones presionables:
  const boton_t botones[6] = {S1, S2, S3, S4, S5, S6};

  // Detectar si hay algún boton presionado
  int x, y;
  x = 0;
  y = 0;

  // Comprobar varias veces si se presiona el boton con un or
  if (se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y) || se_presiona_pantalla(&x, &y)) {

    for (int i = 0; i < 6; i++) {
      if (mp[i] != botones[i].is_pressed(x, y) && botones[i].is_pressed(x, y) == 0) {
        mf[i] = 1;
      }
      else {
        mf[i] = 0;
      }
    }
    for (int i = 0; i < 6; i++) {
      mp[i] = botones[i].is_pressed(x, y);
    }
  }

  else {

    for (int i = 0; i < 6; i++) {
      if (mp[i] != botones[i].is_pressed(x, y) && botones[i].is_pressed(x, y) == 0) {
        mf[i] = 1;
      }
      else {
        mf[i] = 0;
      }
    }

    for (int i = 0; i < 6; i++) {
      mp[i] = 0;
    }
  }


  for (byte i = 0; i < 6; i++) {
    if (mf[i] == 1) {
      byte aux;
      aux = i + 1;
      Serial.println(aux, HEX);
      send_sec(aux);
      mf[i] = 0;
    }
  }



}



//------------------------------------------------------------------------
//                        PARADA DE EMERGENCIA
//------------------------------------------------------------------------


void tft_parada_emergencia() {

  // Carteles parada de emergencia
  tft_clear();
  tft_formato_de_texto(RED, 2, 10, 10);
  tft.print("Parada de emergencia:");
  tft_formato_de_texto(RED, 1, 10, 60);
  tft.print("Solucione la incidencia y verifique que el robot se  encuentra listo para la puesta en marcha");
  tft_formato_de_texto(BLUE, 2, 10, 110);
  tft.print("Rearme en");

  // Cuenta regresiva de seguridad:
  for (int i = T_MIN_REARME; i > 0; i--) {
    tft.fillRoundRect(120, 100, 60, 60, 5, WHITE);
    tft_formato_de_texto(BLUE, 2, 130, 110);
    tft.print(i);
    tft.print("s");
    delay(1000);
  }

  // Botón de rearme:
  tft_formato_de_texto(WHITE, 2, 10, 110);
  tft.print("Rearme en 1s");
  tft_formato_de_texto(BLUE, 2, 10, 110);
  tft.print("Pulse para rearmar:");
  tft.fillRoundRect(80, 150, 160, 60, 5, GREEN);
  tft_formato_de_texto(BLACK, 2, 120, 170);
  tft.print("Rearme");

  // Detectar el boton pulsado:
  while (true) {
    int x, y;
    while (se_presiona_pantalla(&x, &y) == 0);
    while (se_presiona_pantalla(&x, &y) == 1);
    if (x >= 80 && x <= 240 && y >= 150 && y <= 210) {
      break;
    }
  }

}



//------------------------------------------------------------------------
//                        FUNCIONES RUTINARIAS
//------------------------------------------------------------------------

void check_salir_y_emergencia(int x, int y) {
  if (boton_salir(x, y) == 1) {
    salir = true;
  }
  if (!digitalRead(P_EMER)) {
    emergencia = true;
  }
}




//------------------------------------------------------------------------
//                        FUNCIONES DE LA TFT
//------------------------------------------------------------------------

void tft_init() {
  tft.reset();
  uint16_t id = tft.readID();
  if (id == 0xEFEF) {
    id = 0x9486;
  }
  tft.begin(id);
  tft.setRotation(3);
  tft_clear();
}

void tft_clear() {
  tft.fillScreen(WHITE);
}

void tft_formato_de_texto(uint16_t color, byte tamano, int x, int y) {
  tft.setTextColor(color);
  tft.setTextSize(tamano);
  tft.setCursor(x, y);
}

void tft_print_boton_salir() {
  tft.fillRoundRect(290, 0, 30, 30, 3, RED);
  tft_formato_de_texto(WHITE, 3, 298, 4);
  tft.print('X');
}

void tft_print_boton_qi(int x, int y) {
  tft.fillRoundRect(x, y, 25, 25, 3, GREEN);
  tft_formato_de_texto(BLACK, 3, x + 5, y + 2);
  tft.print('-');
  tft.fillRoundRect(x + 100, y, 25, 25, 3, GREEN);
  tft_formato_de_texto(BLACK, 3, x + 105, y + 2);
  tft.print('+');
}

boolean se_presiona_pantalla(int* pos_x, int* pos_y) {
  TSPoint p = ts.getPoint();
  pinMode(TFT_YP, OUTPUT);
  pinMode(TFT_XM, OUTPUT);
  digitalWrite(TFT_YP, HIGH);
  digitalWrite(TFT_XM, HIGH);
  bool pulsando = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pulsando) {
    *pos_x = map(p.y, 77, 910, 0, 320);
    *pos_y = map(p.x, 125, 900, 0, 240);
    // Para orientacion invertida:
    *pos_x = 320 - *pos_x;
    *pos_y = 240 - *pos_y;
    check_salir_y_emergencia(*pos_x, *pos_y);
  }
  check_salir_y_emergencia(0, 0);
  return pulsando;
}

boolean boton_salir(int x, int y) {
  boolean retorno = 0;
  if (x >= 290 && x <= 320 && y >= 0 && y <= 30) {
    retorno = 1;
  }
  return (retorno);
}
