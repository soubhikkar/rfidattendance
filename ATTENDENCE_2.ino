





/* -----------------------------------------------------------------------------
  - Project: RFID attendance system using NodeMCU

  SDA--------->D2
  SCK--------->D5
  MOSI--------->D7
  MISO--------->D6
  GND---------->G
  RST---------->D1
  3V----------->3V
  lcd---------->D3,D4
  
 
  the RFID attendance project with NodeMCU.
   ---------------------------------------------------------------------------*/
//*******************************libraries********************************
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
//RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


//************************************************************************
#define SS_PIN  D2  //D8
#define RST_PIN D1  //D3
#define LED D0    //led D
#define Buzzer D8 // Buzzer
//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2);
//************************************************************************
/* Set these to your desired credentials. */
const char *ssid = "Robotics Club";
const char *password = "AndroHumanoid";
const char* device_token  = "3ea0fed9633245fe";   //186b50c35dd76f2e
//************************************************************************
String URL = "http://192.168.31.156/rfidattendance/getdata.php"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;
//************************************************************************
void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  //---------------------------------------------
  connectToWiFi();
  Wire.begin(2,0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight 
  lcd.clear();
  pinMode(LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  lcd.print("Scan card");
}
//************************************************************************
void loop() {
  //lcd.clear();
  //lcd.print(" SCANE THE CARD  ");
  //check if there's a connection to Wi-Fi or not
  if(!WiFi.isConnected()){
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  //delay(50);
  //---------------------------------------------
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//got to start of loop if there is no card present
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //---------------------------------------------
//  Serial.println(CardID);
  SendCardID(CardID);
  delay(1000);
}
//************send the Card UID to the website*************
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
  
    Link = URL + getData;
    http.begin(client,Link); //initiate HTTP request 
    
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

//    Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
//      Serial.println(user_name);
        lcd.clear();
        lcd.print(" Welcome ");
        lcd.setCursor(0,1);
        lcd.print(user_name);
        digitalWrite (Buzzer, HIGH); //turn buzzer on
        delay(1000);
        digitalWrite (Buzzer, LOW);
        delay(1000);
        lcd.clear();

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
//      Serial.println(user_name);
        lcd.clear();
        String a = user_name;
//        Serial.print(a);
          lcd.print("LOGOUT");
        lcd.setCursor(0,1);
        lcd.print(user_name);
          digitalWrite (Buzzer, HIGH); //turn buzzer on
          delay(1000);
          digitalWrite (Buzzer, LOW);
          delay(1000);
          lcd.clear();        
      }
      else if (payload == "succesful") {
        

      }
      else if (payload == "available") {

      }
      else if (payload == "Not found!"){
        lcd.print("Not allowed!");
        digitalWrite (Buzzer, HIGH); //turn buzzer on
          delay(1000);
          digitalWrite (Buzzer, LOW);
          delay(1000);
          lcd.clear();
        }
      
      //delay(100);
      http.end();  //Close connection
    }
  }
}
//********************connect to the WiFi******************
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      digitalWrite(LED, LOW);//led
    }
    Serial.println("");
    Serial.println("Connected");
    digitalWrite(LED, HIGH);//led
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    
    //delay(1000);
}
//=======================================================================
