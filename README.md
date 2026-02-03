# Alimentador ESPaw-32
**Projeto de Sistemas Embarcados – IFPR 2025**

## Descrição Geral
O **Alimentador ESPaw-32** é um sistema automatizado de alimentação para pets, desenvolvido com **ESP32**, sensores e atuadores, permitindo controle local e remoto via Wi-Fi.  
O projeto tem como objetivo aplicar conceitos de **sistemas embarcados**, **IoT** e **automação residencial**.

## Objetivos

### Objetivo Geral
Desenvolver um sistema IoT com o ESP-32 para um Alimentador com um bot telegram para notificação de alertas aos usuários registrados. O projeto visa englobar os conhecimentos e conteúdos passados durante todo o percurso da matéria de Sistemas Embarcados.

### Objetivos Específicos
- Prototipar no Tinkercard com a base do Arduino Uno para simulação da balança, envolvendo potênciometro + Painel LED + LED RGB;
- Prototipar no Wokwi com a base do ESP-32 e integrar o protótipo anterior ao Universal Telegram Bot;
- Realizar o teste do protótipo final no Wokwi, obtendo a resposta dos níveis via bot telegram aos usuários cadastrados. 

## Arquitetura do Sistema

## Componentes Utilizados
- 1x ESP-32.
- 1x protoboard.
- 1x LED RGB.
- 3x resistores de 220 Ohms.
- 1x potênciometro (simulação de balança).
- 1x painel OLED Monocromático de 128x64.

## Como Executar o Projeto


### Pré-requisitos
- **Wokwi**  
- **Bibliotecas:**  
  - WiFi.h  
  - WiFiClientSecure.h  
  - UniversalTelegramBot.h  
  - ArduinoJson.h 
  - Adafruit_GFX.h
  - Adafruit_SSD1306.h

Projeto disponível publicamente no [Wokwi](https://wokwi.com/projects/449906774265785345)
  
## Autores
- [Gabriel Albuquerque](https://github.com/Gadsz)  
- [João Vitor Campõe Galescky](https://github.com/Joaogalescky)
