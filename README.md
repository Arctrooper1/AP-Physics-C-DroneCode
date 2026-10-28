# Quadcopter

**Authors:** Andrew Lu, Priyansh Mehta  
**Date:** June 2025  

---

## Abstract

In this paper, we will detail our process for researching, designing, building, and coding a quadcopter from scratch without any prior experience in drone creation. First, the choice of materials and tools used will be outlined. Next, we will explore the various design choices made in the CAD models for the structure of our quadcopter as well as the wiring for all of the electrical components, including issues we faced and how we solved them. Then, we detail the building process, which includes soldering, drilling, and additional design considerations. Finally, we explore the usage of PID tuning and its implementation into Arduino software in order to stabilize the flight of our quadcopter, but we unfortunately ran into some issues when applying enough throttle to our quadcopter. To close, we will cover the physics concepts which govern the behavior of our quadcopter and reflect back upon our experiences.

---

## Contents

1. [Introduction](#1-introduction)
2. [Components](#2-components)
3. [Mechanical Design](#3-mechanical-design)
4. [Electrical Design](#4-electrical-design)
5. [Build](#5-build)
6. [Software](#6-software)
7. [Flight](#7-flight)
8. [Conclusion](#8-conclusion)
9. [References](#9-references)

---

## 1. Introduction

As our society advances further into the technological era, engineering and physics skills become a necessity. Recognizing this, as future engineers, we wanted to put our skills to the test by constructing a quadcopter. The overarching goal of this project was to explore and understand the various aspects of engineering and physics that govern the process of creating a functional quadcopter.

---

## 2. Components

Below are all of the materials used to make the quadcopter:

- 4x 1000KV Brushless Motor 13T
- 4x 40A RC Brushless Electric Speed Controller (ESC)
- 4x 2 sets of Propellers
- 1x Power Distribution Board (PDB)
- 1x FS-i6 6CH 2.4GHz RC Controller
- 1x FS-iA6B Receiver
- Male & Female Bullet Connectors
- Red & Black Heat Shrink Tubing
- 1x Roll of Electrical Tape
- 1x HC-05 Wireless Bluetooth module
- 1x Adafruit Perma-Proto Board
- 3x 3.3kΩ resistors
- 2x 10kΩ resistors
- 1x Arduino Uno
- 1x 3S LiPo Battery
- 1x LiPo Charger
- 1x Female XT-30 to Male XT-60 Adapter

Some notable tools used were wire strippers, a helping hand for soldering, solder wicks, a soldering iron, drills, a drill bit kit, a pin, and scissors.

---

## 3. Mechanical Design

The design process for the drone arms considered several key factors: propeller size, motor placement, connection mechanism, and the overall mass of the arm. Our arm design was inspired by two main sources: a Mini Quad FPV Drone CAD model on OnShape [1] and MikeyB's FPV Drone on Thingiverse [2]. The Mini Quad model inspired our implementation of a friction-fit connection with horizontal screw holes, maximizing torque transfer while minimizing print time. MikeyB's design influenced our use of a thin, lightweight structure with an X-shaped cutout.

### Initial Prototype & Iterations

Our initial prototype featured a friction-fit slot design but lacked firm connection support or access to screw holes. It was also too heavy, resulting in downward drooping caused by gravitational torque acting on the arm's center of mass.

The revised design improved structural stability, reduced weight, and included horizontal screw holes to enhance attachment. It also significantly reduced drooping. The arm is printed upside down to minimize overhangs during 3D printing. The central X-shape not only lightens the arm but also reduces print time and material usage.

Furthermore, the motor holes along with the horizontal screw hole used to connect the arm to the body were printed without a thread; however, threads were later created by threading a bolt into the hole and pulling it out.

### Body & Frame Structure

The shape of the center was custom-designed to accommodate the dimensions of key internal components: the Arduino UNO, power distribution board (PDB), flight control board, and arm connection points. The body consists of the undercarriage and the upper connection hub.

- **Undercarriage:** The undercarriage houses the Arduino UNO and flight control board, which together serve as the drone's brain. Six circular holes were included to accommodate bolts for secure mounting. The four elevated pins on the right and four pins on the left are designed to hold foam padding: one side supports the flight control board (right), and the other secures the Arduino (left). Additionally, an elliptical cutout was implemented in the center to reduce material usage and print time. The undercarriage implements swan-neck style brackets that create a strong and materially efficient container for the flight control board and the Arduino. Additionally, the brackets are used to secure the undercarriage to the upper connection hub, which joins the entire build together.
- **Upper Connection Hub:** The upper carriage is the most critical component of the drone's structure. It connects to the arms via T-shaped slots and screws, while also attaching to the undercarriage through screw holes located at the base of each T-slot. The four elevated circular holes in the center are used to mount the power distribution board (PDB). Additionally, the remaining four cutouts serve as cable routing channels from the undercarriage, improving overall cable management.

---

## 4. Electrical Design

Being relatively new to designing boards, we had to start with the fundamentals of circuit design, including power and communication.

The first challenge was to safely deliver power to all components, including the electronic speed controllers (ESCs), Arduino Uno, HC-05 Transmission Module, MPU6050, and Arduino MKR ENV Shield. 

Importantly, each ESC has 3 wires connected to its respective motor, which on its opposite side has 3 more outgoing wires: a thick red and black wire soldered to a positive and negative pad on the power distribution board (PDB), respectively. The final outgoing wire bundle has 3 female wire headers: black (ground), white (signal), and red (power). Upon researching the purpose of each wire, the red wire was not needed to connect to anything, as powering other components from the ESC BEC was unnecessary. A wire connects the 5V pad on the PDB to a rail on the prototyping board, from which the Uno and HC-05 can draw power when disconnected from the laptop.

### Voltage Divider Circuit

Initially, the Arduino Uno was powered through a direct USB connection to a laptop, which would then transfer 3.3V and 5V power and provide a GND connection with the other sensors. Upon further inspection, the HC-05 module required a power range of 3.6V to 6V (necessitating a 5V power supply), but the RX pin on the sensor required a 3.3V logic level. This conundrum led to using a voltage divider circuit to step down the incoming 5V signal to a safe 3.3V level for the HC-05 RX pin.

Using Kirchhoff's Voltage Law and Ohm's Law:

$$V_{in} = I R_1 + I R_2$$

$$V_{out} = I R_2$$

Isolating $I$ and solving for $V_{out}$:

$$I = \frac{V_{in}}{R_1 + R_2}$$

$$V_{out} = V_{in} \left(\frac{R_2}{R_1 + R_2}\right)$$

Using available resistors with $R_1 = 10{,}000\,\Omega$ and $R_2 = 2 \times 3{,}300\,\Omega = 6{,}600\,\Omega$, for $V_{in} = 5\text{V}$:

$$V_{out} = 5 \left(\frac{10}{16.6}\right) = 3.01\text{V}$$

This voltage level suffices as a logic-level supply for the RX pin.

### Communication & Pinout

To allow for communication between the ESCs, receiver, Uno, and sensors, we designated pins and used the I2C protocol:
- **ESCs:** Signal wires connected to digital pins **4, 5, 6, and 7** on the Uno.
- **Receiver:** The FS-iA6B contains 6 PWM channels; 4 channels connect to digital pins **9, 10, 11, and 12** on the Uno, allowing direct signal routing to the ESCs via code.
- **Form Factor & Assembly:** Half of a standard breadboard / Adafruit Perma-Proto board was used due to CAD compartment size constraints. #4-40 screws of varying lengths were used to assemble the frame securely.

---

## 5. Build

The process of acquiring all our materials and assembling them required a large amount of time. One particularly frustrating, time-consuming, somewhat hazardous, yet necessary portion of the build was soldering.

Soldering the bullet connectors to the PDB was by far the most difficult task of the physical construction of the quadcopter. When attempting to place the bullet connector onto a pad on the PDB, the previously liquefied solder would immediately harden as the soldering iron was pulled away. Eventually, we figured out that by carefully liquefying the solder while holding the bullet connector directly in place and then slowly withdrawing the soldering iron, a solid mechanical and electrical joint could be formed.

To test motor control safely without accidentally providing dual power supplies to the Uno (via USB and PDB simultaneously), we disconnected the 5V power wire from the Uno. This allowed the LiPo battery to exclusively power the PDB and motors, while the USB connection to the laptop supplied safe logic power, serial monitoring, and code uploading.

---

## 6. Software

Simultaneously powering the motors and connecting the Uno to a laptop enabled PID (Proportional-Integral-Derivative) tuning.

### Flight Control Program (`Listing 1`)

```cpp
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <MPU6050.h>
#include <Arduino_MKRENV.h>

// RC Receiver pins
uint8_t PIN_CH_ROLL     = 9;   // CH1 Roll
uint8_t PIN_CH_PITCH    = 10;  // CH2 Pitch
uint8_t PIN_CH_THROTTLE = 11;  // CH3 Throttle
uint8_t PIN_CH_YAW      = 12;  // CH4 Yaw

// ESC output pins
uint8_t PIN_ESC_FL = 4;  // front-left  (motor 1)
uint8_t PIN_ESC_FR = 5;  // front-right (motor 2)
uint8_t PIN_ESC_BL = 6;  // back-left   (motor 3)
uint8_t PIN_ESC_BR = 7;  // back-right  (motor 4)

Servo escFL, escFR, escBL, escBR;
MPU6050 mpu;
SoftwareSerial BTSerial(3, 2); // RX, TX

// Proportional
float Kp_rate_roll  = 0.90f;
float Kp_rate_pitch = 0.90f;
float Kp_rate_yaw   = 0.90f;

// Integral
float Ki_roll  = 0.002f;
float Ki_pitch = 0.002f;
float Ki_yaw   = 0.002f;

float int_err_roll  = 0;
float int_err_pitch = 0;
float int_err_yaw   = 0;

// Derivative
float Kd_roll  = 0.04f;
float Kd_pitch = 0.04f;
float Kd_yaw   = 0.04f;

float prev_err_roll  = 0;
float prev_err_pitch = 0;
float prev_err_yaw   = 0;

// Read PWM Channel
uint16_t readChannel(uint8_t pin) {
  return pulseIn(pin, HIGH, 30000);
}

void setup() {
  Serial.begin(115200);
  BTSerial.begin(9600);
  Wire.begin();
  
  mpu.initialize();
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
  if (!mpu.testConnection()) {
    Serial.println(F("MPU6050 connection failed"));
    while (1) delay(10);
  }
  
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV Shield");
    while (1) delay(10);
  }
  
  Serial.println("All sensors initialized.");
  BTSerial.println("BT OK: sensors initialized.");
  
  escFL.attach(PIN_ESC_FL);
  escFR.attach(PIN_ESC_FR);
  escBL.attach(PIN_ESC_BL);
  escBR.attach(PIN_ESC_BR);
  
  escFL.writeMicroseconds(1000);
  escFR.writeMicroseconds(1000);
  escBL.writeMicroseconds(1000);
  escBR.writeMicroseconds(1000);
  delay(2000);
  
  Serial.println("All ESCs armed.");
}

void loop() {
  static long last_time = micros();
  long now = micros();
  float dt = (now - last_time) / 1e6f; // dt in seconds
  last_time = now;
  
  if (dt <= 0 || dt > 0.1f) dt = 0.01f; // safety check
  
  uint16_t pwm_roll     = readChannel(PIN_CH_ROLL);
  uint16_t pwm_pitch    = readChannel(PIN_CH_PITCH);
  uint16_t pwm_throttle = readChannel(PIN_CH_THROTTLE);
  uint16_t pwm_yaw      = readChannel(PIN_CH_YAW);
  
  // Map sticks to desired angular rates (-100 to 100 deg/s)
  float desired_rate_roll  = map(pwm_roll, 1000, 2000, -100, 100);
  float desired_rate_pitch = map(pwm_pitch, 1000, 2000, -100, 100);
  float desired_rate_yaw   = map(pwm_yaw, 1000, 2000, -100, 100);
  
  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);
  
  float rate_roll  = (float)gy / 16.4f;
  float rate_pitch = (float)gx / 16.4f;
  float rate_yaw   = (float)gz / 16.4f;
  
  float err_roll  = desired_rate_roll - rate_roll;
  float err_pitch = desired_rate_pitch - rate_pitch;
  float err_yaw   = desired_rate_yaw - rate_yaw;
  
  // Integral
  int_err_roll  += err_roll * dt;
  int_err_pitch += err_pitch * dt;
  int_err_yaw   += err_yaw * dt;
  
  // Anti-windup safeguards
  int_err_roll  = constrain(int_err_roll, -50, 50);
  int_err_pitch = constrain(int_err_pitch, -50, 50);
  int_err_yaw   = constrain(int_err_yaw, -50, 50);
  
  // Derivative
  float d_err_roll  = (err_roll - prev_err_roll) / dt;
  float d_err_pitch = (err_pitch - prev_err_pitch) / dt;
  float d_err_yaw   = (err_yaw - prev_err_yaw) / dt;
  
  prev_err_roll  = err_roll;
  prev_err_pitch = err_pitch;
  prev_err_yaw   = err_yaw;
  
  // PID calculation
  float corr_roll  = Kp_rate_roll * err_roll + Ki_roll * int_err_roll + Kd_roll * d_err_roll;
  float corr_pitch = Kp_rate_pitch * err_pitch + Ki_pitch * int_err_pitch + Kd_pitch * d_err_pitch;
  float corr_yaw   = Kp_rate_yaw * err_yaw + Ki_yaw * int_err_yaw + Kd_yaw * d_err_yaw;
  
  Serial.print(err_roll, 1);
  Serial.print("\t");
  Serial.print(corr_roll, 1);
  Serial.print("\t");
  Serial.print(desired_rate_roll, 1);
  Serial.print("\t");
  Serial.print(err_pitch, 1);
  Serial.print("\t");
  Serial.print(corr_pitch, 1);
  Serial.print("\t");
  Serial.print(desired_rate_pitch, 1);
  Serial.print("\t");
  Serial.print(err_yaw, 1);
  Serial.print("\t");
  Serial.print(corr_yaw, 1);
  Serial.print("\t");
  Serial.println(desired_rate_yaw, 1);
  
  int motorFL = pwm_throttle + corr_pitch - corr_roll - corr_yaw;
  int motorFR = pwm_throttle + corr_pitch + corr_roll + corr_yaw;
  int motorBL = pwm_throttle - corr_pitch - corr_roll + corr_yaw;
  int motorBR = pwm_throttle - corr_pitch + corr_roll - corr_yaw;
  
  motorFL = constrain(motorFL, 1000, 2000);
  motorFR = constrain(motorFR, 1000, 2000);
  motorBL = constrain(motorBL, 1000, 2000);
  motorBR = constrain(motorBR, 1000, 2000);
  
  escFL.writeMicroseconds(motorFL);
  escFR.writeMicroseconds(motorFR);
  escBL.writeMicroseconds(motorBL);
  escBR.writeMicroseconds(motorBR);
  
  delay(10);
}
```

### PID Control Theory

PID tuning reduces the error between the input command from the RC controller and the detected roll, pitch, and yaw rates. Because the quadcopter never truly took flight, tuning was conducted while stationary on a test bench.

Mathematically, PID control is represented by:

$$u(t) = K_p e(t) + K_i \int e(t)\,dt + K_d \frac{de(t)}{dt}$$

where:
- $K_p, K_i, K_d$ are empirical gains
- $u(t)$ is the control output (corrected rate)
- $e(t)$ is the instantaneous tracking error

1. **Proportional (P) Control:** Compares input setpoint (e.g., desired roll rate) to gyroscopic angular rate from the MPU6050. The difference is multiplied by $K_p$ and added or subtracted to the individual motor throttles depending on motor position.
2. **Integral (I) Control:** Accumulates past steady-state error over time (`int_err_roll`), preventing drift or droop across axes. Clamping (anti-windup) constrains this accumulation within $[-50, 50]$.
3. **Derivative (D) Control:** Approximates the rate of error change $\frac{\Delta e}{\Delta t}$, providing damping against sudden disturbances and reducing overshoot.

Benchmark testing with the RC controller roll stick moved fully right and left verified that the quadcopter's corrective output closely tracked desired user inputs.

---

## 7. Flight

### Testing & Complications

During initial spin-up tests:
1. Three motors initially spun clockwise and one counterclockwise. Wire pairs between the ESC and motor were swapped to correct rotational direction.
2. Propeller nuts loosened during acceleration, throwing blades; this was resolved by tightening adapters securely.
3. Despite PID tuning, mass balancing, and software adjustments, the quadcopter flipped over immediately when lift-off throttle was applied.

Likely causes identified:
- Uneven power delivery across ESCs due to soldering resistance or voltage drops.
- Discrepancies between motor mixer mapping in code vs. physical motor layout.
- Ground effect and sensor vibration noise interfering with the gyro loop prior to liftoff.

### Aerodynamics & Governing Physics

Propellers convert rotational kinetic energy into thrust/lift by accelerating air downward:
- **Blade Pitch:** Angle of attack relative to rotation plane; dictates volume of air displaced per revolution.
- **Diameter:** Larger disc diameter increases swept area, generating higher lift efficiency.
- **Rotation Direction:** Two clockwise (CW) and two counterclockwise (CCW) propellers cancel net reactive torque.
- Propellers must be mounted with the leading edge higher than the trailing edge.

#### Lift Equation

Lift generated by an individual propeller:

$$L = \frac{1}{2} C_L \rho A v^2$$

where:
- $L$: Lift force $(\text{N})$
- $C_L$: Lift coefficient (profile / angle of attack dependent)
- $\rho$: Ambient air density $(\text{kg/m}^3)$
- $A$: Propeller disc swept area $(\text{m}^2)$
- $v$: Velocity of airflow through propeller disk $(\text{m/s})$

Total lift is summative:

$$\Sigma L = L_1 + L_2 + L_3 + L_4$$

#### Multi-Rotor Torque & Dynamics

By Newton's Third Law, each rotating blade applies reactive aerodynamic drag torque on the airframe. Having diagonal motor pairs spin CW and CCW achieves torque equilibrium:

$$\tau_{\text{roll}} \propto (L_2 + L_3) - (L_1 + L_4)$$

$$\tau_{\text{pitch}} \propto (L_1 + L_2) - (L_3 + L_4)$$

$$\tau_{\text{yaw}} = k \left(\sum \omega_{\text{CW}}^2 - \sum \omega_{\text{CCW}}^2\right)$$

where $L_i$ denotes thrust from motor $i$, $\omega_i$ the angular speed, and $k$ the motor torque constant.

---

## 8. Conclusion

Key takeaways and lessons learned:
1. **Holistic System Understanding:** Gaining full theoretical and architectural clarity prior to fabrication saves substantial bench time.
2. **Quality of Hardware Interconnects:** Soldering heavy-gauge power lines and bullet connectors requires high thermal mass and proper technique. Connector mismatches (XT30 vs. XT60, logic levels) must be cataloged in advance.
3. **Timeline Buffers:** Prototyping physical hardware and real-time control software requires generous margins for debugging unexpected dynamic behaviors.

Although sustained hovering was not achieved, the project provided comprehensive foundational experience across mechanical CAD, power electronics, embedded C++, and flight dynamics.

---

## 9. References

1. Onshape. *Mini Quad FPV Drone CAD model*, 2025. [https://cad.onshape.com/documents/281222d51728d347853c14d0/w/1720634e578f5f454952f021/e/8387e60dd185aa2e25e73537](https://cad.onshape.com/documents/281222d51728d347853c14d0/w/1720634e578f5f454952f021/e/8387e60dd185aa2e25e73537). Accessed 16 June 2025.
2. Thingiverse.com. *"3D Printed FPV Quadcopter the Crossfire by MikeyB."* Thingiverse, 2025. [https://www.thingiverse.com/thing:32281](https://www.thingiverse.com/thing:32281). Accessed 16 June 2025.
