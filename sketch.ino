
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin configuration
#define TRIG_PIN 5
#define ECHO_PIN 18

#define SERVO_PIN 19

#define RED_LED 26
#define GREEN_LED 25

#define BUZZER 27

// LCD configuration
#define SDA_PIN 21
#define SCL_PIN 22

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Objects
Servo scanner;

// Settings
int obstacleLimit = 20;

// Measure distance
long getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return 400;
  }

  return duration * 0.034 / 2;
}

// Scan a direction
long scanDirection(int angle) {

  scanner.write(angle);
  delay(700);

  long distance = getDistance();

  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" | Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

// Display LCD message
void displayMessage(String line1, String line2) {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(line1);

  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {

  Serial.begin(115200);

  // Sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // LED and buzzer pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // LCD initialization
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  displayMessage("AUTONOMOUS CAR", "SYSTEM STARTING");

  // Servo initialization
  scanner.attach(SERVO_PIN);
  scanner.write(90);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.println("AUTONOMOUS VEHICLE SYSTEM");
  Serial.println("Obstacle Avoidance Started");

  delay(2000);

  displayMessage("SYSTEM READY", "SCANNING FRONT");

  delay(1000);
}

void loop() {

  // Look forward
  scanner.write(90);
  delay(300);

  long frontDistance = getDistance();

  Serial.print("Front Distance: ");
  Serial.print(frontDistance);
  Serial.println(" cm");

  // Road is clear
  if (frontDistance > obstacleLimit) {

    Serial.println("ROAD CLEAR - MOVING FORWARD");

    displayMessage("ROAD CLEAR", "MOVING FORWARD");

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    delay(300);
  }

  // Obstacle detected
  else {

    Serial.println("OBSTACLE DETECTED!");
    Serial.println("VEHICLE STOPPED");

    displayMessage("OBSTACLE FOUND", "VEHICLE STOPPED");

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);

    delay(500);

    digitalWrite(BUZZER, LOW);

    // Scan left
    Serial.println("SCANNING LEFT...");

    displayMessage("SCANNING LEFT", "CHECKING PATH");

    long leftDistance = scanDirection(180);

    // Scan right
    Serial.println("SCANNING RIGHT...");

    displayMessage("SCANNING RIGHT", "CHECKING PATH");

    long rightDistance = scanDirection(0);

    // Return sensor to front
    scanner.write(90);
    delay(500);

    // Select direction
    if (leftDistance > rightDistance &&
        leftDistance > obstacleLimit) {

      Serial.println("TURN LEFT - PATH CLEAR");

      displayMessage("PATH CLEAR", "TURN LEFT");

    }

    else if (rightDistance > obstacleLimit) {

      Serial.println("TURN RIGHT - PATH CLEAR");

      displayMessage("PATH CLEAR", "TURN RIGHT");

    }

    else {

      Serial.println("BOTH SIDES BLOCKED");
      Serial.println("WAITING FOR CLEAR PATH");

      displayMessage("BOTH SIDES", "PATH BLOCKED");

      delay(1500);
    }

    digitalWrite(RED_LED, LOW);

    delay(500);
  }
}
