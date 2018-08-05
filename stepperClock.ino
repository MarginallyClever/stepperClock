// clock driven by stepper motor.
// can wind to specific time, count on demand, etc.
#include <Stepper.h>

#define MOTOR_STEPS        (32.0)
#define GEAR_RATIO         (64.0)
#define STEPS_PER_TURN     (GEAR_RATIO * MOTOR_STEPS)
#define SECONDS_PER_MINUTE (60.0)

#define STEPS_PER_SECOND   (STEPS_PER_TURN / SECONDS_PER_MINUTE)

// Motor pin definitions
#define MOTORPIN1  5     // IN1 on the ULN2003 driver 1
#define MOTORPIN2  4     // IN2 on the ULN2003 driver 1
#define MOTORPIN3  3     // IN3 on the ULN2003 driver 1
#define MOTORPIN4  2     // IN4 on the ULN2003 driver 1

#define LIMIT_SWITCH 7
#define BUTTON1 13
#define BUTTON2 12
#define BUTTON3 11
#define BUTTON4 10
#define BUTTON5  9
#define BUTTON6  8

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
Stepper stepper1(STEPS_PER_TURN, MOTORPIN1, MOTORPIN3, MOTORPIN2, MOTORPIN4);

float handPos = 0;

void setup() {
  Serial.begin(57600);

  stepper1.setSpeed(7);  // RPM
  
  pinMode(LIMIT_SWITCH,INPUT);  //digitalWrite(LIMIT_SWITCH,HIGH);

  pinMode(BUTTON1,INPUT);  //digitalWrite(BUTTON1,HIGH);
  pinMode(BUTTON2,INPUT);  //digitalWrite(BUTTON2,HIGH);
  pinMode(BUTTON3,INPUT);  //digitalWrite(BUTTON3,HIGH);
  pinMode(BUTTON4,INPUT);  //digitalWrite(BUTTON4,HIGH);
  pinMode(BUTTON5,INPUT);  //digitalWrite(BUTTON5,HIGH);
  pinMode(BUTTON6,INPUT);  //digitalWrite(BUTTON6,HIGH);

  turnUntilLimit();
  Serial.println("\n** READY **");
}

void loop() {
  stateMachine();
  //testButtons();
  //turnUntilLimit();
  //testAllSeconds();
  //testFullTurns();
}

int stateNow=0;
int buttonPins[6]={BUTTON1,BUTTON2,BUTTON3,BUTTON4,BUTTON5,BUTTON6};
int buttonStates[6]={0,0,0,0,0,0};
int buttonRelease[6]={0,0,0,0,0,0};

void updateButtonStates() {
  for(int i=0;i<6;++i) {
    int b = digitalRead(buttonPins[i]);
    buttonRelease[i]=0;
    if(buttonStates[i] != b) {
      buttonStates[i] = b;
      if(b==LOW) buttonRelease[i] = 1;
    }
  }
}


void stateMachine() {
  updateButtonStates();
  
  // update state
  if(buttonStates[0]==HIGH) {
    // while button 0 is held, turn forward.
    stepForward();
    stateNow=0;
  }
  if(buttonStates[1]==HIGH) {
    // while button 1 is held, turn backward.
    stepBackward();
    stateNow=0;
  }
    
       if(buttonRelease[2]==1) startClock();
  else if(buttonRelease[3]==1) stopClock();
  else if(buttonRelease[4]==1) goToMinus27();
  else if(buttonRelease[5]==1) goToMinus6();
  // else no state change

  // act on current state
  if(stateNow==1) stepOneSecond(1);
  // else don't tick.
}

void stopClock() {
  if(stateNow==1) Serial.println("Stop.");
  stateNow = 0;
}

void startClock() {
  if(stateNow==0) Serial.println("Start.");
  stateNow = 1;
}

void goToMinus27() {
  turnUntilLimit();
  Serial.println("Going to -27.");
  stepper1.step(STEPS_PER_SECOND*-27);
  stateNow = 0;
}

void goToMinus6() {
  turnUntilLimit();
  Serial.println("Going to -6.");
  stepper1.step(STEPS_PER_SECOND*-6);
  stateNow = 0;
}

void testButtons() {
  static long v=0;
  
  stepper1.step(1);
  Serial.print(v++);
  Serial.print('\t');
  Serial.print(digitalRead(LIMIT_SWITCH)==HIGH?"1 ":"0 ");
  Serial.print(digitalRead(BUTTON1)==HIGH?"1":"0");
  Serial.print(digitalRead(BUTTON2)==HIGH?"1":"0");
  Serial.print(digitalRead(BUTTON3)==HIGH?"1":"0");
  Serial.print(digitalRead(BUTTON4)==HIGH?"1":"0");
  Serial.print(digitalRead(BUTTON5)==HIGH?"1":"0");
  Serial.print(digitalRead(BUTTON6)==HIGH?"1":"0");
  Serial.println();
  delay(50);
}

void stepForward() {
  stepper1.step(1);
}

void stepBackward() {
  stepper1.step(-1);
}

void turnUntilLimit() {
  Serial.println("Turning to limit.");
  while(digitalRead(LIMIT_SWITCH)==LOW) {
    stepForward();
  }
}

void testAllSeconds() {
  testSeconds(6,-1);  delay(250);
  testSeconds(6,1);  delay(500);
  
  testSeconds(27,-1);  delay(250);
  testSeconds(27,1);  delay(500);
}

void testFullTurns() {
  stepper1.step( STEPS_PER_TURN);  delay(250);
  stepper1.step(-STEPS_PER_TURN);  delay(250);
}

void testSeconds(int seconds,int arg0) {
  for(int i=0;i<seconds;++i) {
    stepOneSecond(arg0);
  }
}


void stepOneSecond(int arg0) {
  long t=millis();
  stepper1.step(STEPS_PER_SECOND*arg0);
  while(millis()-t<1000);
  handPos += STEPS_PER_SECOND*arg0;
}

