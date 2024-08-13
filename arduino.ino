// Biblioteca FastLED 3.5.0 (Daniel Garcia)
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "Dados.h"

const char* ssid = STASSID;
const char* password = STAPSK;

#define DATA_PIN D4
#define LARGURA 16
#define ALTURA 16
#define NUM_LEDS (LARGURA * ALTURA)

#define amarelo CRGB(255, 200, 0)
#define azul CRGB(0, 0, 255)
#define laranja CRGB(255, 128, 0)
#define roxo CRGB(128, 0, 255)
#define verde CRGB(0, 255, 0)
#define vermelho CRGB(255, 0, 0)

CRGB leds[NUM_LEDS];

ESP8266WebServer server(80);

static const char html[] PROGMEM = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/css/bootstrap.min.css\" /><link rel=\"stylesheet\" href=\"https://araujozb.github.io/wardice/style.css\" /><script src=\"https://cdn.jsdelivr.net/npm/jquery@3.5.1/dist/jquery.slim.min.js\"></script><script src=\"https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/js/bootstrap.bundle.min.js\"></script><title>WarDice</title></head><body><div class=\"limitador\"><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-md-4 offset-md-4 col-sm-6 offset-sm-3\"><div class=\"card my-3 shadow\"><div class=\"card-header\"><h2 class=\"m-0\">WarDice</h2></div><div class=\"card-body\"><div class=\"form-group\"><label for=\"atq\">Time que ataca</label><div class=\"row\"><button class=\"btn btn-atq\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('atq', 'azul')\"><img id=\"imgAtqAzul\" src=\"https://araujozb.github.io/wardice/img/Peao-Grow-Azul.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('atq', 'vermelho')\"><img src=\"https://araujozb.github.io/wardice/img/Peao-Grow-Vermelho.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('atq', 'verde')\"><img src=\"https://araujozb.github.io/wardice/img/Peao-Grow-Verde.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('atq', 'laranja')\"><img src=\"https://araujozb.github.io/wardice/img/PLaranja.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('atq', 'roxo')\"><img src=\"https://araujozb.github.io/wardice/img/PRoxo.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('atq', 'amarelo')\"><img src=\"https://araujozb.github.io/wardice/img/PAmarelo.png\" alt=\"\" style=\"height: 52px;\"></button></div></div><div class=\"form-group\"><label for=\"df\">Time que defende</label><div class=\"row\"><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('df', 'azul')\"><img src=\"https://araujozb.github.io/wardice/img/Peao-Grow-Azul.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn active\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('df', 'vermelho')\"><img src=\"https://araujozb.github.io/wardice/img/Peao-Grow-Vermelho.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('df', 'verde')\"><img src=\"https://araujozb.github.io/wardice/img/Peao-Grow-Verde.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('df', 'laranja')\"><img src=\"https://araujozb.github.io/wardice/img/PLaranja.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('df', 'roxo')\"><img src=\"https://araujozb.github.io/wardice/img/PRoxo.png\" alt=\"\" style=\"height: 52px;\"></button><button class=\"btn\" style=\"border: none; padding: 0;\" onclick=\"escolherTime('df', 'amarelo')\"><img src=\"https://araujozb.github.io/wardice/img/PAmarelo.png\" alt=\"\" style=\"height: 52px;\"></button></div></div><div class=\"form-group\"><label for=\"qntAtq\">Quantidade de dados do ataque</label><input type=\"number\" id=\"qntAtq\" min=\"1\" max=\"3\" value=\"1\" class=\"form-control\" /></div><div class=\"form-group\"><label for=\"qntDf\">Quantidade de dados da defesa</label><input type=\"number\" id=\"qntDf\" min=\"1\" max=\"3\" value=\"1\" class=\"form-control\" /></div><button type=\"button\" class=\"btn btn-primary\" onclick=\"jogar()\">Jogar dados</button><div id=\"result\"></div></div></div></div></div></div></div><script src=\"https://araujozb.github.io/wardice/index.js\"></script></body></html>\r\n";
static const char json[] PROGMEM = "true";

void enviarPagina() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(html);
}

void limparLedsRGB() {
  for (int i = 0; i < 256; i++) {
    leds[i] = CHSV((byte)i, 255, 255);
  }
  noInterrupts();
  FastLED.show();
  noInterrupts();
  delay(100);
  interrupts();
}

void limparLeds(CRGB cor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = cor;
  }
}

void enviarCor(CRGB cor) {
  for (int i = 0; i < 5; i++) {
    limparLeds(cor);
    noInterrupts();
    FastLED.show();
    noInterrupts();
    delay(200);
    limparLeds(CRGB::Black);
    noInterrupts();
    FastLED.show();
    noInterrupts();
    delay(200);
    noInterrupts();
  }

  limparLedsRGB();

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(4);
  server.send(200, "application/json", "");
  server.sendContent(json);
}

void enviarAmarelo() {
  enviarCor(amarelo);
}

void enviarAzul() {
  enviarCor(azul);
}

void enviarLaranja() {
  enviarCor(laranja);
}

void enviarRoxo() {
  enviarCor(roxo);
}

void enviarVerde() {
  enviarCor(verde);
}

void enviarVermelho() {
  enviarCor(vermelho);
}

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20); // Vai de 0 a 255

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", enviarPagina);
  server.on("/amarelo", enviarAmarelo);
  server.on("/azul", enviarAzul);
  server.on("/laranja", enviarLaranja);
  server.on("/roxo", enviarRoxo);
  server.on("/verde", enviarVerde);
  server.on("/vermelho", enviarVermelho);
  server.onNotFound(enviarPagina);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}