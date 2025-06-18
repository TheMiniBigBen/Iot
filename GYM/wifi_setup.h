#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFiManager.h>
#include <ESPmDNS.h>

void iniciarWiFi_y_mDNS() {
  WiFiManager wm;
  wm.setTitle("Sistema IoT Gimnasio");

  bool res = wm.autoConnect("GIMNASIO-SETUP", "admin123");

  if (!res) {
    Serial.println("❌ No se pudo conectar a WiFi");
  } else {
    Serial.println("✅ Conectado a WiFi");
  }

  if (MDNS.begin("gimnasio")) {
    Serial.println("✅ mDNS iniciado: http://gimnasio.local");
  } else {
    Serial.println("❌ Error iniciando mDNS");
  }
}

#endif
