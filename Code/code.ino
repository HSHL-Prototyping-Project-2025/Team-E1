// Corrected motor control pins (based on wiring image)
const int rightMotorPin1 = 8;  // IN3
const int rightMotorPin2 = 9;  // IN4
const int leftMotorPin1  = 10; // IN1
const int leftMotorPin2  = 11; // IN2

// Ultrasonic Sensor 1
const int trigPin1 = 4;
const int echoPin1 = 3;

// Ultrasonic Sensor 2
const int trigPin2 = 5;
const int echoPin2 = 6;

// IR sensors
const int irSensorPin = 12;
const int irSensorPin2 = 13;

long duration1, distance1;
long duration2, distance2;

void setup() {
  // Motor pins
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // Ultrasonic sensors
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // IR sensors
  pinMode(irSensorPin, INPUT);
  pinMode(irSensorPin2, INPUT);

  Serial.begin(9600);
}

void loop() {
  int lineState = digitalRead(irSensorPin);
  int lineState1 = digitalRead(irSensorPin2);

  // Read Ultrasonic Sensor 1
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;

  // Read Ultrasonic Sensor 2
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;

  Serial.print("Dist1: "); Serial.print(distance1);
  Serial.print(" cm | Dist2: "); Serial.print(distance2);
  Serial.print(" cm | IR: "); Serial.print(lineState);
  Serial.print(", "); Serial.println(lineState1);

  // Obstacle Avoidance
  if (distance1 < 20 || distance2 < 20) {
    stopMotors();
  } else {
    if (lineState == LOW || lineState1 == LOW) {
      moveForward();
    } else {
      searchForLine();
    }
  }
}

void moveForward() {
  // REVERSED: Now both motors spin forward correctly
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
}

void stopMotors() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
}

void searchForLine() {
  // REVERSED: Now rotates in the opposite direction to search
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  delay(500);
}
