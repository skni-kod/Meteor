#ifndef CONST_FILE
#define CONST_FILE
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

#define SPEED_LEFT 6
#define SPEED_RIGHT 5

#define TRIGGER 4

//Must be interrupt pin
#define ECHO 2

#define SENSOR_COUNT 4
#define BUFFER_SIZE 5
#define DISCARD_OFFSET 200
#define MAX_FAILURES 2

//Sensors that will be taken into account by fuzzy regulator
//Changing this or position of any sensor also requires change of regulator
#define REGULATOR_SENSORS 3

#endif
