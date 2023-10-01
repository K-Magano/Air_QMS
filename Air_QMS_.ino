#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"

int buzzerPin = 7;
int MQ2 = A0; // smoke
int MQ6 = A1; // lpg
int SMOKE_THRESHOLD = 100; // Set your smoke threshold value
int LPG_THRESHOLD = 300;   // Set your LPG threshold value

SoftwareSerial mySerial(2, 3);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(MQ2, INPUT);

  mySerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Initializing...");
  delay(1000);

  mySerial.println("AT");
  delay(1000); // Sends an ATTENTION command, reply should be OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuration for sending SMS
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Configuration for receiving SMS
  updateSerial();

  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.print("Gas & Smoke Monitoring");
}

void loop() {
  int MQ2Value = analogRead(MQ2);
  int MQ6Value = analogRead(MQ6);

  String smokeData = "Smoke: " + String(MQ2Value) + " ppm";
  String lpgData = "LPG: " + String(MQ6Value) + " ppm";

  Serial.println("");
  Serial.println("Gas & Smoke Monitoring:");
  Serial.println(smokeData);
  Serial.println(lpgData);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Air Monitoring");

  if (MQ2Value > SMOKE_THRESHOLD) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Smoke Detected!");
    Serial.println("Smoke detected!");
    tone(buzzerPin, 1000);
    delay(1000);
       makeCall(); // Call when smoke detected
    delay(1000);
  } else {
    lcd.setCursor(0, 1);
    noTone(buzzerPin);
    
  }

  if (MQ6Value > LPG_THRESHOLD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LPG Detected, ");
    lcd.setCursor(0,1);
    lcd.print("Please check gas cylinders!");
    Serial.println("LPG Detected, Please check gas cylinders");
    tone(buzzerPin, 1000);
        makeCall(); // Call when LPG detected
    delay(1000);
  } else {
    lcd.setCursor(0, 0);
    noTone(buzzerPin);
      }

  updateSerial();
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    String cmd = Serial.readString();
    cmd.trim();
    if (cmd.length()>0) {
      if (cmd.equals("S")) {
        sendSMS();
      } else if (cmd.equals("C")) {
        makeCall();
      } else {
        mySerial.println(cmd);
      }
    }
  }

  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}

void sendSMS() {
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.println("AT+CMGS=\"+2783585655\"");
  delay(500);
  mySerial.print("Dangerous gas leak, please check the house and evacuate!");
  delay(500);
  mySerial.write(26);
}

void makeCall() {
  mySerial.println("ATD+27831234669;"); // Replace with the phone number you want to call
}
