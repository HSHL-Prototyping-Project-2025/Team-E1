// --- Pin Definitions ---
// IR Sensors
const int leftIR = 5;
const int rightIR = 2;

// Ultrasonic Sensors (Obstacle Detection)
const int trigRight = 3;
const int echoRight = 4;
const int echoLeft = 6;
const int trigLeft = 7;

// Motor Driver Pins
const int ENB = 8;   // Right Motor Speed Control
const int IN4 = 9;   // Right Motor Direction
const int IN3 = 10;  // Right Motor Direction
const int IN2 = 11;  // Left Motor Direction
const int IN1 = 12;  // Left Motor Direction
const int ENA = 13;  // Left Motor Speed Control

// --- PID Constants (tune for best performance) ---
float Kp = 10.0; // Proportional gain
float Ki = 0.0;  // Integral gain
float Kd = 10.0; // Derivative gain

// --- PID Variables ---
float error = 0, previousError = 0, integral = 0;

// --- Speed Settings ---
const int baseSpeed = 80;
const int maxSpeed = 100;
const int minSpeed = 70;

// --- Obstacle Threshold ---
const int obstacleDistance = 15; // in centimeters

// --- Recovery Direction Memory ---
String lastSeen = "center"; // Tracks which side last saw the line

// --- Setup Function ---
void setup() {
  // Set pin modes for IR sensors
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  // Set pin modes for ultrasonic sensors
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  // Set pin modes for motor control
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.begin(9600); // For debugging
}

// --- Measure Distance using Ultrasonic Sensor ---
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

// --- Check if Obstacle is Detected ---
bool isObstacle() {
  long leftDist = getDistance(trigLeft, echoLeft);
  long rightDist = getDistance(trigRight, echoRight);
  return (leftDist < obstacleDistance || rightDist < obstacleDistance);
}

// --- Motor Control Function ---
void setMotor(int leftSpeed, int rightSpeed) {
  // Clamp speeds to max/min range
  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  // Left Motor Control
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, leftSpeed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -leftSpeed);
  }

  // Right Motor Control
  if (rightSpeed >= 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, rightSpeed);
  } else {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, -rightSpeed);
  }
}

// --- Stop Both Motors ---
void stopCar() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// --- Recovery Behavior When Line is Lost ---
void recoverLine() {
  Serial.println("Recovering line...");
  if (lastSeen == "left") {
    setMotor(-minSpeed, minSpeed);  // Spin left
  } else if (lastSeen == "right") {
    setMotor(minSpeed, -minSpeed);  // Spin right
  } else {
    setMotor(-minSpeed, minSpeed);  // Default to left spin
  }
  delay(150);  // Small scanning delay
}

// --- Main Loop ---
void loop() {
  // Stop the car if obstacle is detected
  if (isObstacle()) {
    stopCar();
    return;
  }

  // Read IR sensor values (1 = black line, 0 = white surface)
  int leftValue = digitalRead(leftIR);
  int rightValue = digitalRead(rightIR);

  // Determine position relative to the line and set error accordingly
  if (leftValue == 1 && rightValue == 1) {
    error = 0; // On track
    lastSeen = "center";
  } else if (leftValue == 1 && rightValue == 0) {
    error = -1; // Veering right
    lastSeen = "left";
  } else if (leftValue == 0 && rightValue == 1) {
    error = 1; // Veering left
    lastSeen = "right";
  } else {
    // Line completely lost, try to recover
    recoverLine();
    return;
  }

  // --- PID Control ---
  integral += error;
  float derivative = error - previousError;
  float correction = Kp * error + Ki * integral + Kd * derivative;
  previousError = error;

  // Adjust motor speeds using correction value
  int leftSpeed = baseSpeed - correction;
  int rightSpeed = baseSpeed + correction;

  // Apply motor speeds
  setMotor(leftSpeed, rightSpeed);

  delay(20); // Small delay for smoother motion
}
