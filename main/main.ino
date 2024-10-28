#include <Arduino.h>


// ┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
// │                                                                                                      │
// │                                                                                                      │
// │                                                                                                      │
// │                               ┌─────────┐                     ┌────────┐                             │
// │         wheelA1   │           │  ┌────────────────────────────────┐    │         │  wheelB1          │
// │                   │           │  │      │                     │   │    │         │                   │
// │                   │           │  │      │                     │   │    │         │                   │
// │                   └────────►  │  │      │                     │   │    │         │                   │
// │                               │  │      │                     │   │    │  ◄──────┘                   │
// │                               └──│──────┘                     └───│────┘                             │
// │                                  │                                │                                  │
// │                                  │                                │   xxxxxxx                        │
// │                                  │                                │   xx    xxx                      │
// │                         xxxx     │                                │   xx      xx                     │
// │                        xx  xx    │                                │   xx       x                     │
// │                       xx    xx   │                                │   x       xx                     │
// │                    xx x      xx  │                                │   x xxxxxxx                      │
// │                     xxxxxxxxxxxx │                                │   xx xx xxx                      │
// │                      x         x │                                │   xx      x                      │
// │                     x          x │                                │   xx      x                      │
// │                                  │                                │   xx      x                      │
// │                                  │                                │   xx    xx                       │
// │                                  │                                │   xx  xxx                        │
// │                                  │                                │   xx xx                          │
// │                                  │                                │   xxxx                           │
// │                                  │                                │                                  │
// │                                  │                                │                                  │
// │             wheelA2  │        ┌──│────┐                      ┌────│──┐                               │
// │                      │        │  │    │                      │    │  │      │ wheelB2                │
// │                      │        │  │    │                      │    │  │      │                        │
// │                      │        │  └────────────────────────────────┘  │      │                        │
// │                      └────►   │       │                      │       │    ◄─┘                        │
// │                               └───────┘                      └───────┘                               │
// │                                                                                                      │
// └──────────────────────────────────────────────────────────────────────────────────────────────────────┘
// // Motor A pins
int motorAEnablePin = 9;
int motorAInput1Pin = 8;
int motorAInput2Pin = 7;

// Motor B pins
int motorBEnablePin = 3;
int motorBInput1Pin = 4;
int motorBInput2Pin = 5;

bool isProgramEnded = false;

// Robot movement states
enum MovementState {READY, STOP, FORWARD, BACKWARD, TURN_RIGHT, TURN_LEFT};
MovementState currentState = READY;

// Speed constants
float MAX_SPEED = 255;       // Max PWM value for full speed
float THREE_QUARTER_SPEED = MAX_SPEED * 0.75;
float HALF_SPEED = MAX_SPEED * 0.5;
float QUARTER_SPEED = MAX_SPEED * 0.25;

float currentSpeed = MAX_SPEED;
float turnSpeed = HALF_SPEED;
float turnDuration = 1000;   // Duration for turns, in milliseconds

void setup() {
  // Set all motor control pins to outputs
  pinMode(motorAEnablePin, OUTPUT);
  pinMode(motorBEnablePin, OUTPUT);
  pinMode(motorAInput1Pin, OUTPUT);
  pinMode(motorAInput2Pin, OUTPUT);
  pinMode(motorBInput1Pin, OUTPUT);
  pinMode(motorBInput2Pin, OUTPUT);
  // Serial.println("Setup Complete!");
}

// Stop both motors
void stopMotors() {
  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, 0);
  analogWrite(motorBEnablePin, 0);
  currentState = STOP;
}

// Move both motors forward
void moveForward(int speed, unsigned long duration) {
  digitalWrite(motorAInput1Pin, HIGH);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, HIGH);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);
  delay(duration);
  stopMotors();
  currentState = FORWARD;
}

// Move both motors backward
void moveBackward(int speed, unsigned long duration) {
  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, HIGH);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, HIGH);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);
  delay(duration);
  stopMotors();
  currentState = BACKWARD;
}

// Turn left by moving motors in opposite directions
void turnLeft(int speed, unsigned long duration) {
  digitalWrite(motorAInput1Pin, HIGH);
  digitalWrite(motorAInput2Pin, LOW);
  digitalWrite(motorBInput1Pin, LOW);
  digitalWrite(motorBInput2Pin, HIGH);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);
  delay(duration);
  stopMotors();
  currentState = TURN_LEFT;
}

// Turn right by moving motors in opposite directions
void turnRight(int speed, unsigned long duration) {
  digitalWrite(motorAInput1Pin, LOW);
  digitalWrite(motorAInput2Pin, HIGH);
  digitalWrite(motorBInput1Pin, HIGH);
  digitalWrite(motorBInput2Pin, LOW);
  analogWrite(motorAEnablePin, speed);
  analogWrite(motorBEnablePin, speed);
  delay(duration);
  stopMotors();
  currentState = TURN_RIGHT;
}

// Convert PWM value to speed in meters per second
float pwmToMetersPerSecond(int pwmValue) {
  float maxMetersPerSecond = 0.70; // Max speed in m/s for 255 PWM
  return (pwmValue / 255.0) * maxMetersPerSecond;
}

// Determine turning duration based on speed mode
float getTurnDuration(float speed) {
  if (speed == MAX_SPEED) {
    return 375.0;
  } else if (speed == THREE_QUARTER_SPEED) {
    return 460.0;
  } else if (speed == HALF_SPEED) {
    return 825.0;
  } else {
    return 1000.0;
  }
}

// Calculate time required to travel a certain distance at a given speed\
// Distance in meters, Time in seconds 
float calculateTravelTime(float speed, float distance ) {
  speed = pwmToMetersPerSecond(speed);
  float timeInSeconds = distance / speed;
  return timeInSeconds * 1000; // Convert to milliseconds
}

void loop() {
  // Execute movement sequence with delays and turns
  // moveForward(currentSpeed, calculateTravelTime(0.25, currentSpeed));
  // delay(100);
  
  // turnLeft(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.25, currentSpeed));
  // delay(100);

  // turnRight(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.25, currentSpeed));
  // delay(100);

  // turnRight(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.25, currentSpeed));
  // delay(100);

  // turnLeft(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.60, currentSpeed));
  // delay(100);

  // turnLeft(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.25, currentSpeed));
  // delay(100);

  // turnRight(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.25, currentSpeed));
  // delay(100);

  // turnRight(currentSpeed, getTurnDuration(currentSpeed));
  // delay(100);

  // moveForward(currentSpeed, calculateTravelTime(0.35, currentSpeed));
  moveForward(currentSpeed, calculateTravelTime(currentSpeed, 1));
  stopMotors();

  if (currentState == STOP) {
    stopMotors();
    delay(99999999); // Effectively halts the program
  }
}
