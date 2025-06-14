// === Pin Definitions ===
#define IR_LEFT 5
#define IR_RIGHT 2

#define TRIG_LEFT 7
#define ECHO_LEFT 6
#define TRIG_RIGHT 3
#define ECHO_RIGHT 4

#define ENA 13 // Left Motor Enable
#define IN1 12
#define IN2 11

#define ENB 8  // Right Motor Enable
#define IN3 10
#define IN4 9

// === State Definitions ===
enum State {
  INIT,
  FOLLOW_LINE,
  AVOID_OBSTACLE,
  LOST_LINE
};

State currentState = INIT;

// === Speed Settings ===
const int baseSpeed = 150;
const int turnSpeed = 100;
const int avoidSpeed = 120;

// === Setup ===
void setup() {
  Serial.begin(9600);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();
  delay(3000); // Wait 3s before starting
  currentState = FOLLOW_LINE;
}

// === Main Loop ===
void loop() {
  switch (currentState) {
    case INIT:
      stopMotors();
      break;

    case FOLLOW_LINE:
      followLineLogic();
      break;

    case AVOID_OBSTACLE:
      avoidObstacleLogic();
      break;

    case LOST_LINE:
      recoverLine();
      break;
  }
}

// === Sensor Helpers ===
bool isLineLeft() {
  return digitalRead(IR_LEFT) == HIGH; // HIGH = black = line
}

bool isLineRight() {
  return digitalRead(IR_RIGHT) == HIGH; // HIGH = black = line
}

long readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
  return duration * 0.034 / 2;
}

bool isObstacleDetected() {
  long distL = readUltrasonic(TRIG_LEFT, ECHO_LEFT);
  long distR = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);
  return (distL > 0 && distL < 20) || (distR > 0 && distR < 20);
}

// === Line Following Logic ===
void followLineLogic() {
  bool left = isLineLeft();
  bool right = isLineRight();

  Serial.print("IR L: ");
  Serial.print(left);
  Serial.print(" | IR R: ");
  Serial.println(right);

  if (isObstacleDetected()) {
    currentState = AVOID_OBSTACLE;
    return;
  }

  if (left && right) {
    moveForward(baseSpeed);
  } else if (left && !right) {
    turnLeft(turnSpeed);
  } else if (!left && right) {
    turnRight(turnSpeed);
  } else {
    stopMotors();
    currentState = LOST_LINE;
  }
}

// === Obstacle Avoidance ===
void avoidObstacleLogic() {
  Serial.println("Avoiding Obstacle");

  // Back up a bit
  moveBackward(avoidSpeed);
  delay(500);

  // Turn right to avoid
  turnRight(avoidSpeed);
  delay(600);

  // Go forward to bypass
  moveForward(avoidSpeed);
  delay(1000);

  // Turn back to left
  turnLeft(avoidSpeed);
  delay(600);

  // Resume forward
  moveForward(baseSpeed);
  delay(500);

  currentState = FOLLOW_LINE;
}

// === Line Recovery ===
void recoverLine() {
  Serial.println("Recovering Line");

  int spinSpeed = 100;
  bool found = false;

  // Spin right to search
  for (int i = 0; i < 15; i++) {
    spinRight(spinSpeed);
    if (isLineLeft() || isLineRight()) {
      found = true;
      break;
    }
    delay(100);
  }

  if (!found) {
    // Spin left to search
    for (int i = 0; i < 15; i++) {
      spinLeft(spinSpeed);
      if (isLineLeft() || isLineRight()) {
        found = true;
        break;
      }
      delay(100);
    }
  }

  if (found) {
    Serial.println("Line Recovered");
    stopMotors();
    delay(200);
    currentState = FOLLOW_LINE;
  } else {
    Serial.println("Still Lost");
    stopMotors();
  }
}

// === Movement Commands ===
void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void moveBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void spinLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void spinRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
