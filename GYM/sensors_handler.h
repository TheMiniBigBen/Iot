#ifndef SENSORS_HANDLER_H
#define SENSORS_HANDLER_H

#include <DHT.h> // Librería para el sensor DHT11
#include "firebase_handler.h" // Incluye tu archivo Firebase
#include "web_server.h" // Incluye tu archivo de servidor web si lo necesitas aquí
// Puedes agregar otras librerías necesarias

// --- Definiciones de Pines (basado en Sensores.docx) --- 

// DHT11
#define DHT11_PIN_ZONA1 GPIO_NUM_15 // DHT11 Zona 1 -> GPIO 15 
#define DHT11_PIN_ZONA2 GPIO_NUM_2  // DHT11 Zona 2 -> GPIO 2 
#define DHT11_PIN_ZONA3 GPIO_NUM_4  // DHT11 Zona 3 -> GPIO 4 
#define DHTTYPE DHT11 // Tipo de sensor, puedes cambiar a DHT22 si lo usas

// LDR
#define LDR_PIN_ZONA1 GPIO_NUM_34 // LDR Zona 1 -> GPIO 34 (ADC1_CH6) 
#define LDR_PIN_ZONA2 GPIO_NUM_35 // LDR Zona 2 -> GPIO 35 (ADC1_CH7) 
#define LDR_PIN_ZONA3 GPIO_NUM_32 // LDR Zona 3 -> GPIO 32 (ADC1_CH4) 

// PIR
#define PIR_PIN_BAÑO GPIO_NUM_16 // PIR Baño -> GPIO 16 

// Relés (Actuadores)
#define RELAY_PIN_BAÑO GPIO_NUM_17  // Relay-Baño -> GPIO 17 
#define RELAY_PIN_CLIMA GPIO_NUM_19 // Relay-Clima -> GPIO 19 
#define RELAY_PIN_FOCOS GPIO_NUM_18 // Relay-Focos -> GPIO 18 

// --- Instancias de Sensores ---
DHT dht1(DHT11_PIN_ZONA1, DHTTYPE);
DHT dht2(DHT11_PIN_ZONA2, DHTTYPE);
DHT dht3(DHT11_PIN_ZONA3, DHTTYPE);

// --- Variables para almacenar los valores de los sensores ---
// Temperatura y Humedad
float temp1 = 0, hum1 = 0;
float temp2 = 0, hum2 = 0;
float temp3 = 0, hum3 = 0;

// Luz (LDR)
int luz1 = 0;
int luz2 = 0;
int luz3 = 0;

// Movimiento (PIR)
int movimiento_baño = LOW; // LOW si no hay movimiento, HIGH si hay 

// --- Variables para estado de Relés ---
bool relay_bano_estado = false;  // false = apagado, true = encendido
bool relay_clima_estado = false;
bool relay_focos_estado = false;

// --- Prototipos de funciones ---
void inicializarSensores();
void leerSensores();
void enviarDatosSensoresFirebase();
void controlarActuadores();

// --- Implementación de funciones ---

void inicializarSensores() {
  dht1.begin();
  dht2.begin();
  dht3.begin();

  pinMode(PIR_PIN_BAÑO, INPUT); // PIR como entrada 

  // Relés como salida. Se inicializan en estado "apagado" (HIGH si activan con LOW) 
  pinMode(RELAY_PIN_BAÑO, OUTPUT);
  digitalWrite(RELAY_PIN_BAÑO, HIGH); // Apagado inicial
  pinMode(RELAY_PIN_CLIMA, OUTPUT);
  digitalWrite(RELAY_PIN_CLIMA, HIGH); // Apagado inicial
  pinMode(RELAY_PIN_FOCOS, OUTPUT);
  digitalWrite(RELAY_PIN_FOCOS, HIGH); // Apagado inicial

  Serial.println("✅ Sensores y actuadores inicializados.");
}

void leerSensores() {
  // Lectura DHT11
  hum1 = dht1.readHumidity();
  temp1 = dht1.readTemperature();
  if (isnan(hum1) || isnan(temp1)) {
    Serial.println("❌ Error leyendo DHT11 Zona 1!");
  } else {
    Serial.printf("Zona 1: Temp: %.1fC, Hum: %.1f%%\n", temp1, hum1);
  }

  hum2 = dht2.readHumidity();
  temp2 = dht2.readTemperature();
  if (isnan(hum2) || isnan(temp2)) {
    Serial.println("❌ Error leyendo DHT11 Zona 2!");
  } else {
    Serial.printf("Zona 2: Temp: %.1fC, Hum: %.1f%%\n", temp2, hum2);
  }
  
  hum3 = dht3.readHumidity();
  temp3 = dht3.readTemperature();
  if (isnan(hum3) || isnan(temp3)) {
    Serial.println("❌ Error leyendo DHT11 Zona 3!");
  } else {
    Serial.printf("Zona 3: Temp: %.1fC, Hum: %.1f%%\n", temp3, hum3);
  }

  // Lectura LDR (valores analógicos de 0 a 4095 en ESP32)
  luz1 = analogRead(LDR_PIN_ZONA1);
  luz2 = analogRead(LDR_PIN_ZONA2);
  luz3 = analogRead(LDR_PIN_ZONA3);
  Serial.printf("Zona 1: Luz: %d\n", luz1);
  Serial.printf("Zona 2: Luz: %d\n", luz2);
  Serial.printf("Zona 3: Luz: %d\n", luz3);

  // Lectura PIR
  movimiento_baño = digitalRead(PIR_PIN_BAÑO);
  Serial.printf("Baño: Movimiento: %s\n", (movimiento_baño == HIGH ? "Detectado" : "No detectado"));
}

void enviarDatosSensoresFirebase() {
  if (!Firebase.ready()) {
    Serial.println("❌ Firebase no está listo para enviar datos.");
    return;
  }

  // Objeto JSON para la Zona 1
  FirebaseJson jsonZona1;
  jsonZona1.set("temperatura", String(temp1, 1)); // Formatear a 1 decimal
  jsonZona1.set("humedad", String(hum1, 1));
  jsonZona1.set("luz", luz1);

  // Objeto JSON para la Zona 2
  FirebaseJson jsonZona2;
  jsonZona2.set("temperatura", String(temp2, 1));
  jsonZona2.set("humedad", String(hum2, 1));
  jsonZona2.set("luz", luz2);

  // Objeto JSON para la Zona 3
  FirebaseJson jsonZona3;
  jsonZona3.set("temperatura", String(temp3, 1));
  jsonZona3.set("humedad", String(hum3, 1));
  jsonZona3.set("luz", luz3);

  // Objeto JSON para el Baño
  FirebaseJson jsonBano;
  jsonBano.set("movimiento", (movimiento_baño == HIGH)); // True/False

  // Objeto JSON para el estado de los relés
  FirebaseJson jsonActuadores;
  jsonActuadores.set("luz_bano", relay_bano_estado);
  jsonActuadores.set("clima", relay_clima_estado);
  jsonActuadores.set("focos_generales", relay_focos_estado);


  // Enviar los datos a Firebase
  // Puedes usar diferentes rutas para organizar los datos
  if (Firebase.RTDB.setJSON(&fbdo, "/gimnasio/zona1", &jsonZona1)) {
    Serial.println("✅ Datos Zona 1 enviados a Firebase.");
  } else {
    Serial.print("❌ Error enviando Zona 1: ");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.setJSON(&fbdo, "/gimnasio/zona2", &jsonZona2)) {
    Serial.println("✅ Datos Zona 2 enviados a Firebase.");
  } else {
    Serial.print("❌ Error enviando Zona 2: ");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.setJSON(&fbdo, "/gimnasio/zona3", &jsonZona3)) {
    Serial.println("✅ Datos Zona 3 enviados a Firebase.");
  } else {
    Serial.print("❌ Error enviando Zona 3: ");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.setJSON(&fbdo, "/gimnasio/bano", &jsonBano)) {
    Serial.println("✅ Datos Baño enviados a Firebase.");
  } else {
    Serial.print("❌ Error enviando Baño: ");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.setJSON(&fbdo, "/gimnasio/actuadores", &jsonActuadores)) {
    Serial.println("✅ Datos actuadores enviados a Firebase.");
  } else {
    Serial.print("❌ Error enviando actuadores: ");
    Serial.println(fbdo.errorReason());
  }
}

// Controla los actuadores basados en la lógica definida 
void controlarActuadores() {
  // Zona 1: DHT11 (GPIO 15), LDR (GPIO 34) -> Relay-Clima (GPIO 19)
  // Lógica: Si hay alta temperatura (>28C) o luz excesiva (LDR < 500) -> activar ventilación
  // (Asumo que un valor bajo del LDR significa mucha luz)
  // 
  if (temp1 > 28.0 || luz1 < 500) { 
    if (!relay_clima_estado) { // Solo si no está encendido
      digitalWrite(RELAY_PIN_CLIMA, LOW); // Activar relé (asumo LOW activa) 
      relay_clima_estado = true;
      Serial.println("🌬️ Clima ACTIVADO (Zona 1: Temp alta o Luz excesiva)");
    }
  } else {
    if (relay_clima_estado) { // Solo si está encendido
      digitalWrite(RELAY_PIN_CLIMA, HIGH); // Desactivar relé
      relay_clima_estado = false;
      Serial.println("🍃 Clima DESACTIVADO (Zona 1)");
    }
  }

  // Zona 2: DHT11 (GPIO 2), LDR (GPIO 35) -> Relay-Focos (GPIO 18)
  // Lógica: Si no hay luz (LDR > 2000) -> encender focos automáticamente
  // 
  if (luz2 > 2000) { // Valor alto de LDR = oscuridad
    if (!relay_focos_estado) {
      digitalWrite(RELAY_PIN_FOCOS, LOW); // Activar relé 
      relay_focos_estado = true;
      Serial.println("💡 Focos ACTIVADOS (Zona 2: Oscuridad)");
    }
  } else {
    if (relay_focos_estado) {
      digitalWrite(RELAY_PIN_FOCOS, HIGH); // Desactivar relé
      relay_focos_estado = false;
      Serial.println("🌑 Focos DESACTIVADOS (Zona 2)");
    }
  }

  // Baño: PIR (GPIO 16) -> Relay-Baño (GPIO 17)
  // Lógica: Si hay movimiento -> encender luz del baño, si no -> apagar
  // 
  if (movimiento_baño == HIGH) {
    if (!relay_bano_estado) {
      digitalWrite(RELAY_PIN_BAÑO, LOW); // Activar relé 
      relay_bano_estado = true;
      Serial.println("🚽 Luz Baño ACTIVADA (Movimiento)");
    }
  } else {
    if (relay_bano_estado) {
      digitalWrite(RELAY_PIN_BAÑO, HIGH); // Desactivar relé
      relay_bano_estado = false;
      Serial.println("⚫ Luz Baño DESACTIVADA (Sin movimiento)");
    }
  }
}

#endif