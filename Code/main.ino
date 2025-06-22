// Pin assignments
const int leftIR = 5;
const int rightIR = 2;

const int trigRight = 3;
const int echoRight = 4;
const int echoLeft = 6;
const int trigLeft = 7;

const int ENB = 8;
const int IN4 = 9;
const int IN3 = 10;

const int IN2 = 11;
const int IN1 = 12;
const int ENA = 13;

// Speeds
const int forwardSpeed = 100;
const int reverseSpeed = 60;
const int turnMainSpeed = 90;
const int turnSlowSpeed = 40;

const int obstacleDistance = 15;

void setup() {
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.begin(9600);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

bool isObstacle() {
  long leftDist = getDistance(trigLeft, echoLeft);
  long rightDist = getDistance(trigRight, echoRight);
  return (leftDist < obstacleDistance || rightDist < obstacleDistance);
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, forwardSpeed);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, forwardSpeed);
}

void reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, reverseSpeed);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, reverseSpeed);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void turnRight() {
  // Left wheel fast, right wheel slow
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, turnMainSpeed);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, turnSlowSpeed);
}

void turnLeft() {
  // Right wheel fast, left wheel slow
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, turnSlowSpeed);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, turnMainSpeed);
}

void loop() {
  if (isObstacle()) {
    stopCar();
    return;
  }

  int leftValue = digitalRead(leftIR);
  int rightValue = digitalRead(rightIR);

  if (leftValue == 1 && rightValue == 1) {
    moveForward();
  } 
  else if (leftValue == 0 && rightValue == 1) {
    turnRight();
  } 
  else if (leftValue == 1 && rightValue == 0) {
    turnLeft();
  } 
  else {
    reverse();
  }

  delay(15);
}
