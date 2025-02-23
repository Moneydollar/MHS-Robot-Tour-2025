#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ─────────────────────────── MOTOR CONFIGURATION ────────────────────────────
// Motor A pins
int motorAEnablePin = 9;
int motorAInput1Pin = 8;
int motorAInput2Pin = 7;

int motorBEnablePin = 3;
int motorBInput1Pin = 4;
int motorBInput2Pin = 5;

// Speed constants
float MAX_SPEED = 255;
float HALF_SPEED = MAX_SPEED * 0.5;

// Distance conversion
const float WHEEL_DIAMETER_CM = 6.35;  // 2.5 inches in cm
const float WHEEL_CIRCUMFERENCE_CM = WHEEL_DIAMETER_CM * 3.1416; // Circumference in cm
const float STD_DISTANCE = 0.50; // 25 cm standard move distance in meters

// ─────────────────────────── MPU6050 CONFIGURATION ────────────────────────────
Adafruit_MPU6050 mpu;
float currentAngle = 0.0;
float gyroBiasZ = 0.0;
unsigned long lastTime = 0;
float alpha = 0.9; // Low-pass filter adjustment

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }

  calibrateIMU(2000); 

  pinMode(motorAEnablePin, OUTPUT);
  pinMode(motorBEnablePin, OUTPUT);
  pinMode(motorAInput1Pin, OUTPUT);
  pinMode(motorAInput2Pin, OUTPUT);
  pinMode(motorBInput1Pin, OUTPUT);
  pinMode(motorBInput2Pin, OUTPUT);

  Serial.println("Setup Complete!");
}

// ─────────────────────────── IMU CALIBRATION ────────────────────────────
void calibrateIMU(int samples) {
  Serial.println("Calibrating IMU...");
  float sum = 0.0;

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    sum += g.gyro.z;
    delay(1);
  }

  gyroBiasZ = sum / samples;
  Serial.print("Gyro Bias Z: ");
  Serial.println(gyroBiasZ, 6);
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

// Move forward for a given distance (meters)
void moveForward(int speed, float distanceMeters) {
  float distanceCM = distanceMeters * 100;  // Convert to cm
  float rotations = distanceCM / WHEEL_CIRCUMFERENCE_CM;  // Calculate how many rotations needed
  float timeToMove = rotations * 1000;  // Calculate how much time it should take (in ms)

  Serial.print("Moving Forward: ");
  Serial.print(distanceCM);
  Serial.println(" cm");

  digitalWrite(motorAInput1Pin, HIGH);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, HIGH);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);

  delay(timeToMove); // Move for calculated time
  stopMotors();
}

// Move backward for a given distance (meters)
void moveBackward(int speed, float distanceMeters) {
  float distanceCM = distanceMeters * 100;  // Convert to cm
  float rotations = distanceCM / WHEEL_CIRCUMFERENCE_CM;  // Calculate how many rotations needed
  float timeToMove = rotations * 1000;  // Calculate how much time it should take (in ms)

  Serial.print("Moving Backward: ");
  Serial.print(distanceCM);
  Serial.println(" cm");

  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, HIGH);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, HIGH);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);

  delay(timeToMove); // Move for calculated time
  stopMotors();
}

// ─────────────────────────── GYROSCOPE FUNCTIONS ────────────────────────────
void resetAngle() {
  currentAngle = 0.0;
  lastTime = millis();
}

void updateAngle() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;

  float gyroZ = g.gyro.z - gyroBiasZ;
  currentAngle += gyroZ * deltaTime * 57.2958;

  lastTime = currentTime;

  Serial.print("Current Angle: ");
  Serial.println(currentAngle);
}

// ─────────────────────────── TURN FUNCTIONS ────────────────────────────
void turnLeft(int speed, float angle) {
  resetAngle();
  Serial.println("Turning Left...");

  digitalWrite(motorAInput1Pin, HIGH);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, HIGH);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);

  while (abs(currentAngle) < angle) {
    updateAngle();
    delay(10);
  }

  stopMotors();
  Serial.println("Turn Left Done!");
}

void turnRight(int speed, float angle) {
  resetAngle();
  Serial.println("Turning Right...");

  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, HIGH);
  digitalWrite(motorBInput1Pin, HIGH);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);

  while (abs(currentAngle) < angle) {
    updateAngle();
    delay(10);
  }

  stopMotors();
  Serial.println("Turn Right Done!");
}

// ─────────────────────────── MAIN LOOP ────────────────────────────
void loop() {

  moveForward(MAX_SPEED, STD_DISTANCE); 
  delay(500);
  
  turnLeft(HALF_SPEED, 90);
  delay(500);


  moveForward(MAX_SPEED, STD_DISTANCE);
  delay(500);

 
  turnRight(HALF_SPEED, 90);
  delay(500);

  
  moveBackward(HALF_SPEED, 1); 
  delay(500);

  while (1); // Halt the program
}
