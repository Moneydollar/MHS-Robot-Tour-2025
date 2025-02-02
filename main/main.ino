#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ─────────────────────────── MOTOR CONFIGURATION ────────────────────────────
// Motor A pins
int motorAEnablePin = 9;
int motorAInput1Pin = 8;
int motorAInput2Pin = 7;

// Motor B pins
int motorBEnablePin = 3;
int motorBInput1Pin = 4;
int motorBInput2Pin = 5;

// Speed constants
float MAX_SPEED = 255;
float HALF_SPEED = MAX_SPEED * 0.5;

// ─────────────────────────── MPU6050 CONFIGURATION ────────────────────────────
Adafruit_MPU6050 mpu;
float currentAngle = 0.0;
float lastGyroZ = 0.0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }

  // Set motor pins as outputs
  pinMode(motorAEnablePin, OUTPUT);
  pinMode(motorBEnablePin, OUTPUT);
  pinMode(motorAInput1Pin, OUTPUT);
  pinMode(motorAInput2Pin, OUTPUT);
  pinMode(motorBInput1Pin, OUTPUT);
  pinMode(motorBInput2Pin, OUTPUT);

  Serial.println("Setup Complete!");
}

// ─────────────────────────── MOTOR CONTROL FUNCTIONS ────────────────────────────
void stopMotors() {
  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, 0);
  analogWrite(motorBEnablePin, 0);
}

// Move motors for forward motion
void moveForward(int speed) {
  digitalWrite(motorAInput1Pin, HIGH);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, HIGH);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);
}

// Move motors for backward motion
void moveBackward(int speed) {
  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, HIGH);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, HIGH);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);
}

// ─────────────────────────── GYROSCOPE FUNCTIONS ────────────────────────────
void updateAngle() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0; // Convert to seconds

  float gyroZ = g.gyro.z; // Angular velocity in radians/sec
  currentAngle += gyroZ * deltaTime * 57.2958; // Convert to degrees

  lastTime = currentTime;
}

// ─────────────────────────── TURN FUNCTIONS ────────────────────────────
// Turn left using gyroscope feedback
void turnLeft(int speed, float angle) {
  currentAngle = 0; // Reset angle tracking
  lastTime = millis();
  
  digitalWrite(motorAInput1Pin, HIGH);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, HIGH);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);

  while (abs(currentAngle) < angle) {
    updateAngle();
  }

  stopMotors();
}

// Turn right using gyroscope feedback
void turnRight(int speed, float angle) {
  currentAngle = 0; // Reset angle tracking
  lastTime = millis();
  
  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, HIGH);
  digitalWrite(motorBInput1Pin, HIGH);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);

  while (abs(currentAngle) < angle) {
    updateAngle();
  }

  stopMotors();
}

// ─────────────────────────── MAIN LOOP ────────────────────────────
void loop() {
  moveForward(MAX_SPEED);
  delay(1000);
  stopMotors();

  delay(500);
  turnLeft(HALF_SPEED, 90); // Turn left 90 degrees

  delay(500);
  moveForward(MAX_SPEED);
  delay(1000);
  stopMotors();

  delay(500);
  turnRight(HALF_SPEED, 90); // Turn right 90 degrees

  while (1); // Halt the program
}
