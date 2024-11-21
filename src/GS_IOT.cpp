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
        servo.write(180); // Abre o servo imediatamente
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
