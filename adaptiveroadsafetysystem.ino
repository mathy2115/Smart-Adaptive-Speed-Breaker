#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- PINOUT ---
#define TRIG1 5
#define ECHO1 19
#define TRIG2 18
#define ECHO2 32  
#define SERVO_PIN 13
#define GREEN_LED 25
#define YELLOW_LED 26
#define RED_LED 27

Servo flap;
const float sensorDistance = 0.20; // 20 cm

// State tracking
bool sensor1Triggered = false; 
bool sensor2Triggered = false; 
unsigned long t1 = 0;
unsigned long t2 = 0;
unsigned long systemLockoutTime = 0; 

float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if(duration == 0) return 999;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);
  
  // THE FIX: Give the power rails 0.5 seconds to stabilize!
  delay(500); 
  
  // Start the screen FIRST before attaching heavy hardware
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  SMART SPEED   ");
  lcd.setCursor(0, 1);
  lcd.print("    BREAKER     ");
  
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Now attach the servo (this is what caused the power spike)
  flap.attach(SERVO_PIN);
  flap.write(15); 
  
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Hold the title for 2 seconds, then go Stealth
  delay(2000);
  lcd.clear(); 
}

void loop() {
  if (millis() < systemLockoutTime) return; 

  float d1 = readDistance(TRIG1, ECHO1);
  float d2 = readDistance(TRIG2, ECHO2);

  // --- TIMEOUT CLEANUP ---
  if (sensor1Triggered && (millis() - t1 > 3000)) {
    sensor1Triggered = false;
    lcd.clear();
  }
  if (sensor2Triggered && (millis() - t2 > 3000)) {
    sensor2Triggered = false;
    lcd.clear();
  }

  // --- SENSOR 1 (ENTRY) ---
  if (d1 > 0 && d1 < 10) {
    if (!sensor1Triggered && !sensor2Triggered) {
      t1 = millis();
      sensor1Triggered = true;
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle at S1...");
      delay(200); 
    } 
    else if (sensor2Triggered) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("! WRONG WAY !");
      lcd.setCursor(0,1);
      lcd.print("VIOLATION LOGGED");
      
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH); 
      systemLockoutTime = millis() + 3000; 
      delay(2000); 
      
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      sensor2Triggered = false;
      lcd.clear();
    }
  }

  // --- SENSOR 2 (EXIT) ---
  if (d2 > 0 && d2 < 10) {
    if (!sensor1Triggered && !sensor2Triggered) {
      t2 = millis();
      sensor2Triggered = true;
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle at S2...");
      delay(200); 
    } 
    else if (sensor1Triggered) {
      unsigned long totalTime = millis();
      float timeTaken = (totalTime - t1) / 1000.0;
      float speed = sensorDistance / timeTaken;

      lcd.clear();
      lcd.setCursor(0, 0);
      
      lcd.print("T:"); 
      lcd.print(timeTaken, 1); 
      lcd.print("s ");
      lcd.print("S:"); 
      lcd.print(speed, 1); 
      lcd.print("m/s");

      lcd.setCursor(0, 1);

      if(speed > 0.25) { // The scale limit update
        lcd.print(" ! OVERSPEED !  ");
        
        digitalWrite(GREEN_LED, LOW);   
        digitalWrite(YELLOW_LED, HIGH); 

        flap.write(90); 
        delay(2000);    
        flap.write(15); 
        
        digitalWrite(YELLOW_LED, LOW);  
        digitalWrite(GREEN_LED, HIGH);  
      } else {
        lcd.print("  NORMAL SPEED  ");
        delay(2000);
      }
      
      systemLockoutTime = millis() + 3000; 
      sensor1Triggered = false;
      lcd.clear(); 
    }
  }
}