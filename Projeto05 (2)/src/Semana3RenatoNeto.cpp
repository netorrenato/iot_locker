#include <SPI.h>
#include <MFRC522.h> 
#include <GxEPD2_BW.h> 
#include <U8g2_for_Adafruit_GFX.h>
#include <GFButton.h>
#include <WiFi.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include "certificados.h"
#include <MQTT.h>
#include <ArduinoJson.h> 


#define BUZZER_PIN  2
#define RELAY_PIN   1

const bool RELAY_ACTIVE_LOW = true;

JsonDocument lista;

WiFiClientSecure conexaoSegura;
MQTTClient mqtt(1000);

U8G2_FOR_ADAFRUIT_GFX fontes; 
GxEPD2_290_T94_V2 modeloTela(10, 14, 15, 16); 
GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> tela(modeloTela);

GFButton botao(5);

GFButton sensor(21);

MFRC522 rfid(46, 17);


MFRC522::MIFARE_Key chaveA = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

String id_base = "";
String id_lido = "";
String id_usado = "";
String id_usuario = "";
String nome_usuario = "";

int lockerAtual = 1;

bool alarmeLigado = false;
int contaAlarm = 0;

unsigned long instanteAnterior = 0; 

bool lockerTrancado = 0;
bool idErrado = false;
bool idnreconhecido = false;
bool portaAberta = false;

String lerRFID() {
 String id = "";
 for (byte i = 0; i < rfid.uid.size; i++) {
 if (i > 0) {
 id += " ";
 }
 if (rfid.uid.uidByte[i] < 0x10) {
 id += "0";
 }
 id += String(rfid.uid.uidByte[i], HEX);
 }
 id.toUpperCase();
 return id;
}

void reconectarWiFi() {
 if (WiFi.status() != WL_CONNECTED) {
 WiFi.begin("Projeto", "2022-11-07");
 Serial.print("Conectando ao WiFi...");
 while (WiFi.status() != WL_CONNECTED) {
 Serial.print(".");
 delay(1000);
 }
 Serial.print("conectado!\nEndereço IP: ");
 Serial.println(WiFi.localIP());
 }
}

void reconectarMQTT() {
 if (!mqtt.connected()) {
 Serial.print("Conectando MQTT...");
 while(!mqtt.connected()) {
 mqtt.connect("018", "aula", "zowmad-tavQez");
 Serial.print(".");
 delay(1000);
 }
 Serial.println(" conectado!");

 mqtt.subscribe("IDlocker01"); // qos = 0
 }
}


void telaInicial(){
  tela.fillScreen(GxEPD_WHITE); 
  Serial.println("Porta abrindo");
  fontes.setFont( u8g2_font_helvB24_te );
  fontes.setFontMode(1);
  fontes.setCursor(50, 65);
  fontes.print("Locker livre");

  fontes.setFont( u8g2_font_helvB10_te );
  fontes.setFontMode(1);
  fontes.setCursor(20, 90);
  fontes.print("Passe o RFID para trancar o locker 01");
  tela.display(true); 
}

void telaTranca(){
  Serial.println("Porta trancando");
  struct tm tempo;
  getLocalTime(&tempo);
  int hora = tempo.tm_hour;
  int minuto = tempo.tm_min;
  String str_minuto = String(minuto);
  if (minuto < 10){
    str_minuto = 0 + str_minuto;
  }
  String tempofecha = "Locker fechado desde " + String(hora) + ":" + str_minuto;
  String stringnome = "ocupado por " + nome_usuario;
  
  tela.fillScreen(GxEPD_WHITE); 
  fontes.setFont( u8g2_font_helvB24_te );
  fontes.setFontMode(1);
  fontes.setCursor(20, 55);
  fontes.print("Locker ocupado"); 

  fontes.setFont( u8g2_font_helvB10_te );
  fontes.setFontMode(1);
  fontes.setCursor(50, 80);
  fontes.print(tempofecha);
  
  fontes.setFont( u8g2_font_helvB10_te );
  fontes.setFontMode(1);
  fontes.setCursor(50, 100);
  fontes.print(stringnome);
  tela.display(true);
}

void tranca(){
  Serial.println("Trancando locker...");
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH);
  lockerTrancado = 1;
  rgbLedWrite(RGB_BUILTIN, 100, 0, 0);
  telaTranca();
  JsonDocument dados2; 
  dados2["id_usuario"] = id_usuario; 
  dados2["locker"] = String(lockerAtual);
  dados2["status"] = false; 
  String textoJson2; 
  serializeJson(dados2, textoJson2); 
  mqtt.publish("locker01tranca", textoJson2);
}

void destranca(){
  Serial.println("Destrancando locker...");  
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);  
  lockerTrancado = 0;
  rgbLedWrite(RGB_BUILTIN, 0, 100, 0);
  telaInicial();
  JsonDocument dados2; 
  dados2["id_usuario"] = id_usuario; 
  dados2["locker"] = String(lockerAtual);
  dados2["status"] = true; 
  String textoJson2; 
  serializeJson(dados2, textoJson2); 
  mqtt.publish("locker01tranca", textoJson2);
  
}

void botaoPressionado (GFButton& botaoDoEvento) { 
  Serial.println("Botão foi pressionado!"); 
}


void recebeuMensagem(String topico, String conteudo) {
 Serial.println(topico + ": " + conteudo);

 if (topico == "IDlocker01"){
  Serial.println("teste do json");
  deserializeJson(lista, conteudo);
  String elemento = lista[0]["rfid"];
    Serial.println(elemento);
    String elemento2 = lista[0]["user_id"];
    String elemento3 = lista[0]["name"];
    if(id_lido == elemento){
      id_base = elemento;
      id_usuario = elemento2;
      nome_usuario = elemento3;
      Serial.println("ID DO USUARIO: " + elemento2);
      Serial.println("NOME DO USUARIO: " + elemento3);
    }
    if(botao.isPressed()){
      Serial.println("Porta está encostada");
      if (id_lido == id_base){
        tranca();
        id_usado = id_lido;
        id_lido = "";
      }
      else{
        instanteAnterior = millis();
        idnreconhecido = true;
        tela.fillScreen(GxEPD_WHITE); 
        fontes.setFont( u8g2_font_helvB18_te );
        fontes.setFontMode(1);
        fontes.setCursor(35, 60);
        fontes.print("ID não reconhecido!");

        fontes.setFont( u8g2_font_helvR12_te );
        fontes.setFontMode(1);
        fontes.setCursor(75, 85);
        fontes.print("Tente novamente!");
        tela.display(true); 
        
      }
    }
    else{
      Serial.println("A porta está Aberta");
      instanteAnterior = millis();
      portaAberta = true;
      tela.fillScreen(GxEPD_WHITE); 
      fontes.setFont( u8g2_font_helvB18_te );
      fontes.setFontMode(1);
      fontes.setCursor(70, 60);
      fontes.print("Porta Aberta");

      fontes.setFont( u8g2_font_helvR12_te );
      fontes.setFontMode(1);
      fontes.setCursor(35, 85);
      fontes.print("feche a porta para usar o locker");

      tela.display(true); 
    }  
 }

}

void movimento (GFButton& sensor) { 
  Serial.println("Movimento detectado!"); 
  alarmeLigado = false;
  contaAlarm = 0;
} 

void inercia (GFButton& sensor) { 
  Serial.println("Inércia detectada!"); 
} 

void setup() {
  Serial.begin(115200); delay(500);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  SPI.begin();
  rfid.PCD_Init();
  tela.init(); 
  tela.setRotation(3); 
  tela.fillScreen(GxEPD_WHITE); 
  fontes.setFont( u8g2_font_helvB24_te );
  tela.display(true); 
   
  fontes.begin(tela); 
  fontes.setForegroundColor(GxEPD_BLACK); 
  telaInicial();
  rgbLedWrite(RGB_BUILTIN, 0, 100, 0);

  botao.setPressHandler(botaoPressionado);
  reconectarWiFi(); 
  configTzTime("<-03>3", "a.ntp.br", "pool.ntp.org"); 

  conexaoSegura.setCACert(certificado1);
  mqtt.begin("mqtt.janks.dev.br", 8883, conexaoSegura);
  mqtt.onMessage(recebeuMensagem);
  mqtt.setKeepAlive(10);
  mqtt.setWill("tópico da desconexão", "conteúdo");
  reconectarMQTT(); 
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW); 
  sensor.setReleaseHandler(movimento); 
  sensor.setPressHandler(inercia);
  }



void loop() {
  sensor.process();
  //Serial.println(sensor.isPressed());
  botao.process(); 
  reconectarWiFi();
  reconectarMQTT();
  mqtt.loop(); 
  if (!botao.isPressed() && !lockerTrancado && !alarmeLigado){
    alarmeLigado = true;
    instanteAnterior = millis();
    Serial.println("Teste alarme tirando mov");
  }
 
  if(botao.isPressed() || lockerTrancado){
    alarmeLigado = false;
    //noTone(BUZZER_PIN);
    contaAlarm = 0;
  }

  unsigned long instanteAtual = millis(); 
  if (instanteAtual > instanteAnterior + 5000) { 
    Serial.println("+5 segundos"); 
    instanteAnterior = instanteAtual; 
    if(idErrado){
      telaTranca();
      idErrado = false;
    }
    if(idnreconhecido){
      telaInicial();
      idnreconhecido = false;
    }
    if(portaAberta){
      telaInicial();
      portaAberta = false;
    }
    if (alarmeLigado){
        contaAlarm++;
        if (contaAlarm == 5){
            Serial.println("porta a 25 seg aberta");
            tone(BUZZER_PIN, 440);
            contaAlarm = 0;
        }
    }
  }

 if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
  String id = lerRFID();
  Serial.println("UID: " + id);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  id_lido = id;
  
  if(lockerTrancado){
    if (id == id_usado){
      destranca();
    }
    else{
      instanteAnterior = millis();
      idErrado = true;
      tela.fillScreen(GxEPD_WHITE); 
      fontes.setFont( u8g2_font_helvB18_te );
      fontes.setFontMode(1);
      fontes.setCursor(70, 60);
      fontes.print("ID incorreto!");

      fontes.setFont( u8g2_font_helvR12_te );
      fontes.setFontMode(1);
      fontes.setCursor(75, 85);
      fontes.print("Tente novamente!");
      tela.display(true); 
      
      
    }
  }
  else{
    mqtt.publish("lockerP01", id_lido);
    
  }
 }
}