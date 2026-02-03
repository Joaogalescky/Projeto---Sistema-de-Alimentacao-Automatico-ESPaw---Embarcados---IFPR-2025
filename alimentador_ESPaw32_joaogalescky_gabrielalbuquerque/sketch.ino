// Dupla: João Vitor Campõe Galescky && Gabriel Albuquerque
// Projeto: Alimentador - ESPaw-32

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Hardware
#define PIN_LED_R 13
#define PIN_LED_G 12
#define PIN_LED_B 14
#define PIN_POT 34
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Wifi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Telegram
#define BOT_TOKEN "8589789553:AAFnpMCjYAFA_RqqG4ZbZjBa9smpMsimQIo"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Chat Telegram
#define MAX_CHATS 5
String validChatIds[MAX_CHATS];
int numValidChats = 0;

// Variáveis globais
int globalOutputValue = 0;
int sensorValue = 0;
unsigned long lastTimeBotRan;

void taskTelegram(void* pvParameters);

bool isChatIdValid(String chatId)
{
  for (int i = 0; i < numValidChats; i++)
  {
    if (validChatIds[i] == chatId)
    {
      return true;
    }
  }
  return false;
}

bool addChatId(String chatId)
{
  if (numValidChats < MAX_CHATS && !isChatIdValid(chatId))
  {
    validChatIds[numValidChats++] = chatId;
    return true;
  }
  return false;
}

bool removeChatId(String chatId)
{
  for (int i = 0; i < numValidChats; i++)
  {
    if (validChatIds[i] == chatId)
    {
      // Mover proximos elementos para a posição
      for (int j = i; j < numValidChats - 1; j++)
      {
        validChatIds[j] = validChatIds[j + 1];
      }
      validChatIds[numValidChats - 1] = "";
      numValidChats--;
      return true;
    }
  }
  return false;
}

void sendNotification(String message)
{
  for (int i = 0; i < numValidChats; i++)
  {
    bot.sendMessage(validChatIds[i], message, "");
  }
}

void handleNewMessage(int numMessages) {
  for (int i=0; i<numMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    // Adicionar id
    if (text.equalsIgnoreCase("/addid")) {
      if (addChatId(chat_id)) {
        bot.sendMessage(chat_id, "SUCESSO: ID adicionado para notificacoes!", "");
      } else if (isChatIdValid(chat_id)) {
        bot.sendMessage(chat_id, "ATENCAO! ID ja esta na lista.", "");
      } else {
        bot.sendMessage(chat_id, "ERRO: Lista cheia.", "");
      }
    }

    // Remover ID
    else if (text.equalsIgnoreCase("/removeid")) {
      if (removeChatId(chat_id)) {
        bot.sendMessage(chat_id, "SUCESSO: ID removido da lista.", "");
      } else {
        bot.sendMessage(chat_id, "ATENCAO! ID nao esta na lista.", "");
      }
    }

    // Tutorial
    else if (text == "/start") {
      String welcome = "Bem-vindo ao FeederBot, " + from_name + ".\n\n";
      welcome += "Use os comandos:\n";
      welcome += "/addid - Receber notificações do status.\n";
      welcome += "/removeid - Parar de receber notificações.\n";
      welcome += "/info - Ver o status atual do alimentador.\n";
      welcome += "/options - Exibir teclado inline de exemplo.\n";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }

    // Informacao
    else if (text.equalsIgnoreCase("/info"))
    {
      String infoMsg = "--- Status do Feeder ---\n";
      infoMsg += "Nível Mapeado: " + String(globalOutputValue) + "\n";
      infoMsg += "IDs Registrados: " + String(numValidChats) + "/" + String(MAX_CHATS) + "\n";
      bot.sendMessage(chat_id, infoMsg, "");
    }
  }
}

long ultimoEnvio = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  digitalWrite(PIN_LED_R, LOW);
  digitalWrite(PIN_LED_G, LOW);
  digitalWrite(PIN_LED_B, LOW);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha no OLED"));
  }
  display.clearDisplay();

  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  client.setInsecure(); // Wokwi simplifica o SSL

  // CRIAÇÃO DA TASK NO CORE 0
  xTaskCreatePinnedToCore(
    taskTelegram, // Função da tarefa
    "BotTelegram", // Nome
    10000, // Tamanho da Stack
    NULL, // Parâmetros
    1, // Prioridade
    NULL, // Handle
    0 // Núcleo
  );
}

void loop() {
  // Logica do alimentador (core 1 por padrao)
  sensorValue = analogRead(PIN_POT);
  globalOutputValue = map(sensorValue, 0, 4095, 0, 2000); // ESP32 é 12 bits (4095)

  // Leds
  if (globalOutputValue > 1000) {
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH);
  } else if (globalOutputValue > 500) {
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
  } else {
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, LOW);
    if (millis() - ultimoEnvio > 5000) {
      Serial.println("Nivel baixo");
      sendNotification("ATENCAO! Nivel do alimentador baixo: " + String(globalOutputValue));
      ultimoEnvio = millis();
    }
  }

  // Display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Nivel Alimentador:");
  display.setCursor(0,20);
  display.setTextSize(2);
  display.print(globalOutputValue);
  display.display();
  delay(100); // Delay para estabilidade do ADC

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      for (int i=0; i<numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
      
        handleNewMessage(numNewMessages);
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
}

// FUNCAO DA TASK DO TELEGRAM (CORE 0)
void taskTelegram(void* pvParameters) {
  while(true) {
    // int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    // while(numNewMessages) {
    //   for (int i=0; i<numNewMessages; i++) {
    //     String chat_id = String(bot.messages[i].chat_id);
    //     String text = bot.messages[i].text;
      
    //     handleNewMessage(numNewMessages);
    //   }
    //   numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    // }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera de 1s para checar novamente
  }
}