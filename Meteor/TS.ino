#include "constants.h"

// ANY SERIAL COMMUNICATION HERE WILL GREATLY SLOW DOWN THE PROGRAM! USE ONLY IF NECESSARY

#define FORWARD 127.0
#define REVERSE -128.0
#define SLOW /2

#define RULE_COUNT 8

float (*rules[RULE_COUNT])(uint32_t*) = {weightSSS,weightSSB,weightSBS,weightSBB,weightBSS,weightBSB,weightBBS,weightBBB};

//Motor Settings
float LMS[RULE_COUNT] = {FORWARD, FORWARD, FORWARD SLOW, FORWARD, REVERSE, FORWARD, REVERSE, FORWARD};
float RMS[RULE_COUNT] = {REVERSE, REVERSE, FORWARD SLOW, REVERSE, FORWARD, REVERSE, FORWARD, FORWARD};

//Variable To Set Adherence (Small/Big)
float VTSAS[3] = {0};
float VTSAB[3] = {0};

float isSmallFS(uint32_t val){
  if (val > 20) return 0.0;
  return -(float)val/20.0 + 1.0;  
}

float isBigFS(uint32_t val){
  if (val < 10) return 0.0;
  if (val > 30) return 1.0;
  return ((float)val - 10.0)/20.0;
}

//Calculate variable to set adherences
void calculateVTSA(uint32_t* inputs){
  VTSAS[0] = isSmallFS(inputs[0]);
  VTSAS[1] = isSmallFS(inputs[1]);
  VTSAS[2] = isSmallFS(inputs[2]);

  VTSAB[0] = isBigFS(inputs[0]);
  VTSAB[1] = isBigFS(inputs[1]);
  VTSAB[2] = isBigFS(inputs[2]);
}

float weightSSS(uint32_t* inputs){
  return min(VTSAS[0], min(VTSAS[1], VTSAS[2]));
}
float weightSSB(uint32_t* inputs){
  return min(VTSAS[0], min(VTSAS[1], VTSAB[2]));
}
float weightSBS(uint32_t* inputs){
  return min(VTSAS[0], min(VTSAB[1], VTSAS[2]));
}
float weightSBB(uint32_t* inputs){
  return min(VTSAS[0], min(VTSAB[1], VTSAB[2]));
}
float weightBSS(uint32_t* inputs){
  return min(VTSAB[0], min(VTSAS[1], VTSAS[2]));
}
float weightBSB(uint32_t* inputs){
  return min(VTSAB[0], min(VTSAS[1], VTSAB[2]));
}
float weightBBS(uint32_t* inputs){
  return min(VTSAB[0], min(VTSAB[1], VTSAS[2]));
}
float weightBBB(uint32_t* inputs){
  return min(VTSAB[0], min(VTSAB[1], VTSAB[2]));
}



//Motor Regulator
MotorSettings getRegulatorValues(uint32_t* inputs){
  MotorSettings ms{.LM = 0, .RM = 0};
  float weight_sum = 0.0;
  calculateVTSA(inputs);
  for(int i = 0; i<RULE_COUNT; i++){
    float weight = (*rules[i])(inputs);
    weight_sum += weight;
    ms.LM += round(weight * LMS[i]);
    ms.RM += round(weight * RMS[i]);
  }

  ms.LM = round(weight_sum * (float)ms.LM);
  ms.RM = round(weight_sum * (float)ms.RM);

  return ms;
}
