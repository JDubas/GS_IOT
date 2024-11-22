
# Projeto IoT: Controle Inteligente de Janelas para Economia de Energia

Este projeto utiliza a placa **ESP32** e o serviço **Thinger.io** para criar um sistema inteligente de controle de janelas. O objetivo é otimizar o consumo de energia, reduzindo a dependência de ar-condicionado ao regular a temperatura interna de forma eficiente.


### Video explicando o projeto:
https://youtu.be/FPqf9qtWW0A
---

## 🎯 **Objetivo**
Criar um sistema IoT que:
- Permita ao usuário definir uma **temperatura ideal** através do portal do **Thinger.io**.
- Controle automaticamente as janelas para ajustar a temperatura interna.
- Monitore as condições climáticas (como chuva) para garantir segurança e eficiência.
- Reduza o consumo de energia elétrica ao priorizar métodos passivos de ventilação antes de acionar o ar-condicionado.

---

## 🛠️ **Como Funciona**

1. **Configuração Inicial**  
   O usuário acessa o portal **Thinger.io** e define uma **temperatura ideal** para o ambiente.

2. **Monitoramento de Condições Climáticas**  
   A **ESP32** monitora:  
   - A **temperatura atual** do ambiente interno, usando um sensor de temperatura e umidade com o **DHT22**.  
   - Se está **chovendo**, utilizando um sensor de chuva ou outro método de detecção.

3. **Controle Inteligente das Janelas**  
   - Se a temperatura atual está acima da temperatura ideal, o sistema tenta reduzir a temperatura interna **abrindo as janelas**.  
   - Caso esteja chovendo, as janelas permanecem **fechadas** para evitar danos.  
   - O sistema verifica continuamente se a ventilação natural está suficiente para alcançar a temperatura desejada.

4. **Ação Complementar**  
   - Se a ventilação natural não for suficiente, o sistema sinaliza que o ar-condicionado pode ser ligado, mantendo o uso mínimo e eficiente.

5. **Resultados e Economia de Energia**  
   O sistema prioriza o uso de ventilação natural, reduzindo o consumo de eletricidade e contribuindo para um ambiente mais sustentável.

---


## Imagens

- Diagrama do projeto desligado
  
![image](https://github.com/user-attachments/assets/ebc90814-e3fe-4d4c-9a15-31acd30eaf02)

- Leds significam um comando para o ar-condicionado, Verde é ligado, Vermelho é desligar

![image](https://github.com/user-attachments/assets/3dbab6d0-34de-47b1-98ff-cf54829ad552)

- Servo de controle de janela

 ![image](https://github.com/user-attachments/assets/83037e49-255b-4a38-8719-1e97f82f0868)

- DashBoard no Thinger.io
  
![image](https://github.com/user-attachments/assets/37e8a567-3a9f-40b0-ad74-5c6f9a7c48d7)



- Exemplo Janela fechada, Ar ligado

![image](https://github.com/user-attachments/assets/f33b039a-5d3b-4e1d-8211-e52f9fc33052)

- Exemplo Janela Aberta, Ar desligado

![image](https://github.com/user-attachments/assets/3a3215e3-6d5e-470d-80e0-fbfaca97cf3f)


- Conexão fechada com o Thinger.io

![image](https://github.com/user-attachments/assets/bd917b49-cedf-4d6c-b3de-50d7f1ce7e9a)


- Itens enviados do Thinger.io para ESP32

  ![image](https://github.com/user-attachments/assets/7ade6986-a6db-4085-9ae9-325721fbb0d5)

- Itens enviados do ESP32 para Thinger.io

  ![image](https://github.com/user-attachments/assets/fa9c4389-2140-4430-a815-e3a62f981f0b)




## Código
<details><summary>Sketch.ino</summary>
   
      #include <ESP32Servo.h>
      #include <ThingerESP32.h>
      #include "DHTesp.h"
      #define USERNAME "Dubas"
      #define DEVICE_ID "GlobalSolutionHall9000"
      #define DEVICE_CREDENTIAL "Hall9000"
      
      #define SSID "Wokwi-GUEST"
      #define SSID_PASSWORD ""
      
      bool energiaOn = false;
      int sliderValue = 0;
      const int servoPin = 18;
      
      const int DHT_PIN = 15;
      
      float temp = 0;
      int hum = 0;
      
      const int Led_verde = 14;
      const int Led_vermelho = 27;
      
      Servo servo;
      
      DHTesp dhtSensor;
      ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
      
      
      unsigned long energiaStartTime = 0;
      bool initialDelayActive = false;
      
      void setup() {
        pinMode(Led_verde, OUTPUT);
        pinMode(Led_vermelho, OUTPUT);
        thing.add_wifi(SSID, SSID_PASSWORD);
        Serial.begin(115200);
   
     thing["temp"] >> outputValue(temp);
     thing["hum"] >> outputValue(hum);
   
     dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
   
     thing["Energia"] << [](pson& in) {
       if (in.is_empty()) {
         in = energiaOn; 
       } else {
         energiaOn = in;
         if (energiaOn) {
           energiaStartTime = millis();
           initialDelayActive = true;
           servo.write(180);
           Serial.println("Energia ON: Servo aberto por 30 segundos.");
           digitalWrite(Led_verde, LOW);
           digitalWrite(Led_vermelho, HIGH);
         }
       }
     };
   
     thing["Slider"] << inputValue(sliderValue, {
       Serial.print("Valor do slider recebido: ");
       Serial.println(sliderValue);
     });
   
     servo.attach(servoPin, 500, 2400);
     servo.write(0);
     Serial.println("Servo inicializado em posição fechada.");
      }
   
      void loop() {
   
     TempAndHumidity data = dhtSensor.getTempAndHumidity();
     hum = data.humidity;
     temp = data.temperature;
   
     Serial.print("Umidade: ");
     Serial.println(hum);
     Serial.print("Temperatura: ");
     Serial.println(temp);

    if (energiaOn) {
    if (initialDelayActive) {
      if (millis() - energiaStartTime >= 30000) {
        initialDelayActive = false;
        Serial.println("30 segundos passaram. Verificando condições...");
      } else {
        return;
      }
    }

    if (temp >= sliderValue - 2 && temp <= sliderValue + 2 && hum < 50) {
      servo.write(180);
      Serial.println("Condições atendidas: Servo permanece aberto (180°).");
      digitalWrite(Led_verde, LOW);
      digitalWrite(Led_vermelho, HIGH);
    } else {
      servo.write(0);
      Serial.println("Condições não atendidas: Servo movido para posição fechada (0°).");
      digitalWrite(Led_vermelho, LOW);
      digitalWrite(Led_verde, HIGH);
    }
     } else {
       servo.write(0);
       Serial.println("Energia OFF: Servo movido para posição fechada (0°).");
       digitalWrite(Led_vermelho, LOW);
       digitalWrite(Led_verde, HIGH);
     }
   
     thing.handle();
     delay(5000);
   
    }

</details>

<details><summary>Diagram.json</summary>
   
      {
        "version": 1,
        "author": "joão Dubas",
        "editor": "wokwi",
        "parts": [
          { "type": "wokwi-breadboard-mini", "id": "bb1", "top": 142.6, "left": 122.4, "attrs": {} },
          {
            "type": "board-esp32-devkit-c-v4",
            "id": "esp",
            "top": -96,
            "left": -177.56,
            "rotate": 90,
            "attrs": {}
          },
          { "type": "wokwi-servo", "id": "servo1", "top": -69.2, "left": 163.2, "attrs": {} },
          {
            "type": "wokwi-dht22",
            "id": "dht1",
            "top": 38.7,
            "left": 177,
            "attrs": { "temperature": "26.2", "humidity": "42.5" }
          },
          {
            "type": "wokwi-resistor",
            "id": "r1",
            "top": 206.4,
            "left": 172.25,
            "rotate": 90,
            "attrs": { "value": "1000" }
          },
          {
            "type": "wokwi-led",
            "id": "led1",
            "top": -186,
            "left": -188.2,
            "attrs": { "color": "limegreen" }
          },
          {
            "type": "wokwi-led",
            "id": "led2",
            "top": -186,
            "left": -130.6,
            "attrs": { "color": "red" }
          }
        ],
        "connections": [
          [ "esp:TX", "$serialMonitor:RX", "", [] ],
          [ "esp:RX", "$serialMonitor:TX", "", [] ],
          [ "servo1:V+", "esp:5V", "green", [ "h-38.4", "v-67.1", "h-345.6" ] ],
          [ "servo1:GND", "esp:GND.2", "black", [ "h-115.2", "v67.2" ] ],
          [ "servo1:PWM", "esp:18", "green", [ "h-76.8", "v86.6", "h-211.2" ] ],
          [ "esp:CLK", "bb1:8b.j", "green", [ "v286.34", "h46.86" ] ],
          [ "esp:GND.2", "bb1:10t.d", "black", [ "v238.34", "h267.66" ] ],
          [ "esp:CLK", "bb1:7t.e", "green", [ "v0" ] ],
          [ "esp:15", "bb1:8t.c", "green", [ "v0" ] ],
          [ "dht1:VCC", "bb1:7t.a", "", [ "$bb" ] ],
          [ "dht1:SDA", "bb1:8t.a", "", [ "$bb" ] ],
          [ "dht1:NC", "bb1:9t.a", "", [ "$bb" ] ],
          [ "dht1:GND", "bb1:10t.a", "", [ "$bb" ] ],
          [ "r1:1", "bb1:8t.d", "", [ "$bb" ] ],
          [ "r1:2", "bb1:8b.h", "", [ "$bb" ] ],
          [ "led1:C", "esp:GND.1", "green", [ "v0" ] ],
          [ "led1:A", "esp:14", "green", [ "v48", "h9.6" ] ],
          [ "led2:C", "esp:GND.1", "green", [ "v57.6", "h-57.2" ] ],
          [ "led2:A", "esp:27", "green", [ "v76.8", "h-38.4" ] ]
        ],
        "dependencies": {}
      }
</details>
