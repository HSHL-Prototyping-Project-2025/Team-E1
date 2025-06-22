# Arduino Code

This folder contains all the Arduino source code that controls the autonomous vehicle.

## Structure

- `main.ino`: Core program for line-following and obstacle detection.
- `modules/`: Contains modular code for sensors and motor control.

## Features

- **Line Following:** Uses IR sensors to track a black line.
- **Obstacle Detection:** Uses ultrasonic sensors to stop or reroute when an object is detected within 15 cm.
- **Modular Design:** Each component has its own function for clarity and reusability.

## Uploading Instructions

1. Open `main.ino` in the Arduino IDE.
2. Connect your Arduino Uno via USB.
3. Select the correct board and port under *Tools > Board / Port*.
4. Click **Upload**.

---
