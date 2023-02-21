#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

#define TRIGGER 4

//Must be interrupt pin
#define ECHO 2

#define SENSOR_COUNT 2
#define BUFFER_SIZE 10
#define DISCARD_OFFSET 200
#define MAX_FAILURES 4

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
  digitalWrite(TRIGGER, HIGH);
  delay(100);
  digitalWrite(TRIGGER,LOW);
}

void setReverse(){
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  Serial.println("FORWARD");
}
void setForward(){
  
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  
  Serial.println("REVERSE");
}
void setLeft(){
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  
  Serial.println("RIGHT");
}
void setRight(){
  
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  Serial.println("LEFT");
}

unsigned long m_b = 0, m_a = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  attachInterrupt(digitalPinToInterrupt(ECHO), echoInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), test, CHANGE);
  m_b = millis();

}

  unsigned long distances[SENSOR_COUNT] = {0};

void test(){

  Serial.println("VINT");
}
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
  if (distances[0] < 10) setRight();
  else if (distances[1] < 10) setLeft();
  else setForward();

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
