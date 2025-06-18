#include <Arduino.h>
#include <WiFi.h> // Necesario para WiFiManager
#include <Firebase_ESP_Client.h> // Asegúrate de que esta sea la librería unificada

// Incluye tus otros archivos de cabecera
#include "wifi_setup.h"
#include "firebase_handler.h"
#include "web_server.h"
#include "sensors_handler.h" // ¡Ahora incluimos el manejador de sensores!

// Variables para control de tiempo (opcional, para enviar datos cada cierto tiempo)
unsigned long lastSendTime = 0;
const long sendInterval = 5000; // Enviar datos cada 5 segundos

void setup() {
  Serial.begin(115200);
  delay(100);

  // Inicializar WiFi y mDNS
  iniciarWiFi_y_mDNS();

  // Esperar a que WiFi se conecte antes de sincronizar la hora y Firebase
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ Conexión WiFi establecida.");

  // Sincronizar hora con NTP (muy importante para Firebase)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Sincronizando hora con NTP...");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) { // Esperar a que la hora se sincronice (ajusta si necesitas más tiempo)
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\n✅ Hora NTP sincronizada correctamente.");


  // Inicializar Firebase
  iniciarFirebase();

  enviarDatosFirebase();
  
  // Inicializar Servidor Web
  iniciarServidorWeb();

  // Inicializar Sensores y Relés
  inicializarSensores();
}

void loop() {
  // Manejar el servidor web (para acceder a http://gimnasio.local)
  manejarWeb();

  // Leer sensores y controlar actuadores en cada ciclo de loop
  leerSensores();
  controlarActuadores();

  // Enviar datos a Firebase periódicamente o cuando haya cambios significativos
  // Aquí los enviamos cada 5 segundos, puedes ajustar la lógica para "solo si hay cambios"
  if (millis() - lastSendTime > sendInterval) {
    enviarDatosSensoresFirebase();
    lastSendTime = millis();
  }

  // Pequeña pausa para evitar lecturas de sensor demasiado rápidas o spam de Firebase
  delay(500); 
}