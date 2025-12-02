#include <Arduino.h>
#include <WiFi.h>
#include <GFButton.h>
#include <SPI.h>
#include <MFRC522.h>

#define RELAY_PIN   1
#define BUTTON_PIN  5
#define BUZZER_PIN  2

#define RFID_SS_PIN  46
#define RFID_RST_PIN 17

const bool RELAY_ACTIVE_LOW = true;

GFButton botao(BUTTON_PIN);
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

MFRC522::MIFARE_Key chaveA = {
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

bool portaTrancada = false;
bool alarmeAtivo   = false;

unsigned long instanteAbertura = 0;
const unsigned long TEMPO_MAX_ABERTA = 10000;

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

String lerTextoDoBloco(byte bloco) { 
  byte tamanhoDados = 18; 
  char dados[tamanhoDados]; 
  MFRC522::StatusCode status = rfid.PCD_Authenticate( 
    MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
    bloco, &chaveA, &(rfid.uid) 
  ); 
  if (status != MFRC522::STATUS_OK) { return ""; } 
  status = rfid.MIFARE_Read(bloco, 
              (byte*)dados, &tamanhoDados); 
  if (status != MFRC522::STATUS_OK) { return ""; } 
  dados[tamanhoDados - 2] = '\0'; 
  return String(dados); 
}

void destrancarLocker() {
  Serial.println("Destrancando locker...");
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
  portaTrancada = false;
  instanteAbertura = millis();
}

void trancarLocker() {
  Serial.println("Trancando locker...");
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH);
  portaTrancada = true;
  alarmeAtivo = false;
  // noTone(BUZZER_PIN);
}

void alternarLocker() {
  if (portaTrancada) {
    Serial.println("Locker estava trancado, vou destrancar.");
    destrancarLocker();
  } else {
    Serial.println("Locker estava destrancado, vou trancar.");
    trancarLocker();
  }
}

void botaoPressionado(GFButton& botaoDoEvento) {
  Serial.println("Botão pressionado! Alternando estado do locker.");
  alternarLocker();
}

void comandoSerial() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n'); 

    msg.trim(); 

    if (msg == "A") {
      Serial.println("ABRIR, Destrancando o locker");
      digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);  
    } 
    else if (msg == "F") {
      Serial.println("FECHAR, Trancando o locker");
      digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH);  
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  destrancarLocker();

  botao.setPressHandler(botaoPressionado);

  SPI.begin();
  rfid.PCD_Init();

  Serial.println("Sistema iniciado. Locker começa aberto.");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String id = lerRFID();
    Serial.println("UID: " + id);
    String texto = lerTextoDoBloco(6); 
    Serial.println("Bloco 6: " + texto);

    if (id == "F1 01 34 02") {
      Serial.println("RFID reconhecido! Alternando estado do locker.");
      alternarLocker();
    } else {
      Serial.println("RFID não reconhecido!");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  comandoSerial();
  
  unsigned long agora = millis();

  if (!portaTrancada) {
    if (!alarmeAtivo && (agora - instanteAbertura >= TEMPO_MAX_ABERTA)) {
      Serial.println("Locker ficou mais de 10s aberto. Ativando alarme!");
      alarmeAtivo = true;
    }
  } else {
    alarmeAtivo = false;
  }
  /*
  if (alarmeAtivo) {
     tone(BUZZER_PIN, 440);
  } else {
    noTone(BUZZER_PIN);
  }*/

  botao.process();
}