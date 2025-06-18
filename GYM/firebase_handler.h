#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#include <Firebase_ESP_Client.h>

#define API_KEY "AIzaSyA4Tch4HHiDPkvBoyOq-xArXoEnZek4Bm0"
#define DATABASE_URL "https://gimnasio-setup-default-rtdb.firebaseio.com/"
// Mantén esta línea tal cual, con el placeholder, si así te funciona.
#define FIREBASE_AUTH_SECRET "TU_LEGACY_API_SECRET_AQUI" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void firebaseTokenStatusCallback(TokenInfo info);

void iniciarFirebase() {
  Serial.println("DEBUG: >> Entrando a iniciarFirebase()");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Mantén este bloque así si te funciona, incluso con el placeholder.
  auth.user.email = ""; 
  auth.user.password = ""; 
  config.signer.tokens.legacy_token = FIREBASE_AUTH_SECRET; 

  config.token_status_callback = firebaseTokenStatusCallback;

  Serial.print("DEBUG: API_KEY definida: ");
  Serial.println(API_KEY);
  Serial.print("DEBUG: DATABASE_URL definida: ");
  Serial.println(DATABASE_URL);

  Serial.print("DEBUG: config.api_key ANTES de begin: ");
  Serial.println(config.api_key.c_str());
  Serial.print("DEBUG: config.database_url ANTES de begin: ");
  Serial.println(config.database_url.c_str());

  Serial.println("DEBUG: Llamando a Firebase.begin()...");

  Firebase.begin(&config, &auth);

  if (Firebase.ready()) {
    Serial.println("✅ Firebase inicializado correctamente.");
  } else {
    Serial.println("❌ Firebase no se inicializó correctamente.");
    Serial.print("❌ Razón del error (pos-inicio): ");
    Serial.println(fbdo.errorReason()); 
  }

  Serial.println("DEBUG: << Saliendo de iniciarFirebase()");
}

void firebaseTokenStatusCallback(TokenInfo info) {
  // Puedes descomentar la siguiente línea para depurar el estado del token
  // Serial.printf("Token info: type = %s, status = %s\n", Firebase.TokenType2String(info.type), Firebase.TokenStatus2String(info.status));
}

void enviarDatosFirebase() { 
  time_t now = time(nullptr);
  struct tm timeinfo;
  char buffer[80];
  // Formato: Año-Mes-Día Hora:Minuto:Segundo
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now)); 
  String mensaje = String("Conexión realizada el: ") + buffer;

  if (Firebase.RTDB.setString(&fbdo, "/Conexion/mensaje", mensaje)) { // Escribe en /Conexion/mensaje
    Serial.println("✅ Mensaje de conexión enviado a Firebase.");
  } else {
    Serial.print("❌ Error Firebase al enviar mensaje de conexión: ");
    Serial.println(fbdo.errorReason());
  }
}
#endif