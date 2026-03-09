/**********************************************************************
                       2eyeExpressionCycler V1.1.0
  This is an example for a simplified animatronic eye mechanism using 8 
  SG90 microservos and Adafruit 16-channel PWM & Servo driver.
  
  Code mimics realistic movement of the eye by cycling through multiple 
  behaviors and moods.

  The mood multiplier changes the speed of a movement, the mood shift 
  offsets the top and bottom eyelids.
 
                       Written by John Strope 2021
                        www.johnstrope.com/eyemech
            All text above must be included in any redistribution
 **********************************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVO_FREQ 50           // Analog servos run at ~50 Hz updates
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// ************ Here are some things you will need to tweak ************
/*  Put in the limits found from your 'buttonBlink' and 'lookXY' scripts.
    Make sure that nothing is going to collide when fully closed, etc.
*/
#define LxMid 295              // Left X axis centerpoint (mine was near 330)
#define LxRange  65             // Left X axis range of motion (start small ~50)
#define LyMid  305              // Left Y axis centerpoint (mine was near 330)
#define LyRange  50             // Left Y axis range of motion (start small ~50)

#define LTopLidShut  225        // top eyelid shut limit (from 'buttonBlink')
#define LTopLidOpen  340        // top eyelid open limit (from 'buttonBlink')
// #define LBotLidShut  505        // bottom eyelid shut limit (from 'buttonBlink')
// #define LBotLidOpen  460        // bottom eyelid open limit (from 'buttonBlink')

#define RxMid  360              // Right X axis centerpoint (mine was near 330)
#define RxRange  65             // Right X axis range of motion (start small ~50)                               
#define RyMid  380              // RightY axis centerpoint (mine was near 330)                              
#define RyRange  50             // Right Y axis range of motion (start small ~50)

#define RTopLidShut  360        // top eyelid shut limit (from 'buttonBlink')
#define RTopLidOpen  250        // top eyelid open limit (from 'buttonBlink')
// #define RBotLidShut  505        // bottom eyelid shut limit (from 'buttonBlink')
// #define RBotLidOpen  460        // bottom eyelid open limit (from 'buttonBlink')

uint8_t LbotLid = 0;            // This is the servo board slot for the top lid
uint8_t LtopLid = 1;            // This is the servo board slot for the bottom lid
uint8_t LxEye = 2;              // This is the servo board pins for the Left eye X axis
uint8_t LyEye= 3;              // This is the servo board pins for the Left eye Y axis
uint8_t RbotLid = 4;            // This is the servo board slot for the top lid
uint8_t RtopLid = 5;            // This is the servo board slot for the bottom lid
uint8_t RxEye = 6;              // This is the servo board pins for the Right eye X axis
uint8_t RyEye = 7;              // This is the servo board pins for the Right eye Y axis
const unsigned long period = 10;     // How many seconds between each mood change

/******************* Probably don't change these *********************/
#define LrightLimit  LxMid-(LxRange/2)          // This represents the left X axis right limit
#define LleftLimit  LxMid+(LxRange/2)           // This represents the left X axis left limit
#define LupLimit  LyMid+(LyRange/2)             // This represents the left Y axis up limit
#define LdownLimit  LyMid-(LyRange/2)           // This represents the left Y axis down limit
#define RrightLimit  RxMid-(LxRange/2)          // This represents the right X axis right limit
#define RleftLimit  RxMid+(RxRange/2)           // This represents the right X axis left limit
#define RupLimit  RyMid-(RyRange/2)             // This represents the right Y axis up limit
#define RdownLimit  RyMid+(RyRange/2)           // This represents the right Y axis down limit
#define USMIN  600
#define USMAX  2400
int topMoodShift = 0;            // initializing variable that offsets top eyelid
int botMoodShift = 0;            // initializing variable that offsets bottom eyelid
int moodMult = 4;                // initializing variable that offsets the movement speed
unsigned long startMillis;       // initializing variable that starts a timer
unsigned long currentMillis;     // initializing variable that tracks the current elapsed time

const int onOffPin = 2;   // This is the Arduino digital pin for 'blink' button
bool state = false;
/******************* Initializing PWM and Serial *********************/
void setup() {
  Serial.println("Startup!\r");
  Serial.println("Startup!\r");
  Serial.println("Startup!\r");
  pinMode(onOffPin, INPUT_PULLUP);
  Serial.begin(9600);
  pwm.begin();                  // Powering up the servo board
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(1000);
  startMillis = millis();
  Serial.println("Ready to Go!\r");
  Serial.println("Ready to Go!\r");
  Serial.println("Ready to Go!\r");
}

// *******************  Triggering the Cycles  ************************
/* Here there are two cycles: one that chooses an action, and the other
   that decides the mood. Actions are decided based on a random number,
   while moods are randomly decided every ten seconds.
*/
void loop() {
  int onOffState;
  onOffState = digitalRead(onOffPin);
  if(onOffState == LOW) {
    state = false;                   // Delay before another event can trigger
    Serial.println("State changed!\r");
  }else{
    state = true;
  }

  if(state){
    currentMillis = millis();
    if (currentMillis-startMillis >= period*1000) {
      expressionCycler(random(1,5));
      startMillis = currentMillis;
    }

    actionCycler(random(1,3));      // Picks one of the 7 listed actions
    // if (currentMillis-startMillis >= (period-7)*1000){
    //   Serial.println("going on...");
    // }
  }

}

// ************************  Action Cycles  ***************************
/* Here we have defined several actions: blinking and looking in the 
   left, right, up, and down directions. The code generates a random 
   number to decide which action to perform.
*/
void actionCycler(int action) { 
  Serial.println("action-> " + action);
  switch (action) {
    // case 1: //blink
    //   delay(random(2,10)*100*moodMult);
    //   blink(random(2,5)*moodMult,random(1,10)*100*moodMult,random(2,5)*moodMult);
    //   Serial.println("Blink");
    //   break;
    case 1: //lookLeft
      delay(random(2,5)*100*moodMult);
      lookLeftLim(5*moodMult);
      delay(random(2,5)*100*moodMult);
      lookRightMid(5*moodMult);
      Serial.println("Right");
      // delay(random(2,10)*100*moodMult);
      lookRightLim(5*moodMult);
      delay(random(2,5)*100*moodMult);
      lookLeftMid(5*moodMult);
      Serial.println("Right");
      break;
    case 2: //lookUp
      delay(random(2,5)*100*moodMult);
      lookUpLim(5*moodMult);
      delay(random(2,5)*100*moodMult);
      lookDownMid(5*moodMult);
      Serial.println("Up");
      // delay(random(2,10)*100*moodMult);
      lookDownLim(5*moodMult);
      delay(random(2,5)*100*moodMult);
      lookUpMid(5*moodMult);
      Serial.println("Down");
      break;
  }
}

// *************************  Mood Cycles  ****************************
/* Here we have also defined several moods: normal, surprised, suspicious, 
   and tired. The code generates a random number to decide which action 
   to perform. Increasing the 'topMoodShift' opens the top lid farther, 
   while decreasing makes it squint. Same for the 'botMoodShift'. The 
   'moodMult' can NEVER be less than 1, and controls the speed of action. 
   
   Speed of 1 = normal operation, speed of 10 = slower operation.
*/
void expressionCycler(int mood) { 
  Serial.println("expression->" + mood);
  switch (mood) {
    case 1: //normal
      topMoodShift = 0;
      botMoodShift = 0;
      moodMult = 2;
      blink(10,100,10);
      Serial.println("Mood: Normal");
      delay(100);
      break;
    case 2: //surprised
      topMoodShift = 5;
      botMoodShift = 5;
      moodMult = 2;
      blink(15,100,15);
      Serial.println("Mood: Surprised");
      delay(100);
      break;
    case 3: //suspicious
      topMoodShift = -5;
      botMoodShift = -5;
      moodMult = 3;
      blink(3,500,7);
      Serial.println("Mood: Suspicious");
      delay(100);
      break;
    case 4: //tired
      topMoodShift = -5;
      botMoodShift = 0;
      moodMult = 4;
      blink(15,0,15);
      Serial.println("Mood: Tired");
      delay(100);
      break;
  }
}

// **********************  Blink function  ****************************
/* Expecting input of (openingSpeed, stopDuration, closingSpeed)
   Speed can range from 0(fast) => 15(slow). 
   
   Ex: inputing (0,0,0) => fastest possible.
   
   Gap is in milliseconds. Ex: (3,1000,3) would be slower with a pause 
   of 1 second.
*/
void blink(int spd1, int gap, int spd2) {
  Serial.println("Blink\r");
  closeEye(spd1);
  delay(gap);       // This is the time between closing and reopening
  openEye(spd2);
}
/*************************  Closing Lids  ****************************/
void closeEye(int spd) {
  for (uint16_t LpulseLen1 = LTopLidOpen+topMoodShift, RpulseLen1 = RTopLidOpen+topMoodShift; 
  LpulseLen1 > LTopLidShut || RpulseLen1 < RTopLidShut ;
  LpulseLen1--, RpulseLen1++) {
    if (LpulseLen1 > LTopLidShut) {
      pwm.setPWM(LtopLid, 0, LpulseLen1);
    }
    if (RpulseLen1 < RTopLidShut) {
      pwm.setPWM(RtopLid, 0, RpulseLen1);
    }
    delay(spd);
  }
}
/**************************  Opening Lids  ***************************/
void openEye(int spd) {
  for (uint16_t LpulseLen1 = LTopLidShut, RpulseLen1 = RTopLidShut;
   LpulseLen1 < LTopLidOpen+topMoodShift || RpulseLen1 > RTopLidOpen+topMoodShift;
    LpulseLen1++, RpulseLen1--) {
    if (LpulseLen1 < LTopLidOpen+topMoodShift) {
      pwm.setPWM(LtopLid, 0, LpulseLen1);
    }
    if (RpulseLen1 > RTopLidOpen+topMoodShift) {
      pwm.setPWM(RtopLid, 0, RpulseLen1);
    }
    delay(spd);
  }
}


// **********************  Look Around  *******************************
/**************************  Look L/R  *******************************/
void lookLeftLim(int spd) {
  for (uint16_t LpulseLen = LxMid, RpulseLen = RxMid; LpulseLen < LleftLimit 
  || RpulseLen < RleftLimit; LpulseLen++, RpulseLen++) {
    if (LpulseLen < LleftLimit) {
      pwm.setPWM(LxEye, 0, LpulseLen);
    }
    if (RpulseLen < RleftLimit) {
      pwm.setPWM(RxEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
void lookRightMid(int spd) {
  for (uint16_t LpulseLen = LleftLimit, RpulseLen = RleftLimit; LpulseLen > LxMid || 
  RpulseLen > RxMid; LpulseLen--, RpulseLen--) {
    if (LpulseLen > LxMid) {
      pwm.setPWM(LxEye, 0, LpulseLen);
    }
    if (RpulseLen > RxMid) {
      pwm.setPWM(RxEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
void lookRightLim(int spd) {
  for (uint16_t LpulseLen = LxMid, RpulseLen = RxMid; LpulseLen > LrightLimit || 
  RpulseLen > RrightLimit; LpulseLen--, RpulseLen--) {
    if (LpulseLen > LrightLimit) {
      pwm.setPWM(LxEye, 0, LpulseLen);
    }
    if (RpulseLen > RrightLimit) {
      pwm.setPWM(RxEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
void lookLeftMid(int spd) {
  for (uint16_t LpulseLen = LrightLimit, RpulseLen = RrightLimit; LpulseLen < LxMid || 
  RpulseLen < RxMid; LpulseLen++, RpulseLen++) {
    if (LpulseLen < LxMid) {
      pwm.setPWM(LxEye, 0, LpulseLen);
    }
    if (RpulseLen < RxMid) {
      pwm.setPWM(RxEye, 0, RpulseLen);
    }
    delay(spd);
  }
}

// ************************  Look Up/Down  ***************************/
void lookDownLim(int spd) {
  for (uint16_t LpulseLen = LyMid, RpulseLen = RyMid; LpulseLen > LdownLimit || 
  RpulseLen < RdownLimit; LpulseLen--, RpulseLen++) {
    if (LpulseLen > LdownLimit) {
      pwm.setPWM(LyEye, 0, LpulseLen);
    }
    if (RpulseLen < RdownLimit) {
      pwm.setPWM(RyEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
void lookUpMid(int spd) {
  for (uint16_t LpulseLen = LdownLimit, RpulseLen = RdownLimit; LpulseLen < LyMid || 
  RpulseLen > RyMid; LpulseLen++, RpulseLen--) {
    if (LpulseLen < LyMid) {
      pwm.setPWM(LyEye, 0, LpulseLen);
    }
    if (RpulseLen > RyMid) {
      pwm.setPWM(RyEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
void lookUpLim(int spd) {
  for (uint16_t LpulseLen = LyMid, RpulseLen = RyMid; LpulseLen < LupLimit || 
  RpulseLen > RupLimit; LpulseLen++, RpulseLen--) {
    if (LpulseLen < LupLimit) {
      pwm.setPWM(LyEye, 0, LpulseLen);
    }
    if (RpulseLen > RupLimit) {
      pwm.setPWM(RyEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
void lookDownMid(int spd) {
  for (uint16_t LpulseLen = LupLimit, RpulseLen = RupLimit; LpulseLen > LyMid || 
  RpulseLen < RyMid; LpulseLen--, RpulseLen++) {
    if (LpulseLen > LyMid) {
      pwm.setPWM(LyEye, 0, LpulseLen);
    }
    if (RpulseLen < RyMid) {
      pwm.setPWM(RyEye, 0, RpulseLen);
    }
    delay(spd);
  }
}
t