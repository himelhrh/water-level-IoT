/*Water level monitoring system with the New Blynk app*/
//Include the library files
#define BLYNK_TEMPLATE_ID "TMPL6qP-6AjSR"
#define BLYNK_TEMPLATE_NAME "Water level monitoring system"
#define BLYNK_AUTH_TOKEN "LokcXsrwd2c2yJVIhf_UGQ_sVVYAR9XV"

#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "LokcXsrwd2c2yJVIhf_UGQ_sVVYAR9XV";//Enter your Auth token
char ssid[] = "WaterLevel";//Enter your WIFI name
char pass[] = "12345678";//Enter your WIFI password

BlynkTimer timer;

// Define the component pins
#define trig D7
#define echo D8
#define LED1 D0
#define LED2 D3
#define LED3 D4
#define LED4 D5
#define LED5 D6
#define relay 3

//Enter your tank max value(CM)
int MaxLevel = 20;

int Level1 = (MaxLevel * 75) / 100;
int Level2 = (MaxLevel * 65) / 100;
int Level3 = (MaxLevel * 55) / 100;
int Level4 = (MaxLevel * 45) / 100;
int Level5 = (MaxLevel * 35) / 100;

const int numReadings = 20;
int readings[numReadings];      // the readings from the ultrasonic sensor
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int readingCount = 0;           // the count of readings

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  lcd.setCursor(0, 0);
  lcd.print("Water level");
  lcd.setCursor(4, 1);
  lcd.print("Monitoring");
  delay(4000);
  lcd.clear();

  Serial.println("Setup Complete");

  // initialize all the readings to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  //Call the functions
  timer.setInterval(100L, ultrasonic);
}

//Get the ultrasonic sensor values
void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  int distance = t / 29 / 2;

  int blynkDistance = (distance - MaxLevel) * -1;
  if (distance <= MaxLevel) {
    Blynk.virtualWrite(V0, blynkDistance);
  } else {
    Blynk.virtualWrite(V0, 0);
  }

  // Subtract the last reading:
  total = total - readings[readIndex];
  // Read from the sensor:
  readings[readIndex] = distance;
  // Add the reading to the total:
  total = total + readings[readIndex];
  // Advance to the next position in the array:
  readIndex = readIndex + 1;

  // If we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // Increment the reading count
  readingCount++;

  // Calculate and send the average every 20 readings
  if (readingCount >= numReadings) {
    average = total / numReadings;
    Blynk.virtualWrite(V2, MaxLevel - average);
    readingCount = 0;  // Reset the reading count after computing the average
  }

  lcd.setCursor(0, 0);
  lcd.print("WLevel:");

  if (Level1 <= distance) {
    lcd.setCursor(8, 0);
    lcd.print("Very Low");
    Serial.print("Very Low");
    Serial.println(distance);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
  } else if (Level2 <= distance && Level1 > distance) {
    lcd.setCursor(8, 0);
    lcd.print("Low");
    lcd.print("      ");
    Serial.print("Low");
    Serial.println(distance);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
  } else if (Level3 <= distance && Level2 > distance) {
    lcd.setCursor(8, 0);
    lcd.print("Medium");
    lcd.print("      ");
    Serial.print("Medium");
    Serial.println(distance);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
  } else if (Level4 <= distance && Level3 > distance) {
    lcd.setCursor(8, 0);
    lcd.print("High");
    lcd.print("      ");
    Serial.print("High");
    Serial.println(distance);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, LOW);
  } else if (Level5 >= distance) {
    lcd.setCursor(8, 0);
    lcd.print("Full");
    lcd.print("      ");
    Serial.print("Full");
    Serial.println(distance);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, HIGH);
  }
}

//Get the button value
BLYNK_WRITE(V1) {
  bool Relay = param.asInt();
  if (Relay == 1) {
    digitalWrite(relay, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON ");
  } else {
    digitalWrite(relay, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF");
  }
}

void loop() {
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer
}