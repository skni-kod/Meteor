#include "constants.h"

#define FORWARD 127.0
#define REVERSE -128.0

#define RULE_COUNT 8

float (*rules[RULE_COUNT])(uint32_t*) = {weightSSS,weightSSB,weightSBS,weightSBB,weightBSS,weightBSB,weightBBS,weightBBB};

//Motor Settings
float LMS[RULE_COUNT] = {FORWARD, FORWARD, FORWARD, FORWARD, REVERSE, FORWARD, REVERSE, FORWARD};
float RMS[RULE_COUNT] = {REVERSE, REVERSE, FORWARD, REVERSE, FORWARD, REVERSE, FORWARD, FORWARD};

float isSmallFS(uint32_t val){
  if (val > 20) return 0.0;
  return -(float)val/20.0 + 1.0;  
}

float isBigFS(uint32_t val){
  if (val < 10) return 0.0;
  if (val > 30) return 1.0;
  return ((float)val - 10.0)/20.0;
}

float weightSSS(uint32_t* inputs){
  return min(isSmallFS(inputs[0]), min(isSmallFS(inputs[1]), isSmallFS(inputs[2])));
}
float weightSSB(uint32_t* inputs){
  return min(isSmallFS(inputs[0]), min(isSmallFS(inputs[1]), isBigFS(inputs[2])));
}
float weightSBS(uint32_t* inputs){
  return min(isSmallFS(inputs[0]), min(isBigFS(inputs[1]), isSmallFS(inputs[2])));
}
float weightSBB(uint32_t* inputs){
  return min(isSmallFS(inputs[0]), min(isBigFS(inputs[1]), isBigFS(inputs[2])));
}
float weightBSS(uint32_t* inputs){
  return min(isBigFS(inputs[0]), min(isSmallFS(inputs[1]), isSmallFS(inputs[2])));
}
float weightBSB(uint32_t* inputs){
  return min(isBigFS(inputs[0]), min(isSmallFS(inputs[1]), isBigFS(inputs[2])));
}
float weightBBS(uint32_t* inputs){
  return min(isBigFS(inputs[0]), min(isBigFS(inputs[1]), isSmallFS(inputs[2])));
}
float weightBBB(uint32_t* inputs){
  return min(isBigFS(inputs[0]), min(isBigFS(inputs[1]), isBigFS(inputs[2])));
}

//Motor Regulator
MotorSettings getRegulatorValues(uint32_t* inputs){
  MotorSettings ms{.LM = 0, .RM = 0};
  float weight_sum = 0.0;
  for(int i = 0; i<RULE_COUNT; i++){
    float weight = (*rules[i])(inputs);
    weight_sum += weight;
    ms.LM += round(weight * LMS[i]);
    ms.RM += round(weight * RMS[i]);
  }
  Serial.println(weight_sum);
  Serial.println(ms.LM);
  Serial.println(ms.RM);
  ms.LM = round(weight_sum * (float)ms.LM);
  ms.RM = round(weight_sum * (float)ms.RM);

  return ms;
}
