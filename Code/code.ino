// === Pin Definitions ===
// IR Sensors
const int IR_RIGHT = 2;
const int IR_LEFT = 5;

// Ultrasonic Right
const int US_TRIG_R = 3;
const int US_ECHO_R = 4;

// Ultrasonic Left
const int US_ECHO_L = 6;
const int US_TRIG_L = 7;

// Motor Driver - Right
const int ENB = 8;
const int IN4 = 9;
const int IN3 = 10;

// Motor Driver - Left
const int IN2 = 11;
const int IN1 = 12;
const int ENA = 13;

// === State Machine States ===
enum State {
  INIT,
  FOLLOW_LINE,
  AVOID_OBSTACLE,
  LOST_LINE,
};
State currentState = INIT;

// === Motor Control Parameters ===
int baseSpeed = 140;     // PWM speed for straight
int turnSpeed = 90;      // PWM speed for turning
int reverseSpeed = 100;  // for recovery
int obstacleThreshold = 20; // cm

// === Line Tracking ===
bool lastSeenLeft = false;
bool lastSeenRight = false;

// === Setup ===
void setup() {
  Serial.begin(9600);

  // IR
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_LEFT, INPUT);

  // Ultrasonic
  pinMode(US_TRIG_R, OUTPUT);
  pinMode(US_ECHO_R, INPUT);
  pinMode(US_TRIG_L, OUTPUT);
  pinMode(US_ECHO_L, INPUT);

  // Motors
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();
}

// === Main Loop ===
void loop() {
  switch (currentState) {
    case INIT:
      delay(3000); // wait and observe
      currentState = FOLLOW_LINE;
      break;

    case FOLLOW_LINE:
      followLineLogic();
      break;

    case AVOID_OBSTACLE:
      avoidObstacle();
      currentState = FOLLOW_LINE;
      break;

    case LOST_LINE:
      recoverLine();
      break;
  }
}

// === Sensor Functions ===
bool isLineLeft() {
  return digitalRead(IR_LEFT) == LOW; // Black line
}

bool isLineRight() {
  return digitalRead(IR_RIGHT) == LOW; // Black line
}

long readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2; // in cm
}

// === Motor Control Functions ===
void drive(int leftSpeed, int rightSpeed) {
  // Left motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, leftSpeed);

  // Right motor
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, rightSpeed);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, turnSpeed);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, turnSpeed);
}

void reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, reverseSpeed);
  analogWrite(ENB, reverseSpeed);
}

// === Line Following Logic ===
void followLineLogic() {
  bool left = isLineLeft();
  bool right = isLineRight();
  long frontLeft = readUltrasonic(US_TRIG_L, US_ECHO_L);
  long frontRight = readUltrasonic(US_TRIG_R, US_ECHO_R);

  if (frontLeft < obstacleThreshold || frontRight < obstacleThreshold) {
    stopMotors();
    currentState = AVOID_OBSTACLE;
    return;
  }

  if (left && right) {
    drive(baseSpeed, baseSpeed);  // Go straight
    lastSeenLeft = true;
    lastSeenRight = true;
  } else if (left) {
    drive(turnSpeed, baseSpeed);  // Veer left
    lastSeenLeft = true;
    lastSeenRight = false;
  } else if (right) {
    drive(baseSpeed, turnSpeed);  // Veer right
    lastSeenLeft = false;
    lastSeenRight = true;
  } else {
    stopMotors();
    currentState = LOST_LINE;
  }
}

// === Obstacle Avoidance ===
void avoidObstacle() {
  stopMotors();
  delay(200);
  reverse();
  delay(500);
  stopMotors();

  // Turn away from obstacle
  if (readUltrasonic(US_TRIG_R, US_ECHO_R) < obstacleThreshold) {
    turnLeft();
  } else {
    turnRight();
  }
  delay(600);

  // Drive forward to bypass
  drive(baseSpeed, baseSpeed);
  delay(800);

  // Try to reacquire line
  currentState = LOST_LINE;
}

// === Line Recovery ===
void recoverLine() {
  stopMotors();
  delay(100);

  if (lastSeenLeft) {
    turnLeft();
    delay(400);
  } else if (lastSeenRight) {
    turnRight();
    delay(400);
  } else {
    turnLeft();
    delay(400);
    turnRight();
    delay(800);
    turnLeft();
    delay(400);
  }

  if (isLineLeft() || isLineRight()) {
    currentState = FOLLOW_LINE;
  }
}
