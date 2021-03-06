#include <Servo.h>
#include <MusicalNotes.h>

//To try this example you'll need the following:
//0) Meccanoid G15 in its human form
//1) Any Arduino
//2) Motor shield or motor driver, to drive robot's wheels. I tried the one based on L293D and it was quite decent
//3) Tamiya Mini female connector with wires - to connect to the robots' battery
//4) Male connectors: CWF-2 to connect one motor and PLS-2 to connect another
//5) 3 resistors of 22 kOhm and 3 resistors of 1 kOhm - to make pullup resistors (according to the manual)
//6) Download the library: http://cdn.meccano.com/open-source/meccanoid-library.zip
//7) You can also check out the reference material, to understand, how do the Smart Modules work
//   http://cdn.meccano.com/open-source/Meccano_SmartModuleProtocols_2015.pdf

//Pins to connect Meccanoids' servos, where chain 1 is left arm, chain 2 is head and chain 3 is right arm
//pins can be any digital pins, not necessary PWM
 
Servo leftArmPitch;
Servo leftArmRoll;
Servo leftArmElbow;

Servo headRoll;
Servo headYaw;

Servo rightArmPitch;
Servo rightArmRoll;
Servo rightArmElbow;

const int servoMinValue = 0;
const int servoQtValue = 50;
const int servoMidValue = 100;
const int servoMQtValue = 150;
const int servoMaxValue = 200;

const bool leftArm = 0;
const bool rightArm = 1;

const int leftArmPitchPin = 0;
const int leftArmRollPin = 1;
const int leftArmElbowPin = 2;

const int headRollPin = 3;
const int headYawPin = 4;

const int rightArmPitchPin = 5;
const int rightArmRollPin = 6;
const int rightArmElbowPin = 7;

const int presencePin = 8;
const int speakerPin = 11;

const int ledPin = 13;

//pins to set speed on motor driver
const int leftSpeedPin = 9;
const int rightSpeedPin = 10;

//pins to set direction on motor driver
const int leftDirPin = 12;
const int rightDirPin = 13;

int index = 0;
bool presence = false;
bool timer = true;// -------------------------------------------------------------------------TIMER!!!!
int timerDelay = 10000;// -------------------------------------------------------------------------TIMER!!!!

void beep (int speakerPin, float noteFrequency, long noteDuration) {
  int x;
  // Convert the frequency to microseconds
  float microsecondsPerWave = 1000000 / noteFrequency;
  // Calculate how many HIGH/LOW cycles there are per millisecond
  float millisecondsPerCycle = 1000 / (microsecondsPerWave * 2);
  // Multiply noteDuration * number or cycles per millisecond
  float loopTime = noteDuration * millisecondsPerCycle;
  // Play the note for the calculated loopTime.
  for (x = 0; x < loopTime; x++)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(microsecondsPerWave);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(microsecondsPerWave);
  }
}

void r2D2() {
  beep(speakerPin, note_A7, 100); //A
  beep(speakerPin, note_G7, 100); //G
  beep(speakerPin, note_E7, 100); //E
  beep(speakerPin, note_C7, 100); //C
  beep(speakerPin, note_D7, 100); //D
  beep(speakerPin, note_B7, 100); //B
  beep(speakerPin, note_F7, 100); //F
  beep(speakerPin, note_C8, 100); //C
  beep(speakerPin, note_A7, 100); //A
  beep(speakerPin, note_G7, 100); //G
  beep(speakerPin, note_E7, 100); //E
  beep(speakerPin, note_C7, 100); //C
  beep(speakerPin, note_D7, 100); //D
  beep(speakerPin, note_B7, 100); //B
  beep(speakerPin, note_F7, 100); //F
  beep(speakerPin, note_C8, 100); //C
}

void setJoint(int arm, int angle){
  bool sideArm=(arm>2);
  if(arm==leftArmRollPin || arm==rightArmRollPin){
    angle=( (sideArm*-1) + (!sideArm*1) ) * ( !sideArm*angle + (sideArm*(angle-servoMaxValue)) );
  }
  if(arm==leftArmPitchPin || arm==rightArmPitchPin){
    angle=( (!sideArm*-1) + (sideArm*1) ) * ( sideArm*angle + (!sideArm*(angle-servoMaxValue)) );
  }
  if(arm==leftArmElbowPin || arm==rightArmElbowPin){
    angle=(( (!sideArm*-1) + (sideArm*1) ) *angle)/2+100;
  }
  switch(arm){
    case leftArmPitchPin:
      leftArmPitch.write(angle); 
      break;
    case leftArmRollPin:
      leftArmRoll.write(angle); 
      break;
    case leftArmElbowPin:
      leftArmElbow.write(angle); 
      break;
      
    case headRollPin:
      headRoll.write(angle); 
      break;
    case headYawPin:
      headYaw.write(angle); 
      break;
      
    case rightArmPitchPin:
      rightArmPitch.write(angle); 
      break;
    case rightArmRollPin:
      rightArmRoll.write(angle); 
      break;
    case rightArmElbowPin:
      rightArmElbow.write(angle); 
      break;
  }
}

void waveHand(bool arm, int timeD) {
  setJoint(((arm*5)+leftArmRollPin), servoMQtValue);
  delay(timeD);

  setJoint(((arm*5)+leftArmRollPin), servoMaxValue);
  delay(timeD);
}

void handGesture(bool arm, int timeD) {
  setJoint(((arm*5)+leftArmElbowPin), servoMidValue);
  delay(timeD);

  setJoint(((arm*5)+leftArmElbowPin), servoMinValue);
  delay(timeD);
}

void setup() {
  leftArmPitch.attach(leftArmPitchPin);
  leftArmRoll.attach(leftArmRollPin);
  leftArmElbow.attach(leftArmElbowPin);

  headRoll.attach(headRollPin);
  headYaw.attach(headYawPin);

  rightArmPitch.attach(rightArmPitchPin);
  rightArmRoll.attach(rightArmRollPin);
  rightArmElbow.attach(rightArmElbowPin);
  
  pinMode(leftSpeedPin, OUTPUT);
  pinMode(rightSpeedPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
  
  pinMode(ledPin, OUTPUT);
  pinMode(presencePin, INPUT);
  pinMode(speakerPin, OUTPUT);

  setJoint(0, 0);
  setJoint(1, 0);
  setJoint(2, 0);
  setJoint(3, 0);
  setJoint(4, 0);
  setJoint(5, 0);
  setJoint(6, 0);
  setJoint(7, 0);

  r2D2();
}

void loop() {
  //  Serial.println("S");
  if(!timer){
    presence = digitalRead(presencePin);
  }else{
    delay(timerDelay);
    presence = true;
  }
  digitalWrite(ledPin, presence);


  if (presence) {
    r2D2();
    delay(100);
    
    for (int times = 0; times < 5; times++) {
//      blinkEyes();
//      delay(100);
      waveHand(leftArm, 500);
    }

    setJoint(rightArmPitchPin, servoQtValue);
    setJoint(leftArmPitchPin, servoQtValue);//
    delay(100);

    for (int times = 0; times < 5; times++) {
//      blinkEyes();
//      delay(100);
      handGesture(rightArm, 500);
      handGesture(leftArm, 500);//
    }
    
    setJoint(rightArmPitchPin, servoMinValue);
    setJoint(leftArmPitchPin, servoMinValue);//
    delay(100);

    r2D2();
    if(timer){
      presence=false;
    }
  }
}
