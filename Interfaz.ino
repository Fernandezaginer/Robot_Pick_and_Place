
#include <Adafruit_GFX.h>    // Libreria que incluye las especificaciones de la pantalla tft con el controlador ILI9341
#include <TouchScreen.h>     // Libreria para trabajar con el panel tactil de la pantalla
#include <MCUFRIEND_kbv.h>   // Libreria para trabajar con la pantalla


// Pines de la pantalla táctil
#define XP 8
#define XM A2
#define YP A3
#define YM 9

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
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
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


// DoFs
float radio = 10.0;
float angulo = 180.0;
float altura = 20.0;
enum modo_gripper {PLACE = 0, PICK = 1}; 
bool gripper = PICK;


void setup() {
  Serial.begin(9600);
  tft_init();
  tft_parada_emergencia();
  tft_control_manual();
  tft_control_manual_print_q(radio, angulo, altura, gripper);
}



void loop() {

  modo_manual();

  // Y esperamos a que el usuario seleccione un jugador:
  /*
    int id = 0;
    do {
    int x, y;
    while (se_presiona_pantalla(&x, &y) == 0);
    while (se_presiona_pantalla(&x, &y) == 1);
    if (boton_salir(x, y) == 1) {
      k = 3;
      ret = 0;
      break;
    }
    delay(100);
    if (x >= 20 && x <= 180 && y >= 50 && y <= (50 + num_jug * 20)) {
      id = (y - 50) / 20;
      break;
    }

    } while (true);
  */
}

// Parada de emergencia:




void modo_manual() {

  // Ver si se tocan los botones
  set_qi_manual();

  // Update qi


  // Update muñeco


}


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
      Serial.print(mf[i]);
      Serial.write('\t');
    }
    Serial.println();

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
      Serial.print(mf[i]);
      Serial.write('\t');
    }
    Serial.println();

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
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[0] = 0;
  }

  if (mf[1] == 1) {
    if ((radio - RADIO_INCREMENTO) < RADIO_MIN) {
      radio = RADIO_MIN;
    }
    else {
      radio = radio - RADIO_INCREMENTO;
    }
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[1] = 0;
  }

  if (mf[2] == 1) {
    if ((angulo + ANGULO_INCREMENTO) > ANGULO_MAX) {
      angulo = ANGULO_MAX;
    }
    else {
      angulo = angulo + ANGULO_INCREMENTO;
    }
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[2] = 0;
  }

  if (mf[3] == 1) {
    if ((angulo - ANGULO_INCREMENTO) < ANGULO_MIN) {
      angulo = ANGULO_MIN;
    }
    else {
      angulo = angulo - ANGULO_INCREMENTO;
    }
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[3] = 0;
  }

  if (mf[4] == 1) {
    if ((altura + ALTURA_INCREMENTO) > ALTURA_MAX) {
      altura = ALTURA_MAX;
    }
    else {
      altura = altura + ALTURA_INCREMENTO;
    }
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[4] = 0;
  }

  if (mf[5] == 1) {
    if ((altura - ALTURA_INCREMENTO) < ALTURA_MIN) {
      altura = ALTURA_MIN;
    }
    else {
      altura = altura - ALTURA_INCREMENTO;
    }
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[5] = 0;
  }

  if(mf[6] == 1){
    if(gripper == PLACE){
      gripper = PICK;
    }
    else{
      gripper = PLACE;
    }
    tft_control_manual_print_q(radio, angulo, altura, gripper);
    mf[6] = 0;
  }

  // Pulsación larga:
  // proximamente...

}


//------------------------------------------------------------------------
//                      PANTALLAS DE LA INTERFAZ
//------------------------------------------------------------------------


void tft_control_manual() {
  tft_clear();
  tft_print_boton_salir();
  tft_formato_de_texto(BLUE, 2, 10, 10);
  tft.print("Control manual:");

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


void tft_control_manual_print_q(float q1, float q2, float q3, boolean gripper) {

  tft.fillRoundRect(210, 62, 65, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 66);
  tft.print(q1, 1);
  tft.fillRoundRect(210, 112, 65, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 116);
  tft.print(q2, 1);
  tft.fillRoundRect(210, 162, 65, 25, 0, WHITE);
  tft_formato_de_texto(BLUE, 2, 210, 166);
  tft.print(q3, 1);

  if(gripper == PICK){
    tft.fillRoundRect(180, 210, 125, 25, 3, RED);
    tft_formato_de_texto(BLACK, 2, 220, 215);
    tft.print("PICK");
  }
  if(gripper == PLACE){
    tft.fillRoundRect(180, 210, 125, 25, 3, GREEN);
    tft_formato_de_texto(BLACK, 2, 215, 215);
    tft.print("PLACE");
  }
  

}

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
//                        FUNCIONES DE LA TFT
//------------------------------------------------------------------------

void tft_init() {
  tft.reset();
  uint16_t id = tft.readID();
  if (id == 0xEFEF) {
    id = 0x9486;
  }
  tft.begin(id);
  tft.setRotation(1);
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
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
  bool pulsando = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pulsando) {
    *pos_x = map(p.y, 77, 910, 0, 320);
    *pos_y = map(p.x, 125, 900, 0, 240);
  }
  return pulsando;
}

boolean boton_salir(int x, int y) {
  boolean retorno = 0;
  if (x >= 290 && x <= 320 && y >= 0 && y <= 30) {
    retorno = 1;
  }
  return (retorno);
}
