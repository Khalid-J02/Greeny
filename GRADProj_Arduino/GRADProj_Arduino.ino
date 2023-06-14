#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // library for LCD with I2C
#include <Servo.h>

#define trigPin 6
#define echoPin 7

#define trigPin_classification 4
#define echoPin_classification 5

#define Servo1 12   // these two we will change them ..

#define LIMIT_1 2
#define LIMIT_2 3
#define LIMIT_3 18
#define LIMIT_4 19
// this is for the switches

#define in1 11
#define in2 10
#define in3 9
#define in4 8
const int t= 20;
// this is for the stepper motor and L298N driver

struct TrashLevels {
  int Glass;
  int Metal;
  int Plastic;
  int SomethingElse;
};


LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address (0x27) and size of lcd (16 chars and 2 line display)

Servo Servo_Open_Bin_After_Classification ;

int Object_Detected = 0 ;

int DistanceBetweenUltraSonicAndBin = 18;  //modify this the height of ultra - 20cm
TrashLevels trashLevels;

char ActiveTrashLevel ;

char WasteType ;

volatile bool limit1Flag = false ;
volatile bool limit2Flag = false ;
volatile bool limit3Flag = false ;
volatile bool limit4Flag = false ;

void limitInterrupt1(){
  Serial.println(WasteType);
  if(WasteType == 'E'){
    // Serial.println('E');
    ActiveTrashLevel = 'E' ;
    // Operate_Stepper_Motor(0);
    limit1Flag = false ;
  } 
}

void limitInterrupt2(){
  Serial.println(WasteType);
  if(WasteType == 'G'){
    // Serial.println('G');
    ActiveTrashLevel = 'G' ;
    // Operate_Stepper_Motor(0);
    limit2Flag = false ;
  } 
}
void limitInterrupt3(){
  Serial.println(WasteType);
  if(WasteType == 'M'){
    // Serial.println('M');
    ActiveTrashLevel = 'M' ;
    // Operate_Stepper_Motor(0);
    limit3Flag = false ;
  } 
}
void limitInterrupt4(){
  Serial.println(WasteType);
  if(WasteType == 'P'){
    // Serial.println('P');
    ActiveTrashLevel = 'P' ;
    // Operate_Stepper_Motor(0);
    limit4Flag = false ;
  }
}


void setup() {

  Serial.begin(9600);

  Servo_Open_Bin_After_Classification.attach(Servo1);
  Servo_Open_Bin_After_Classification.write(176);

  // setup LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(trigPin_classification, OUTPUT);
  pinMode(echoPin_classification, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(LIMIT_1, INPUT_PULLUP);
  pinMode(LIMIT_2, INPUT_PULLUP);
  pinMode(LIMIT_3, INPUT_PULLUP);
  pinMode(LIMIT_4, INPUT_PULLUP);

  // interrrupt on falling due to the connection of the limit switch
  attachInterrupt(digitalPinToInterrupt(LIMIT_1), limitInterrupt1, FALLING);
  attachInterrupt(digitalPinToInterrupt(LIMIT_2), limitInterrupt2, FALLING);
  attachInterrupt(digitalPinToInterrupt(LIMIT_3), limitInterrupt3, FALLING);
  attachInterrupt(digitalPinToInterrupt(LIMIT_4), limitInterrupt4, FALLING);


  trashLevels.Glass = 0;
  trashLevels.Metal = 0;
  trashLevels.Plastic = 0;
  trashLevels.SomethingElse = 0;

  lcd.clear();

  lcd.setCursor(0, 0);            //  G   M   P   E
  lcd.print(" G   M   P   E  ");  // N/A N/A N/A N/A 
  lcd.setCursor(0, 1);
  lcd.print("N/A N/A N/A N/A ");

}


void Start_Classification(){
  while (true) {
    digitalWrite(trigPin_classification, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin_classification, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_classification, LOW);
    
    // Measure the echo duration and calculate distance
    long duration = pulseIn(echoPin_classification, HIGH);
    float distance = duration * 0.034 / 2;  // Speed of sound = 34,300 cm/s
    
    Serial.println(distance);
    if(distance < 13.5){  // check if the distance is 80 cm then start taking data from the user
     // delay(2000);    // to make sure user hand doen't appears in the picture
      Serial.println(distance);
      Serial.println("Classify");  // indicators as a start
      Object_Detected = 1 ;
      break ;
    }
    else{
      // do nothing and go back to check
    }
  }
}

void Operate_Servo_Motor(){
  // Serial.println(Object_Detected);
  Servo_Open_Bin_After_Classification.write(0) ;
  delay(2500);

  Servo_Open_Bin_After_Classification.write(178);
  delay(2500);  
  Object_Detected = 0 ;  
  
}

void SetPercentage(int lvl) {
  switch (ActiveTrashLevel) {
    case 'G': trashLevels.Glass = lvl; break;
    case 'P': trashLevels.Plastic = lvl; break;
    case 'M': trashLevels.Metal = lvl; break;
    default: trashLevels.SomethingElse = lvl; break;
  }
}

void PrintLevelsOnLCD() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("G   M   P   E   ");

  lcd.setCursor(0, 1);
  if (trashLevels.Glass >= 100) {
    lcd.print("F");
  } else if (trashLevels.Glass <= 0) {
    lcd.print("E");
  } else {
    lcd.print(String(trashLevels.Glass));
  }

  lcd.setCursor(4, 1);
  if (trashLevels.Metal >= 100) {
    lcd.print("F");
  } else if (trashLevels.Metal <= 0) {
    lcd.print("E");
  } else {
    lcd.print(String(trashLevels.Metal));
  }
  // trashLevels.Metal >=100 && trashLevels.Metal <= 0 ? lcd.print("F") : lcd.print(String(trashLevels.Metal));

  lcd.setCursor(8, 1);
  if (trashLevels.Plastic >= 100) {
    lcd.print("F");
  } else if (trashLevels.Plastic <= 0) {
    lcd.print("E");
  } else {
    lcd.print(String(trashLevels.Plastic));
  }
  // trashLevels.Plastic >=100 ? lcd.print("F") : lcd.print(String(trashLevels.Plastic));

  lcd.setCursor(12, 1);
  if (trashLevels.SomethingElse >= 100) {
    lcd.print("F");
  } else if (trashLevels.SomethingElse <= 0) {
    lcd.print("E");
  } else {
    lcd.print(String(trashLevels.SomethingElse));
  }
  // trashLevels.SomethingElse >=100 ? lcd.print("F") : lcd.print(String(trashLevels.SomethingElse));

  delay(100);
}

void ReadUltraSonic_LEVELS_AndShowOnLCD() { 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
  long duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  int ultraSonic2Distance = duration * 0.034 / 2;

  int percentage = (20 + DistanceBetweenUltraSonicAndBin - ultraSonic2Distance) * 5;  // 20 is the bin hieght and *5 is (*100%/20)
  SetPercentage(percentage);
  PrintLevelsOnLCD();
}

void Wait_Waste_Type_From_Raspi(){ // this will keep waiting the waste type from raspi

  while (true) {
    if(Serial.available()){
      WasteType = Serial.read();
      if(WasteType == 'M' ){
        limit3Flag = true ; 
        break ;
      }
      else if(WasteType == 'P'){
        limit4Flag = true ;
        break;
      }
      else if(WasteType == 'G'){
        limit2Flag = true ;
        break;
      }
      else if(WasteType == 'E'){
        limit1Flag = true ;
        break;
      }
      else{
        // keep waiting 
      }
    }
  }   // this is for keep waiting data from the serial and activating flag
  Serial.println();
  Serial.print("Waste Type is send =");
  Serial.print(WasteType);
  Serial.println("#");

}

void Operate_Stepper_Motor(){   // if 0 ==> off ,   1 ==> on
  while (limit1Flag || limit2Flag || limit3Flag || limit4Flag) {
    digitalWrite(in4, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in1, LOW); 
    delay(t);
    digitalWrite(in4, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in1, LOW); 
    delay(t);
    digitalWrite(in4, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in1, LOW); 
    delay(t);
    digitalWrite(in4, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in2, HIGH);
    digitalWrite(in1, LOW); 
    delay(t);
    digitalWrite(in4, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in1, LOW);
    delay(t);
    digitalWrite(in4, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in1, HIGH); 
    delay(t); 
    digitalWrite(in4, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);
    delay(t);
    digitalWrite(in4, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH); 
    delay(t);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // Operate_Servo_Motor();
  Start_Classification();
  Wait_Waste_Type_From_Raspi();
  Operate_Stepper_Motor();
  while(limit1Flag || limit2Flag || limit3Flag || limit4Flag){
    // here the system will keep waiting till the appropriate flag become false  
  }
  // Operate_Stepper_Motor(0);
  delay(500);
  Operate_Servo_Motor();
  delay(300);

  ReadUltraSonic_LEVELS_AndShowOnLCD();
  delay(1500);

}
