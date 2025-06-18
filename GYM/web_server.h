#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>

WebServer server(80);

void iniciarServidorWeb() {
  server.on("/", []() {
    server.send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head><title>Gimnasio IoT</title></head>
      <body style='font-family:sans-serif;text-align:center;'>
        <h2>Sistema IoT Gimnasio</h2>
        <p>Conectado correctamente.</p>
        <p>Si no ves datos en tu app, revisa Firebase o reinicia el dispositivo.</p>
      </body>
      </html>
    )rawliteral");
  });

  server.begin();
  Serial.println("✅ Servidor web iniciado en puerto 80");
}

void manejarWeb() {
  server.handleClient();
}

#endif
