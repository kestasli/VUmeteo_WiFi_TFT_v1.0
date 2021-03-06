

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

int configPin = 4;
int station = 0;
//int eepromSize = 256;

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
    tft.print("Connect to WiFi iTermometras to configure");
    char station_id[5] = "0";
    //wifiManager.resetSettings();
    WiFiManagerParameter custom_station_id("stationid", "Station ID", station_id, 5);
    WiFiManager wifiManager;
    wifiManager.addParameter(&custom_station_id);
    wifiManager.startConfigPortal("iTermometras");
    eepromWrite(atoi(custom_station_id.getValue()));
  }

  //Read station ID from EEPROM
  station = eepromRead();

  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }

  tft.println();

  WiFi.printDiag(tft);

  tft.println();
  //Serial.println("EEPROM read:");
  //Serial.println(eepromRead1());
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

    //int station = eepromRead1();
    //int station = 1187;
    //int station = 0;

    if (station == 0){
      dataset.read(SOURCE_VU, 0);
    } else {
      dataset.read(SOURCE_KD, station);
    }

    temperatureDsp.set(dataset.temperature, ILI9341_WHITE);
    directionDsp.set(dataset.windspeed, dataset.winddirection);
  }
  delay (10);
}


void eepromWrite(int station_id){
  //This function will not write values larger than 65535!!!
  EEPROM.begin(2);
  //erase EEPROM
  //EEPROM.write(0, 0);
  //EEPROM.write(1, 0);
  //write new values
  EEPROM.write(0,highByte(station_id));
  EEPROM.write(1,lowByte(station_id));
  EEPROM.commit();
}

int eepromRead(){
  EEPROM.begin(2);
  byte high = EEPROM.read(0);
  byte low = EEPROM.read(1);
  return word(high,low);
}
