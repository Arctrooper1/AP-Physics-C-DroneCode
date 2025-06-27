#include <Wire.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <MPU6050.h>
#include <Arduino_MKRENV.h>

// RC Receiver pins
uint8_t PIN_CH_ROLL = 9;       // CH1 Roll
uint8_t PIN_CH_PITCH = 10;     // CH2 Pitch
uint8_t PIN_CH_THROTTLE = 11;  // CH3 Throttle
uint8_t PIN_CH_YAW = 12;       // CH4 Yaw

// ESC output pins
uint8_t PIN_ESC_FL = 4;  // front-left  (motor 1)
uint8_t PIN_ESC_FR = 5;  // front-right (motor 2)
uint8_t PIN_ESC_BL = 6;  // back-left   (motor 3)
uint8_t PIN_ESC_BR = 7;  // back-right  (motor 4)

Servo escFL, escFR, escBL, escBR;
MPU6050 mpu;
SoftwareSerial BTSerial(3, 2);  // RX, TX

// Proportional
float Kp_rate_roll = 0.90f;
float Kp_rate_pitch = 0.90f;
float Kp_rate_yaw = 0.90f;

// Integrate
float Ki_roll = 0.002f;
float Ki_pitch = 0.002f;
float Ki_yaw = 0.002f;

float int_err_roll = 0;
float int_err_pitch = 0;
float int_err_yaw = 0;

// Derivative
float Kd_roll = 0.04f;
float Kd_pitch = 0.04f;
float Kd_yaw = 0.04f;

float prev_err_roll = 0;
float prev_err_pitch = 0;
float prev_err_yaw = 0;



// read
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
  static long last_time = micros();  // isn't assigned to micros() after first use because it is static
  long now = micros();
  float dt = (now - last_time) / 1e6f;  // dt in seconds, convert from microseconds to seconds with 1e6
  last_time = now;                      // reassign last_time

  if (dt <= 0 || dt > 0.1f) dt = 0.01f;  // safety

  uint16_t pwm_roll = readChannel(PIN_CH_ROLL);
  uint16_t pwm_pitch = readChannel(PIN_CH_PITCH);
  uint16_t pwm_throttle = readChannel(PIN_CH_THROTTLE);
  uint16_t pwm_yaw = readChannel(PIN_CH_YAW);

  // Map sticks to 1000-2000
  float desired_rate_roll = map(pwm_roll, 1000, 2000, -100, 100);
  float desired_rate_pitch = map(pwm_pitch, 1000, 2000, -100, 100);
  float desired_rate_yaw = map(pwm_yaw, 1000, 2000, -100, 100);

  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);
  float rate_roll = (float)gy / 16.4f;
  float rate_pitch = (float)gx / 16.4f;
  float rate_yaw = (float)gz / 16.4f;

  float err_roll = desired_rate_roll - rate_roll;
  float err_pitch = desired_rate_pitch - rate_pitch;
  float err_yaw = desired_rate_yaw - rate_yaw;

  // Integrate
  int_err_roll += err_roll * dt;
  int_err_pitch += err_pitch * dt;
  int_err_yaw += err_yaw * dt;

  // Safeguard
  int_err_roll = constrain(int_err_roll, -50, 50);
  int_err_pitch = constrain(int_err_pitch, -50, 50);
  int_err_yaw = constrain(int_err_yaw, -50, 50);

  // Derivative
  float d_err_roll = (err_roll - prev_err_roll) / dt;
  float d_err_pitch = (err_pitch - prev_err_pitch) / dt;
  float d_err_yaw = (err_yaw - prev_err_yaw) / dt;

  prev_err_roll = err_roll;
  prev_err_pitch = err_pitch;
  prev_err_yaw = err_yaw;

  // PID
  float corr_roll = Kp_rate_roll * err_roll + Ki_roll * int_err_roll + Kd_roll * d_err_roll;
  float corr_pitch = Kp_rate_pitch * err_pitch + Ki_pitch * int_err_pitch + Kd_pitch * d_err_pitch;
  float corr_yaw = Kp_rate_yaw * err_yaw + Ki_yaw * int_err_yaw + Kd_yaw * d_err_yaw;

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

  int motorFL = pwm_throttle + corr_pitch + corr_roll - corr_yaw;
  int motorFR = pwm_throttle + corr_pitch - corr_roll + corr_yaw;
  int motorBL = pwm_throttle - corr_pitch + corr_roll + corr_yaw;
  int motorBR = pwm_throttle - corr_pitch - corr_roll - corr_yaw;

  motorFL = constrain(motorFL, 1000, 2000);
  motorFR = constrain(motorFR, 1000, 2000);
  motorBL = constrain(motorBL, 1000, 2000);
  motorBR = constrain(motorBR, 1000, 2000);

  escFL.writeMicroseconds(motorFL);
  escFR.writeMicroseconds(motorFR);
  escBL.writeMicroseconds(motorBL);
  escBR.writeMicroseconds(motorBR);

  delay(10);