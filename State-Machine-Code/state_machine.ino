// State machine enumeration
enum RobotState {
  LINE_FOLLOWING,
  OBSTACLE_DETECTED, 
  EMERGENCY_STOP
};

RobotState currentState = LINE_FOLLOWING;

void loop() {
  switch (currentState) {
    case LINE_FOLLOWING:
      handleLineFollowing();
      break;
      
    case OBSTACLE_DETECTED:
      handleObstacleDetected();
      break;
      
    case EMERGENCY_STOP:
      handleEmergencyStop();
      break;
      
    default:
      currentState = EMERGENCY_STOP;
      break;
  }
  delay(15);
}

void handleLineFollowing() {
  if (isObstacle()) {
    Serial.println("Obstacle detected -> OBSTACLE_DETECTED state");
    currentState = OBSTACLE_DETECTED;
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
}

void handleObstacleDetected() {
  Serial.println("OBSTACLE_DETECTED state - Robot stopped");
  stopCar();
  
  if (!isObstacle()) {
    Serial.println("Path clear -> LINE_FOLLOWING state");
    currentState = LINE_FOLLOWING;
  }
  
  delay(500);
}

void handleEmergencyStop() {
  Serial.println("EMERGENCY_STOP state");
  stopCar();
  delay(1000);
}