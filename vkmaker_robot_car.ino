#include <Servo.h>

//HC-SR04 (5V) - ultrasonic sensor to measure distance
const int SONAR_TRIG_PIN = 12; 
const int SONAR_ECHO_PIN = 2;
const int SONAR_INTERRUPT = 0; //Note: pin 2
volatile int distance = -1;
volatile long distCalcStartTime = 0;

//L298N Dual H-Bridge Motor Controller
const int MOTOR_RIGHT = 0;
const int MOTOR_RIGHT_FORWARD_PIN = 7; //connect to IN3
const int MOTOR_RIGHT_BACKWARD_PIN = 8; //connect to IN4
const int MOTOR_RIGHT_SPEED_PWM_PIN = 9; //connect to ENB
const int MOTOR_LEFT = 1;
const int MOTOR_LEFT_FORWARD_PIN = 5; //connect to IN1
const int MOTOR_LEFT_BACKWARD_PIN = 4; //connect to IN2
const int MOTOR_LEFT_SPEED_PWM_PIN = 3; //connect to ENA
const int MOTOR_MOVE_FORWARD = 1;
const int MOTOR_MOVE_BACKWARD = 0;

//SG90 9G Mini Servo (5V)
const int SERVO_PIN = 6;
Servo servo;

void setup()
{
   //HC-SR04
   pinMode(SONAR_TRIG_PIN, OUTPUT);
   pinMode(SONAR_ECHO_PIN, INPUT);
   //interrupt when SONAR_ECHO_PIN changes; used instead of pulseIn(SONAR_ECHO_PIN, HIGH);
   attachInterrupt(SONAR_INTERRUPT, calculateDistance, CHANGE);
   
   //L298N
   pinMode(MOTOR_LEFT_FORWARD_PIN, OUTPUT);
   pinMode(MOTOR_LEFT_BACKWARD_PIN, OUTPUT);
   pinMode(MOTOR_LEFT_SPEED_PWM_PIN, OUTPUT);
   pinMode(MOTOR_RIGHT_FORWARD_PIN, OUTPUT);
   pinMode(MOTOR_RIGHT_BACKWARD_PIN, OUTPUT);
   pinMode(MOTOR_RIGHT_SPEED_PWM_PIN, OUTPUT);   
  
   //SG90
   servo.attach(SERVO_PIN);
   
   Serial.begin(9600);
}

void startDistanceCalculation()
{
   //Clear the SONAR_TRIG_PIN
   digitalWrite(SONAR_TRIG_PIN, LOW);
   delayMicroseconds(2);

   //Set the SONAR_TRIG_PIN on HIGH state for 10 micro seconds
   digitalWrite(SONAR_TRIG_PIN, HIGH);
   delayMicroseconds(10);
   digitalWrite(SONAR_TRIG_PIN, LOW);
}

void calculateDistance()
{
  switch(digitalRead(SONAR_ECHO_PIN))
  {
    //Start of pulse
    case HIGH:
      distCalcStartTime = micros();
      break;

    //Pulse done; calculate distance in cm
    case LOW:
      distance = (micros() - distCalcStartTime)*0.034/2;
      distCalcStartTime = 0;
      break;
  }
}
void moveMotor(int motorNumber, int motorDirection, int motorSpeed)
{
   int motorForwardPin;
   int motorBackwardPin;
   int motorSpeedPin;
   
   if(motorNumber == MOTOR_RIGHT) {
      motorForwardPin = MOTOR_RIGHT_FORWARD_PIN;
      motorBackwardPin = MOTOR_RIGHT_BACKWARD_PIN;
      motorSpeedPin = MOTOR_RIGHT_SPEED_PWM_PIN;
      Serial.print("RIGHT, ");
   }
   else {
      motorForwardPin = MOTOR_LEFT_FORWARD_PIN;
      motorBackwardPin = MOTOR_LEFT_BACKWARD_PIN;
      motorSpeedPin = MOTOR_LEFT_SPEED_PWM_PIN;
      Serial.print("LEFT, ");
   }
   
   if(motorDirection == MOTOR_MOVE_FORWARD) {
      digitalWrite(motorForwardPin, HIGH);
      digitalWrite(motorBackwardPin, LOW);
      Serial.print("FORWARD, ");
   }
   else {
      digitalWrite(motorForwardPin, LOW);
      digitalWrite(motorBackwardPin, HIGH);
      Serial.print("BACKWARD, ");
   }
   
   Serial.print(motorForwardPin);
   Serial.print(", ");
   Serial.print(motorBackwardPin);
   Serial.print(", ");
   Serial.print(motorSpeedPin);
   Serial.print(", ");
   Serial.println(motorSpeed);
      
   analogWrite(motorSpeedPin, motorSpeed);
}

void moveServo(int angle)
{
   servo.write(angle); 
}

void loop()
{
   //Move servo 60 to 120 degress
   int angle = 60+(sin(millis()/250.0)+1) * 30.0;
   moveServo(angle);
   
   //Calculate distance
   Serial.print("Distance: ");
   Serial.println(distance);
   if(!distCalcStartTime) {
      startDistanceCalculation();
   }
   
   if(distance > 20) {
      //Move forward
      moveMotor(MOTOR_LEFT, MOTOR_MOVE_FORWARD, 255);
      moveMotor(MOTOR_RIGHT, MOTOR_MOVE_FORWARD, 255);
   }
   else {
      //Move backwards
      moveMotor(MOTOR_LEFT, MOTOR_MOVE_BACKWARD, 255);
      moveMotor(MOTOR_RIGHT, MOTOR_MOVE_BACKWARD, 255);
      delay(500);
      //Turn to the left
      moveMotor(MOTOR_LEFT, MOTOR_MOVE_BACKWARD, 255);
      moveMotor(MOTOR_RIGHT, MOTOR_MOVE_BACKWARD, 0);
      delay(500);
   }
}
