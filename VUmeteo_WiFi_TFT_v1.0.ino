

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MeteoData.h>
#include <WiFiManager.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <MeteoDashboard.h>
#include <EEPROM.h>

#include <Fonts/FreeSansBold56pt7b.h>

#define TFT_DC 2
#define TFT_CS 15
#define TFT_RST 16

//const char* ssid     = "TEO-130392";
//const char* password = "skMQyjleDz3qq2Uo2jKsHMJDzJ";

boolean configmode = false;
boolean debugmode = false;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 10000;

int configPin = 5;
//int eepromSize = 256;

char station_id[5] = "1166";

WiFiClient client;
MeteoData dataset;

//ESP8266_SSD1322 display(OLED_DC, OLED_RESET, OLED_CS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

NumberIndicator temperatureDsp = NumberIndicator();
DirectionIndicator directionDsp = DirectionIndicator();
//LevelIndicator windlevelDsp = LevelIndicator();
//LevelIndicator humlevelDsp = LevelIndicator();

void setup() {

  Serial.begin(115200);
  delay(10);

  eepromWrite(station_id);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  temperatureDsp = NumberIndicator(tft, 0, 120, &FreeSansBold56pt7b, ALIGN_TOPC);
  temperatureDsp.setFormat(1, "");
  directionDsp = DirectionIndicator(tft, 0, 168, 62, ILI9341_LIGHTBLUE);

  tft.setCursor(0, 10);
  tft.setFont();
  tft.setTextSize(2);

  pinMode(configPin, INPUT_PULLUP);

  if (digitalRead(configPin) == LOW) {
    tft.print("Connect to WiFi network iTermometras to configure");
    //wifiManager.resetSettings();
    WiFiManagerParameter custom_station_id("stationid", "Station ID", station_id, 5);
    WiFiManager wifiManager;
    wifiManager.addParameter(&custom_station_id);
    wifiManager.startConfigPortal("iTermometras");
  }

  //tft.print("Connecting to ");
  //tft.println(client.SSID());

  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }

  WiFi.printDiag(tft);
  WiFi.printDiag(Serial);

  tft.println("");
  tft.println("WiFi connected");
  tft.println("IP address: ");
  tft.println(WiFi.localIP());
  delay(7000);
  tft.fillScreen(ILI9341_BLACK);
}

void loop() {

  currentMillis = millis();

  if ((currentMillis - previousMillis > interval) | (previousMillis == 0)) {

    previousMillis = currentMillis;

    //dataset.read(SOURCE_KD, 1187);
    dataset.read(SOURCE_VU, 0);
    temperatureDsp.set(dataset.temperature, ILI9341_WHITE);
    directionDsp.set(dataset.windspeed, dataset.winddirection);
  }
  delay (10);
}

void eepromWrite(char* string){
  int eepromSize = 24;
  int size = sizeof(string);
  EEPROM.begin(eepromSize);
  for (int address = 0; address < size; address++){
    EEPROM.write(address, string[address]);
  }
  EEPROM.commit();
}

int eepromRead(){
  int eepromSize = 24;

  for(address = 0; address < eepromSize; address++){

  }
}
