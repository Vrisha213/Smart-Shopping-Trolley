#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SPIFFS.h>

constexpr uint8_t RST_PIN = 22;
constexpr uint8_t SS_PIN = 21;

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

AsyncWebServer server(80);

const char *ssid = "hotspot2G";
const char *password = "knockout1";

String tag;
String page = "";

int p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5=0;
int c1 = 0, c2 = 0, c3 = 0, c4 = 0,c5=0;
double total = 0;
int count_prod = 0;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("Wifi Connected");
  Serial.println(WiFi.localIP());
  delay(1500);

  Serial.println("Welcome to VON Mart");
  Serial.println("Happy Shopping");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    page = "<html><head><title>ECart</title>";
    page += "<style type=\"text/css\">";
    page += "body { background-color: #f0f0f0; font-family: Arial, sans-serif; }";
    page += "h1 { color: #3498db; }";
    page += "table { width: 1200px; height: 450px; border-collapse: collapse; margin-top: 20px; background-color: #ffffff; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    page += "th { background-color: #3498db; color: white; padding: 15px; }";
    page += "td { border: 4px solid black; font-size: large; text-align: center; border-style: groove; border-color: rgb(255, 0, 0); padding: 10px; }";
    page += "input[type=\"button\"] { background-color: #3498db; color: white; width: 200px; height: 50px; font-size: large; border: none; cursor: pointer; }";
    page += "input[type=\"button\"]:hover { background-color: #2980b9; }";
    page += "</style></head>";
    page += "<body><center><h1>Welcome to VON Mart</h1><br><br>";
    page += "<table><tr><th>ITEMS</th><th>QUANTITY</th><th>COST</th></tr>";
    page += "<tr><td>NesCafe</td><td>" + String(p1) + "</td><td>" + String(c1) + "</td></tr>";
    page += "<tr><td>OREO</td><td>" + String(p2) + "</td><td>" + String(c2) + "</td></tr>";
    page += "<tr><td>Sugar(1KG)</td><td>" + String(p3) + "</td><td>" + String(c3) + "</td></tr>";
    page += "<tr><td>Sprite</td><td>" + String(p4) + "</td><td>" + String(c4) + "</td></tr>";
    page += "<tr><td>Chips</td><td>" + String(p5) + "</td><td>" + String(c5) + "</td></tr>";
    page += "<tr><th>Grand Total</th><th>" + String(count_prod) + "</th><th>" + String(total) + "</th></tr>";
    page += "</table><br><input type=\"button\" name=\"Pay Now\" value=\"Pay Now\"></center></body></html>";
    page += "<meta http-equiv=\"refresh\" content=\"2\">";
    request->send(200, "text/html", page);
  });

  server.begin();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 5; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);

    if (tag == "1966234140") {
      // Handle RFID tag for NesCafe
      handleProduct("NesCafe", 35.0, p1, c1);
    } else if (tag == "20813018286") {
      // Handle RFID tag for OREO
      handleProduct("OREO", 10.0, p2, c2);
    } else if (tag == "7316765152") {
      // Handle RFID tag for Sugar(1KG)
      handleProduct("Sugar(1KG)", 62.0, p3, c3);
    } else if (tag == "24923979152") {
      // Handle RFID tag for Sprite
      handleProduct("Sprite", 20.0, p4, c4);
    }
      else if (tag == "24923979152") {
      // Handle RFID tag for Chips
      handleProduct("Chips", 100.0, p5, c5);
    }

    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void handleProduct(String productName, double productPrice, int &quantity, int &cost) {
  Serial.println("Product name: " + productName);
  Serial.println("Product price: " + String(productPrice) + " Rs");
  quantity++;
  total += productPrice;
  count_prod++;
  cost = quantity * productPrice;
  Serial.println(productName + " added to the Cart");
}
