#include "constants.h"
struct MotorSettings{
  int8_t LM;
  int8_t RM;  
};

struct SensorBuffer{
  uint32_t data[BUFFER_SIZE] = {0};
  uint8_t index = 0;
  uint8_t failures = 0;

  uint32_t getRecent(){
    return data[index];  
  }

  void pushChecked(uint32_t val){
    for(int i=0; i<BUFFER_SIZE; i++){
      if (data[i] != 0 && (data[i] - val > DISCARD_OFFSET || val - data[i] > DISCARD_OFFSET )){
        failures++;
        if (failures <= MAX_FAILURES) return;
      }
    }
    data[index++] = val;
    index %= BUFFER_SIZE;
    failures = 0;
  }
};



volatile short currentSensor = 0;
volatile bool echoInProgress = false;
volatile uint32_t sensors[SENSOR_COUNT];

SensorBuffer buffers[SENSOR_COUNT];

volatile uint32_t e_time;

void triggerSensors(){
  currentSensor = 0;
  digitalWrite(TRIGGER, HIGH);
  delay(100);
  digitalWrite(TRIGGER,LOW);
}

void setReverse(){
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  Serial.println("REVERSE");
}
void setForward(){
  
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  
  Serial.println("FORWARD");
}
void setLeft(){
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  
  Serial.println("LEFT");
}
void setRight(){
  
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  Serial.println("RIGHT");
}

void setMotorSpeed(MotorSettings* set){
  uint8_t lm = (uint8_t)abs(set->LM - 1) * 2;
  uint8_t rm = (uint8_t)abs(set->RM - 1) * 2;

  Serial.println("MOTOR SETTINGS");
  Serial.println(lm);
  Serial.println(rm);

  analogWrite(SPEED_LEFT, 255);
  analogWrite(SPEED_RIGHT, 255);
}

unsigned long m_b = 0, m_a = 0;

void setup() {

  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(SPEED_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  attachInterrupt(digitalPinToInterrupt(ECHO), echoInterrupt, CHANGE);
  m_b = millis();

}

unsigned long distances[SENSOR_COUNT] = {0};

void loop() {
  // put your main code here, to run repeatedly:

  m_a = millis();
  if (m_a - m_b > 10){
    triggerSensors();
    m_b = millis();
  }
    /*Serial.print("Dystans: ");
    Serial.print(distances[i]);
    Serial.print("cm dla sensora: ");
    Serial.println(i);
    */
  //delay(100);
  for(int i=0; i<SENSOR_COUNT; i++){
    buffers[i].pushChecked(sensors[i]);
    
    distances[i] = round(buffers[i].getRecent() * 0.0171);
  }


  Serial.print(sensors[0]);
  Serial.print(" : ");
  Serial.println(distances[0]);
  Serial.print(sensors[1]);
  Serial.print(" : ");
  Serial.println(distances[1]);
  Serial.print(sensors[2]);
  Serial.print(" : ");
  Serial.println(distances[2]);
  Serial.print(sensors[3]);
  Serial.print(" : ");
  Serial.println(distances[3]);

  MotorSettings ms;
  ms = getRegulatorValues(distances);

  Serial.println("------------");
  Serial.print("Left motor setting: ");
  Serial.println(ms.LM);
  Serial.print("Right motor setting: ");
  Serial.println(ms.RM);
  Serial.println("------------");

  if (ms.LM > 0 && ms.RM > 0) setForward();
  else if (ms.LM < 0 && ms.RM > 0) setLeft();
  else if (ms.LM > 0 && ms.RM < 0) setRight();
  else setReverse();

  //setMotorSpeed(&ms);

  //delay(1000);
}

void echoInterrupt(){
  volatile uint8_t pin;
  volatile unsigned long c_time = micros();
  
  pin = PIND >> ECHO & 1;
  //pin = digitalRead(ECHO);
  
  if (pin) {
    e_time = c_time;
    echoInProgress = true;
  }
  else if (echoInProgress){
    sensors[currentSensor] = c_time - e_time;
    currentSensor += 1;
    currentSensor %= SENSOR_COUNT;
    echoInProgress = false;
  }
}
