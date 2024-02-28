#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

String URL = "http://192.168.132.158/rfid/test.php";

const char* ssid = "LOGIN WIR"; 
const char* password = "12345678"; 

#define RST_PIN   22
#define SS_PIN    5 
const int pinLed_1 =  12;
const int pinLed_2 = 14;

String Data, id, CardID = "", status = "", akses;
int idcard;

MFRC522 mfrc522(SS_PIN, RST_PIN);
WiFiServer server (80);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  connectWiFi();
  SPI.begin();
  mfrc522.PCD_Init();
  server.begin();
  delay(4);
  pinMode(pinLed_1, OUTPUT);
  pinMode(pinLed_2, OUTPUT);

  Serial.println("Tempel kartu anda");
  Serial.println();
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  client = server.available();
  WebServer();
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<meta http-equiv='refresh' content='1'>");
  client.println("<style>html { font-family: Helvetica; color: black; display: inline-block; margin: 0px auto; text-align: center; }</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<body><h1>SCAN KARTU ANDA</h1>");
  client.println("");
  client.println("<p>Card ID : </p>");
  //client.println("<body>");
  
  // Web Page Heading
  

  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  if(!mfrc522.PICC_ReadCardSerial()){
    return;
  }
  Serial.print("UID tag : ");
  id = String(mfrc522.uid.uidByte[0]) + String(mfrc522.uid.uidByte[1]) + String(mfrc522.uid.uidByte[2]) + String(mfrc522.uid.uidByte[3]);
  Serial.println(id);
  
  if (id == "414140242")
  { 
    akses = "Succes";
    idcard = id.toInt(); // Mengubah string ke integer
    Data = "idcard="+String(idcard)+"&akses="+akses;
    Serial.println("Hello World!");
    status = "Success";
    digitalWrite(pinLed_2, HIGH);
    digitalWrite(pinLed_1, LOW);
  } else{
    akses = "Denied";
    idcard = id.toInt(); // Mengubah string ke integer
    Data = "idcard="+String(idcard)+"&akses="+akses;
    Serial.println("Denied");
    status = "Denied";
    digitalWrite(pinLed_1, HIGH);
    digitalWrite(pinLed_2, LOW);
  }
  CardID = id;
  client.println("<p>"+ CardID +"</p>");
  client.println("<p>"+ status +"</p>");
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(Data);

  Serial.print("URL : "); Serial.println(URL); 
  Serial.print("Data: "); Serial.println(Data);
  Serial.print("httpCode: "); Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.print("Payload: ");
    Serial.println(payload);
  } else {
    Serial.println("Error in HTTP request");
  }
 
  Serial.println("--------------------------------------------------");

}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("connected to : "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}