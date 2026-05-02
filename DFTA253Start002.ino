// Le 16/03/26 11H36
// Le 16/03/26 à 12H43 fonctionne attention RGB inversé sur les APA106
#include <Adafruit_NeoPixel.h>

/* =========================================================
   CONFIGURATION HARDWARE
   ========================================================= */

const int PIN_VS_ADC   = A5;   // mesure VS via pont diviseur

const int PIN_SEUIL1   = D1;   // Seuil 1 (le plus élevé)
const int PIN_SEUIL2   = D2;
const int PIN_SEUIL3   = D3;
const int PIN_SEUIL4   = D4;   // Seuil 4 (le plus bas)

const int PIN_LED_BIN  = D7;   // Barrette binaire
const int PIN_LED_ZONE = D8;   // Barrette zones
const int PIN_LED_OV   = D6;   // LED surtension

const int PIN_SELECT_C = D10;  // Sélection capacité
const int PIN_BUZZER   = D9;   // Buzzer
const int PIN_RESET    = D0;   // Reset utilisateur (entrée)

const int NLEDS_BIN  = 8;
const int NLEDS_ZONE = 4;

Adafruit_NeoPixel stripBin (NLEDS_BIN,  PIN_LED_BIN,  NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripZone(NLEDS_ZONE, PIN_LED_ZONE, NEO_RGB + NEO_KHZ800);

/* =========================================================
   PARAMÈTRES DE MESURE
   ========================================================= */

const float VREF_ADC = 3.3;
const float FACTEUR_PONT = 137.36;   // pont 300k / 2.2k

/* =========================================================
   SEUILS DE DÉTECTION
   ========================================================= */

//const float SEUIL_SONDES_RAW = 2.0; // le 16/02/26 ce seuil de 2 est trop bas si on met 5 ça marche mieux

//_________________
const float SEUIL_SONDES_ON  = 5.0;
const float SEUIL_SONDES_OFF = 3.0;
//_________________

const float SEUIL_SURTENSION = 380.0;

/* =========================================================
   CORRECTION PONT DE DIODES
   ========================================================= */

const float DIODE_DROP = 1.2;
const float MIN_CORR_V = 2.0;

/* =========================================================
   VARIABLES
   ========================================================= */

bool  sondesOK    = false;
bool  grosC       = false;
float vs_affichee = 0.0;

/* =========================================================
   BUZZER GÉNÉRIQUE
   ========================================================= */

void buzzer(uint16_t duree_on, uint16_t duree_off, uint8_t nb_coups) {
  for (uint8_t i = 0; i < nb_coups; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(duree_on);
    digitalWrite(PIN_BUZZER, LOW);
    if (i < nb_coups - 1)
      delay(duree_off);
  }
}

/* =========================================================
   LECTURE ADC ROBUSTE
   ========================================================= */

float lireVS_brut() {

  analogRead(PIN_VS_ADC);   // purge
  delayMicroseconds(10);

  int adc = analogRead(PIN_VS_ADC);

  float vpont = (adc * VREF_ADC) / 4095.0;
  float vs    = vpont * FACTEUR_PONT;

  return vs;
}

/* =========================================================
   AFFICHAGE ZONES (D8)
   ========================================================= */

void afficherZones(float vs) {

  for (int i = 0; i < NLEDS_ZONE; i++)
    stripZone.setPixelColor(i, 0);

  if (vs > 300.0)
    stripZone.setPixelColor(0, stripZone.Color(255, 0, 0));
  else if (vs > 200.0)
    stripZone.setPixelColor(1, stripZone.Color(255, 120, 0));
  else if (vs > 100.0)
    stripZone.setPixelColor(2, stripZone.Color(255, 255, 0));
  else
    stripZone.setPixelColor(3, stripZone.Color(0, 255, 0));

  stripZone.show();
}

/* =========================================================
   RESET LOGICIEL
   ========================================================= */

void resetLogique() {

  vs_affichee = 0.0;
  sondesOK   = false;

  digitalWrite(PIN_SEUIL1, LOW);
  digitalWrite(PIN_SEUIL2, LOW);
  digitalWrite(PIN_SEUIL3, LOW);
  digitalWrite(PIN_SEUIL4, LOW);
  digitalWrite(PIN_LED_OV, LOW);

  for (int i = 0; i < NLEDS_BIN; i++)
    stripBin.setPixelColor(i, 0);

  for (int i = 0; i < NLEDS_ZONE; i++)
    stripZone.setPixelColor(i, 0);

  stripBin.show();
  stripZone.show();

  buzzer(80, 0, 1);   // bip court confirmation reset
}

/* =========================================================
   SETUP
   ========================================================= */

void setup() {

  analogReadResolution(12);

  pinMode(PIN_SEUIL1, OUTPUT);
  pinMode(PIN_SEUIL2, OUTPUT);
  pinMode(PIN_SEUIL3, OUTPUT);
  pinMode(PIN_SEUIL4, OUTPUT);
  pinMode(PIN_LED_OV, OUTPUT);

  pinMode(PIN_SELECT_C, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RESET, INPUT_PULLUP);

  digitalWrite(PIN_SEUIL1, LOW);
  digitalWrite(PIN_SEUIL2, LOW);
  digitalWrite(PIN_SEUIL3, LOW);
  digitalWrite(PIN_SEUIL4, LOW);
  digitalWrite(PIN_LED_OV, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  stripBin.begin();
  stripZone.begin();
  stripBin.show();
  stripZone.show();

  delay(300);

  buzzer(100, 100, 2);   // bip démarrage
}

/* =========================================================
   LOOP PRINCIPALE
   ========================================================= */

void loop() {

  /* ---------- RESET UTILISATEUR ---------- */

  if (digitalRead(PIN_RESET) == LOW) {
    resetLogique();
    delay(300);   // anti-rebond simple
    return;
  }

  /* ---------- LECTURE TENSION ---------- */

  float vs_raw = lireVS_brut();
  float vs = vs_raw;

  /* ---------- DÉTECTION SONDES ---------- */

  //sondesOK = (vs_raw > SEUIL_SONDES_RAW);


if (!sondesOK && vs_raw > SEUIL_SONDES_ON)
    sondesOK = true;

if (sondesOK && vs_raw < SEUIL_SONDES_OFF)
    sondesOK = false;


  /* ---------- CORRECTION DIODE ---------- */

  if (sondesOK && vs > MIN_CORR_V)
    vs += DIODE_DROP;

  /* ---------- MÉMORISATION AFFICHAGE ---------- */

  if (sondesOK)
    vs_affichee = vs;

  /* ---------- SÉLECTION CAPACITÉ ---------- */

  grosC = (digitalRead(PIN_SELECT_C) == LOW);

  /* ---------- SEUILS DYNAMIQUES ---------- */

  float S1, S2, S3, S4;

  if (grosC) {            // C > 500 µF
    S1 = 300.0;
    S2 = 220.0;
    S3 = 140.0;
    S4 =  50.0;
  } else {                // C ≤ 500 µF
    S1 = 250.0;
    S2 = 180.0;
    S3 = 100.0;
    S4 =  30.0;
  }

  /* ---------- LOGIQUE SEUILS ---------- */

  bool s1 = false, s2 = false, s3 = false, s4 = false;

  if (sondesOK) {
    if (vs <= S1) s1 = true;
    if (vs <= S2) s2 = true;
    if (vs <= S3) s3 = true;
    if (vs <= S4) s4 = true;
  }

  digitalWrite(PIN_SEUIL1, s1 ? HIGH : LOW);
  digitalWrite(PIN_SEUIL2, s2 ? HIGH : LOW);
  digitalWrite(PIN_SEUIL3, s3 ? HIGH : LOW);
  digitalWrite(PIN_SEUIL4, s4 ? HIGH : LOW);

  /* ---------- LED SURTENSION ---------- */

  digitalWrite(
    PIN_LED_OV,
    (sondesOK && vs > SEUIL_SURTENSION) ? HIGH : LOW
  );

  /* ---------- BARRETTE BINAIRE ---------- */

  for (int i = 0; i < NLEDS_BIN; i++)
    stripBin.setPixelColor(i, 0);

  uint16_t code = constrain((uint16_t)vs_affichee, 0, 255);

  for (int i = 0; i < NLEDS_BIN; i++) {
    if (code & (1 << i))
      stripBin.setPixelColor(i, stripBin.Color(0, 150, 0));
  }

  stripBin.show();

  /* ---------- BARRETTE ZONES ---------- */

  afficherZones(vs_affichee);

  delay(50);
}
