#include <Wire.h> 
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <HTTPClient.h>
#include <addons/TokenHelper.h>


// #define WIFI_SSID "Redmi Note 11"
// #define WIFI_PASSWORD "123456789"

#define WIFI_SSID "Ahmad"
#define WIFI_PASSWORD "907676100"

#define API_KEY "AIzaSyAf3B1FulNpmbaneaOnduRR1R6Ce7LOnUc"
#define FIREBASE_PROJECT_ID "hardware-graduation-project"

#define USER_EMAIL "esp32_hw@gmail.com"
#define USER_PASSWORD "ESP32_HW"

#define Collection_Name "SIGNED_IN"
#define Document_ID "User_ID"

LiquidCrystal_I2C lcd(0x27,16,2);
 
#define ROWS  4
#define COLS  4
 
char keyMap[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
 
uint8_t colPins[ROWS] = {14, 27, 26, 25}; // GIOP14, GIOP27, GIOP26, GIOP25
uint8_t rowPins[COLS] = {33, 32, 18, 19}; // GIOP33, GIOP32, GIOP18, GIOP19


Keypad keypad = Keypad(makeKeymap(keyMap), rowPins, colPins, ROWS, COLS ); 
uint8_t LCD_CursorPosition = 0;

unsigned long dataMillis = 0;

char key = 0;
int signing_in = 0;
// char id[] ={'0','0','0','0'}; 
String id = "" ;

// char Indicator_To_Start_ESP ;
String Online_UserID = "" ;

FirebaseData fbdo;
FirebaseAuth auth;
HTTPClient http;
FirebaseConfig config;

//motion sensor declaration 

const int motionPin = 34;  // GPIO pin connected to motion sensor output     
// const int ledPin = 4 ;

int calibrationTime = 10; //give yourself time to leave the room (prepare the sensor)   
long unsigned int lowIn;         
long unsigned int pauseDuration  = 5000;  

boolean lockLow = true;
boolean takeLowTime;  


void WiFiConnection(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    // Serial.print(".");
    delay(300);
  }
  // Serial.println();
  // Serial.print("Connected with IP: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();

  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // Firebase.reconnectWiFi(true);
}

void Config_Firestore(){
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  
  config.token_status_callback = tokenStatusCallback;

  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);  
}

void setup()
{
  Serial.begin(9600);
  delay(1000); // Wait for serial port to stabilize
  // Serial.println();

  lcd.init();
  lcd.backlight();

  WiFiConnection();

  Config_Firestore();

  pinMode(motionPin, INPUT);
  // pinMode(ledPin, OUTPUT);
  digitalWrite(motionPin, LOW);

  for(int i = 0; i < calibrationTime; i++){
    delay(500);
  }
  delay(50);
}

void uselessfunctionfornow(){
  char key = 0;
  if (key) {
    Serial.print(key);
    lcd.setCursor(LCD_CursorPosition++, 0);
    if(LCD_CursorPosition == 16 || key == 'D')
    {
      lcd.clear();
      LCD_CursorPosition = 0;
    }
    else
    {
      lcd.print(key);
    }
  }
}


String GET_ID_FROM_FIREBASE_QR_SIGNEDIN(){
  HTTPClient http;

  String url = "https://v1.nocodeapi.com/khalid_j02/fbsdk/ihuizOibbjMBqWhm/firestore/allDocuments?collectionName=SIGNED_IN";

  http.begin(url.c_str());

  int httpResponseCode = http.GET();

  // Check the response code
  if (httpResponseCode == 200) {
    String response = http.getString();

    String attributeKey = "ID\":{\"stringValue";
    int attributeStartIndex = response.indexOf(attributeKey);
    if (attributeStartIndex != -1) {
      attributeStartIndex += attributeKey.length() + 3; // Skip ": "
      int attributeEndIndex = attributeStartIndex + 4 ;//response.indexOf("\",\"valueType", attributeStartIndex);

      Online_UserID = response.substring(attributeStartIndex, attributeEndIndex);
      // Serial.println("Online User ID: " + Online_UserID);
      // id = Online_UserID ; 
    } 
    else {
      // do nothing
      // Serial.println("Attribute not found in the JSON response");
    }
  } 
  
  else {
    // do nothing
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
  }

  http.end();

  signing_in = 1; 
  return Online_UserID ;
}


void Update_UserId_Using_Firebase(){

  if (Firebase.ready() && (millis() - dataMillis > 60000 || dataMillis == 0)){
    dataMillis = millis();
    // count++;

    // Serial.print("Commit a document (set server value, update document)... ");

    // The dyamic array of write object fb_esp_firestore_document_write_t.
    std::vector<struct fb_esp_firestore_document_write_t> writes;


    // Add another write for update
    //A write object that will be written to the document.
    struct fb_esp_firestore_document_write_t update_write;

    update_write.type = fb_esp_firestore_document_write_type_update;

    //Set the document content to write (transform)

    FirebaseJson content;
    String documentPath = "SIGNED_IN/User_ID" ;

    content.set("fields/ID/stringValue" , id);

    update_write.update_document_content = content.raw();
        
    update_write.update_document_path = documentPath.c_str();

    //Add a write object to a write array.
    writes.push_back(update_write);

    if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of fb_esp_firestore_document_write_t */, "" /* transaction */)){
      //  Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      Online_UserID = id ;
      //  strcpy(Online_UserID, id.c_str());   // the online user will be store here.
    }
    else
      Serial.println(fbdo.errorReason());
   
  }
}

String Enter_UserID(){
  String enterID = "";

  while (true) {
    char key = keypad.getKey();

    if (key != NO_KEY) {
      if (key == '#') {
        // Return the entered ID
        return enterID;
      } else if (key == 'D') {
        // Handle backspace
        if (enterID.length() > 0) {
          enterID.remove(enterID.length() - 1);
          lcd.setCursor(enterID.length(), 1);
          lcd.print(" ");
        }
      } else {
        // Append the digit to the ID
        if (enterID.length() < 4) {
          enterID += key;
          lcd.setCursor(enterID.length() - 1, 1);
          lcd.print(key);
        }
      }
    }
  }

}

void READ_KEYPAD_THEN_SIGNIN(){
  //read id from KP 
  //change the signing in in the Firebase 
  //save the id in id[]
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Your ID: ");
  // lcd.setCursor(0, 1);

  id = Enter_UserID();

  if (id.length() < 4) {
    lcd.clear();
    lcd.print("ID is not valid!");

    READ_KEYPAD_THEN_SIGNIN(); 

  } else {
    Update_UserId_Using_Firebase() ;

    lcd.clear();
    lcd.print("ID is valid!");
    
  }
  delay(2000);  // Delay for 2 seconds before clearing the LCD
  
  signing_in = 1;  
}


void Trigger_Motion_Sensor(){
  while(true){
    if(digitalRead(motionPin) == HIGH){
      // digitalWrite(ledPin, HIGH);   
      delay(100);
      // digitalWrite(ledPin, LOW);   
      delay(100);
      // digitalWrite(ledPin, HIGH);   
      delay(100);
    if(lockLow){  
      lockLow = false;
      delay(50);
    }         
    takeLowTime = true;
    }

    if(digitalRead(motionPin) == LOW){       
      // digitalWrite(ledPin, LOW);

      if(takeLowTime){
        lowIn = millis();         
        takeLowTime = false;      
      }
      
      if(!lockLow && millis() - lowIn > pauseDuration ){  
            
        lockLow = true;                        
        delay(50);
        break ;
      }
    }
  }
}

void loop()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Pass Over Motion");

  lcd.setCursor(0, 1);
  lcd.print("To Sign In");
   
  delay(3000);

  Trigger_Motion_Sensor();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(" WELCOME ... ");
   
  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Sign in By :");
  lcd.setCursor(0, 1);
  lcd.print("1QR 2KP 3NoUser");

  while (true)
  {
    key = keypad.getKey();
    if(key >= '1' && key <= '3')
      break;
  }

  switch(key)
  {
    case '1' :  
      delay(3000);  // setup to make sure the mobile change the QR code correctly 
      id = GET_ID_FROM_FIREBASE_QR_SIGNEDIN();
      break;
    case '2' : 
      READ_KEYPAD_THEN_SIGNIN(); 
      break;
    case '3' : 
      signing_in = 0; 
      break; // not a user
  }


  if(signing_in)
  {
    // send the ID to raspi 
    
    Serial.println("ID"); 
    for (int i = 0; i < 4; i++)
    Serial.print(id[i]);
    Serial.println();
  
    //print here the name of the user or id ur choice 
    char message[20];
    snprintf(message, sizeof(message), "Sign in By: %s", id);

    lcd.setCursor(0, 0);
    lcd.print(message);
  }
  else{
    Serial.println("NoUser");
  }

  // strcpy(id , "");
  id = "0000";


  lcd.setCursor(0 , 1);
  lcd.print("Pass over sensor");

  Trigger_Motion_Sensor();

  Serial.println("Done");

  lcd.setCursor(0, 0);
  lcd.print(" Thanks 4 saving ");
  lcd.setCursor(0, 1);
  lcd.print("     earth!     ");

  delay(3000);
}