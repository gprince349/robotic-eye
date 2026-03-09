/**********************************************************************
                     2eyeExpressionCycler - Custom Build
  Animatronic dual-eye mechanism using 6 SG90 micro-servos driven
  directly from Arduino Nano pins D2–D7 via the Servo library.

  Each eye has:
    - 1 top eyelid servo
    - 1 horizontal (left/right) servo
    - 1 vertical (up/down) servo

  No PWM board required. No bottom lids.
 **********************************************************************/

#include <Servo.h>

// ─────────────────────────────────────────────────────────────────────
//  Pin assignments  (D2–D7 on Arduino Nano)
// ─────────────────────────────────────────────────────────────────────
#define PIN_L_TOP_LID  5    // Left  top eyelid
#define PIN_L_X        6    // Left  eye horizontal (left/right)
#define PIN_L_Y        7    // Left  eye vertical   (up/down)

#define PIN_R_TOP_LID  8    // Right top eyelid
#define PIN_R_X        9    // Right eye horizontal (left/right)
#define PIN_R_Y        10   // Right eye vertical   (up/down)

// ─────────────────────────────────────────────────────────────────────
//  Servo objects
// ─────────────────────────────────────────────────────────────────────
Servo LtopLid;
Servo LxEye;
Servo LyEye;

Servo RtopLid;
Servo RxEye;
Servo RyEye;    

// ─────────────────────────────────────────────────────────────────────
//  Calibration — tweak these to match your physical build
//  All values are in degrees (0–180)
// ─────────────────────────────────────────────────────────────────────

// Left eye — horizontal axis
#define LX_MID    90        // Center position
#define LX_RANGE  30        // Total range of motion (±20° from center)

// Left eye — vertical axis
#define LY_MID    90        // Center position
#define LY_RANGE  20        // Total range of motion (±15° from center)

// Left top eyelid
#define L_LID_OPEN   60     // Degrees when fully open
#define L_LID_SHUT   110    // Degrees when fully closed

// Right eye — horizontal axis (likely inverted vs. left)
#define RX_MID    90        // Center position
#define RX_RANGE  20        // Total range of motion

// Right eye — vertical axis (likely inverted vs. left)
#define RY_MID    90        // Center position
#define RY_RANGE  20        // Total range of motion

// Right top eyelid
#define R_LID_OPEN   120    // Degrees when fully open (mirrored from left)
#define R_LID_SHUT   70     // Degrees when fully closed

// ─────────────────────────────────────────────────────────────────────
//  Derived limits  (computed from mid + range above)
// ─────────────────────────────────────────────────────────────────────
#define LX_LEFT_LIM   (LX_MID + LX_RANGE / 2)
#define LX_RIGHT_LIM  (LX_MID - LX_RANGE / 2)
#define LY_UP_LIM     (LY_MID + LY_RANGE / 2)
#define LY_DOWN_LIM   (LY_MID - LY_RANGE / 2)

#define RX_LEFT_LIM   (RX_MID + RX_RANGE / 2)   // same direction as left eye
#define RX_RIGHT_LIM  (RX_MID - RX_RANGE / 2)
#define RY_UP_LIM     (RY_MID - RY_RANGE / 2)   // inverted direction
#define RY_DOWN_LIM   (RY_MID + RY_RANGE / 2)

// ─────────────────────────────────────────────────────────────────────
//  Mood / behavior state
// ─────────────────────────────────────────────────────────────────────
int topMoodShift = 0;          // Offsets the eyelid open position (+open, -squint)
int moodMult     = 8;          // Scales all movement speeds (higher = slower)

// ─────────────────────────────────────────────────────────────────────
//  Timing
// ─────────────────────────────────────────────────────────────────────
const unsigned long MOOD_PERIOD = 10000;  // ms between mood changes
unsigned long startMillis;
unsigned long currentMillis;

// ─────────────────────────────────────────────────────────────────────
//  Setup
// ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("Attaching servos...");

  LtopLid.attach(PIN_L_TOP_LID);
  LxEye.attach(PIN_L_X);
  LyEye.attach(PIN_L_Y);
  RtopLid.attach(PIN_R_TOP_LID);
  RxEye.attach(PIN_R_X);
  RyEye.attach(PIN_R_Y);

  // Move all servos to their neutral/center positions at startup
  LtopLid.write(L_LID_OPEN);
  LxEye.write(LX_MID);
  LyEye.write(LY_MID);
  RtopLid.write(R_LID_OPEN);
  RxEye.write(RX_MID);
  RyEye.write(RY_MID);

  delay(1000);   // Let servos reach start position before anything else runs

  startMillis = millis();
  Serial.println("Ready!");
}

// ─────────────────────────────────────────────────────────────────────
//  Loop  (to be filled in next steps)
// ─────────────────────────────────────────────────────────────────────
void loop() {
  currentMillis = millis();
  if (currentMillis - startMillis >= MOOD_PERIOD) {
    expressionCycler(random(1, 5));
    startMillis = currentMillis;
  }

  int weights[] = {40, 25, 25, 10};
  actionCycler(weightedRandom(weights, 4));
}

// ─────────────────────────────────────────────────────────────────────
//  Internal helper — move two servos simultaneously, 1° per step
//  Each servo steps toward its own target independently, so they both
//  start and finish in the same number of loop iterations (one per °
//  of whichever axis has further to travel).
// ─────────────────────────────────────────────────────────────────────
void sweepDual(Servo &s1, int from1, int to1,
               Servo &s2, int from2, int to2,
               int spd) {
  int p1 = from1;
  int p2 = from2;
  while (p1 != to1 || p2 != to2) {
    if (p1 != to1) { p1 += (to1 > p1) ? 1 : -1;  s1.write(p1); }
    if (p2 != to2) { p2 += (to2 > p2) ? 1 : -1;  s2.write(p2); }
    delay(spd);
  }
}

// ─────────────────────────────────────────────────────────────────────
//  Internal helper — move four servos simultaneously, 1° per step
//  Used for diagonal gaze and micro-saccades (X + Y at the same time)
// ─────────────────────────────────────────────────────────────────────
void sweepQuad(Servo &s1, int from1, int to1,
               Servo &s2, int from2, int to2,
               Servo &s3, int from3, int to3,
               Servo &s4, int from4, int to4,
               int spd) {
  int p1 = from1, p2 = from2, p3 = from3, p4 = from4;
  while (p1 != to1 || p2 != to2 || p3 != to3 || p4 != to4) {
    if (p1 != to1) { p1 += (to1 > p1) ? 1 : -1;  s1.write(p1); }
    if (p2 != to2) { p2 += (to2 > p2) ? 1 : -1;  s2.write(p2); }
    if (p3 != to3) { p3 += (to3 > p3) ? 1 : -1;  s3.write(p3); }
    if (p4 != to4) { p4 += (to4 > p4) ? 1 : -1;  s4.write(p4); }
    delay(spd);
  }
}

// ─────────────────────────────────────────────────────────────────────
//  Blink
//  spd1 — closing speed (ms/°, 0 = fastest)
//  gap  — pause between close and open (ms)
//  spd2 — opening speed
// ─────────────────────────────────────────────────────────────────────
void blink(int spd1, int gap, int spd2) {
  closeEye(spd1);
  delay(gap);
  openEye(spd2);
}

void closeEye(int spd) {
  sweepDual(LtopLid, L_LID_OPEN + topMoodShift, L_LID_SHUT,
            RtopLid, R_LID_OPEN + topMoodShift, R_LID_SHUT,
            spd);
}

void openEye(int spd) {
  sweepDual(LtopLid, L_LID_SHUT, L_LID_OPEN + topMoodShift,
            RtopLid, R_LID_SHUT, R_LID_OPEN + topMoodShift,
            spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Horizontal movement  (left / right)
//  Left eye and right eye are physically mirrored, so their X axes
//  move in opposite degree directions for the same gaze direction.
// ─────────────────────────────────────────────────────────────────────
void lookLeftLim(int spd) {
  sweepDual(LxEye, LX_MID,       LX_LEFT_LIM,
            RxEye, RX_MID,       RX_LEFT_LIM,
            spd);
}

void lookRightMid(int spd) {          // return to center after looking left
  sweepDual(LxEye, LX_LEFT_LIM,  LX_MID,
            RxEye, RX_LEFT_LIM,  RX_MID,
            spd);
}

void lookRightLim(int spd) {
  sweepDual(LxEye, LX_MID,       LX_RIGHT_LIM,
            RxEye, RX_MID,       RX_RIGHT_LIM,
            spd);
}

void lookLeftMid(int spd) {           // return to center after looking right
  sweepDual(LxEye, LX_RIGHT_LIM, LX_MID,
            RxEye, RX_RIGHT_LIM, RX_MID,
            spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Vertical movement  (up / down)
//  Same mirroring principle applies on the Y axes.
// ─────────────────────────────────────────────────────────────────────
void lookUpLim(int spd) {
  sweepDual(LyEye, LY_MID,        LY_UP_LIM,
            RyEye, RY_MID,        RY_UP_LIM,
            spd);
}

void lookDownMid(int spd) {           // return to center after looking up
  sweepDual(LyEye, LY_UP_LIM,    LY_MID,
            RyEye, RY_UP_LIM,    RY_MID,
            spd);
}

void lookDownLim(int spd) {
  sweepDual(LyEye, LY_MID,        LY_DOWN_LIM,
            RyEye, RY_MID,        RY_DOWN_LIM,
            spd);
}

void lookUpMid(int spd) {             // return to center after looking down
  sweepDual(LyEye, LY_DOWN_LIM,  LY_MID,
            RyEye, RY_DOWN_LIM,  RY_MID,
            spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Micro-saccade — tiny nervous jitter (±3°) on all gaze axes
//  Called before each action to make the eyes look alive at rest.
//  Uses sweepQuad so X and Y move simultaneously.
// ─────────────────────────────────────────────────────────────────────
void microSaccade() {
  int jx = random(-3, 4);   // -3 to +3
  int jy = random(-3, 4);
  sweepQuad(LxEye, LX_MID, LX_MID + jx,
            RxEye, RX_MID, RX_MID + jx,
            LyEye, LY_MID, LY_MID + jy,
            RyEye, RY_MID, RY_MID + jy, 3);
  delay(random(50, 200));
  sweepQuad(LxEye, LX_MID + jx, LX_MID,
            RxEye, RX_MID + jx, RX_MID,
            LyEye, LY_MID + jy, LY_MID,
            RyEye, RY_MID + jy, RY_MID, 3);
}

// ─────────────────────────────────────────────────────────────────────
//  Weighted random — returns 1-based action index
//  weights[] must sum to 100; count = number of elements
// ─────────────────────────────────────────────────────────────────────
int weightedRandom(int weights[], int count) {
  int r = random(0, 100);
  int cumulative = 0;
  for (int i = 0; i < count; i++) {
    cumulative += weights[i];
    if (r < cumulative) return i + 1;
  }
  return 1;
}

// ─────────────────────────────────────────────────────────────────────
//  Action cycler — picks one of 4 actions with randomised params
//  Case 1: blink       (~40% chance)
//  Case 2: look L/R    (~25% chance)
//  Case 3: look Up/Down(~25% chance)
//  Case 4: diagonal    (~10% chance)
// ─────────────────────────────────────────────────────────────────────
void actionCycler(int action) {
  int spd      = random(2, 6) * moodMult;
  int holdTime = random(1, 4) * 100;

  // Diagonal targets pre-computed (only used in case 4)
  // random() needs min < max, so use min/max of each axis pair
  int diagLX = random(min(LX_RIGHT_LIM, LX_LEFT_LIM), max(LX_RIGHT_LIM, LX_LEFT_LIM) + 1);
  int diagLY = random(min(LY_DOWN_LIM,  LY_UP_LIM),   max(LY_DOWN_LIM,  LY_UP_LIM)   + 1);
  int diagRX = random(min(RX_RIGHT_LIM, RX_LEFT_LIM), max(RX_RIGHT_LIM, RX_LEFT_LIM) + 1);
  int diagRY = random(min(RY_UP_LIM,    RY_DOWN_LIM), max(RY_UP_LIM,    RY_DOWN_LIM) + 1);

  microSaccade();

  switch (action) {
    case 1: // blink
      blink(random(3, 8) * moodMult,
            random(0, 3) * 100,
            random(5, 12) * moodMult);
      Serial.println("Action: Blink");
      break;

    case 2: // look left <-> right
      lookLeftLim(spd);
      delay(holdTime);
      lookRightMid(spd);
      delay(holdTime);
      lookRightLim(spd);
      delay(holdTime);
      lookLeftMid(spd);
      Serial.println("Action: LR");
      break;

    case 3: // look up <-> down
      lookUpLim(spd);
      delay(holdTime);
      lookDownMid(spd);
      delay(holdTime);
      lookDownLim(spd);
      delay(holdTime);
      lookUpMid(spd);
      Serial.println("Action: UD");
      break;

    case 4: // diagonal glance
      sweepQuad(LxEye, LX_MID, diagLX,
                RxEye, RX_MID, diagRX,
                LyEye, LY_MID, diagLY,
                RyEye, RY_MID, diagRY, spd);
      delay(holdTime);
      sweepQuad(LxEye, diagLX, LX_MID,
                RxEye, diagRX, RX_MID,
                LyEye, diagLY, LY_MID,
                RyEye, diagRY, RY_MID, spd);
      Serial.println("Action: Diagonal");
      break;
  }
}

// ─────────────────────────────────────────────────────────────────────
//  Expression cycler — sets mood state, fires every MOOD_PERIOD ms
//  topMoodShift: positive = wider open, negative = squint
//  moodMult:     higher = slower movement
// ─────────────────────────────────────────────────────────────────────
void expressionCycler(int mood) {
  switch (mood) {
    case 1: // normal
      topMoodShift = 0;
      moodMult = 2;
      blink(10, 100, 10);
      Serial.println("Mood: Normal");
      break;

    case 2: // surprised — wide eyes, fast
      topMoodShift = 5;
      moodMult = 1;
      blink(5, 200, 5);
      Serial.println("Mood: Surprised");
      break;

    case 3: // suspicious — squint, slower
      topMoodShift = -5;
      moodMult = 4;
      blink(3, 500, 7);
      Serial.println("Mood: Suspicious");
      break;

    case 4: // tired — droopy, very slow
      topMoodShift = -5;
      moodMult = 6;
      blink(15, 0, 15);
      Serial.println("Mood: Tired");
      break;
  }
}
