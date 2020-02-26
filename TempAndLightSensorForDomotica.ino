#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
#include <BH1750.h>
BH1750 lightMeter;
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Pins:
const int sensorPin = A0;
const int lightPin = A1;

//RGB LED:
const int redPin = 10;
const int greenPin = 11;
const int bluePin = 12;

//LED'S:
const int greenLed = 2;
const int yellowLed = 3;

//Variables:
float sensorValue;
float voltageOut;
float temperatureC;
float temperatureF;
float temperatureK;
float bmpTemp;
float bmpPress;
float lightValue;
float lux;
float avgTemp;
bool blinds;
bool heating;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(lightPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);

  //BMP280 Sensor:
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    setColor(255, 0, 0); // RED
    while (1) delay(10);
  }

  //SSD1306 OLED:
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    setColor(255, 0, 0); // RED
    for (;;);
  }

  //BH1750 Sensor:
  lightMeter.begin();
}

void loop() {
  delay(3000);
  //LM335:
  sensorValue = analogRead(sensorPin);
  voltageOut = (sensorValue * 5000) / 1024;
  temperatureK = voltageOut / 10;
  temperatureC = temperatureK - 273;
  temperatureF = (temperatureC * 1.8) + 32;

  //BMP280:
  bmpTemp = bmp.readTemperature();
  bmpPress = bmp.readPressure() / 100;

  //BH1750:
  lux = lightMeter.readLightLevel();

  //Light:
  lightValue = analogRead(lightPin);

  //Average temperature:
  avgTemp = (temperatureC + bmpTemp) / 2;

  //Serial monitor print:
  Serial.println("--------------------------------");
  Serial.println("LM335 Sensor:");
  Serial.print("Temperature(ºC): ");
  Serial.println(temperatureC);
  Serial.print("Voltage(mV): ");
  Serial.println(voltageOut);
  Serial.println();
  Serial.println("BMP280 Sensor:");
  Serial.print("Temperature = ");
  Serial.print(bmpTemp);
  Serial.println(" °C");
  Serial.print("Pressure = ");
  Serial.print(bmpPress);
  Serial.println(" hPa");
  Serial.println();
  Serial.println("BH1750 Sensor:");
  Serial.print("Light = ");
  Serial.print(lux);
  Serial.println(" lux");
  Serial.println();
  Serial.println("LDR Sensor:");
  Serial.print("Lightintensity = ");
  Serial.println(lightValue);
  Serial.println();
  Serial.print("Average temperature = ");
  Serial.print(avgTemp);
  Serial.println(" °C");

  //OLED print:
  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("LM335:");
  display.setCursor(0, 8);
  display.setTextColor(BLACK, WHITE);
  display.print((char)247); // ' ° ' symbol
  display.print("C: ");
  display.print(temperatureC, 1);

  display.setCursor(0, 16);
  display.setTextColor(WHITE);
  display.println("BMP280:");
  display.setCursor(0, 24);
  display.setTextColor(BLACK, WHITE);
  display.print((char)247); // ' ° ' symbol
  display.print("C: ");
  display.print(bmpTemp, 1);

  display.setCursor(0, 32);
  display.setTextColor(WHITE);
  display.println("BH1750:");
  display.setCursor(0, 40);
  display.setTextColor(BLACK, WHITE);
  display.print("Light = ");
  display.print(lux);
  display.print(" lux");

  display.setCursor(0, 48);
  display.setTextColor(WHITE);
  display.print("LDR = ");
  display.println(lightValue);
  display.print("Blinds = ");
  display.println(blinds);
  display.print("Heating = ");
  display.print(heating);
  display.display();

  //Blinds:
  if (lightValue > 90) {
    blinds = false; // blinds down
    digitalWrite(greenLed, HIGH);
    Serial.println("Blinds down");
  } else {
    blinds = true; // blinds up
    digitalWrite(greenLed, LOW);
    Serial.println("Blinds up");
  }

  //Heater:
  if ((avgTemp - 0.5) < 21) {
    heating = true; // heating on
    digitalWrite(yellowLed, HIGH);
    Serial.println("Heating on");
  }
  if ((avgTemp + 0.5) > 21) {
    heating = false; // heating off
    digitalWrite(yellowLed, LOW);
    Serial.println("Heating off");
  }
}

//Function to set the color for the RGB LED:
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
