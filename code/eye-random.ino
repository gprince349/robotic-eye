/**********************************************************************
                     2eyeExpressionCycler - Routine 4
  Same mechanism as Routine 2 but with a hard speed floor so movements
  never feel twitchy — all motion stays calm even in "surprised" mood.
 **********************************************************************/

#include <Servo.h>

// ─────────────────────────────────────────────────────────────────────
//  Pin assignments  (D5–D10 on Arduino Nano)
// ─────────────────────────────────────────────────────────────────────
#define PIN_L_TOP_LID  5
#define PIN_L_X        6
#define PIN_L_Y        7

#define PIN_R_TOP_LID  8
#define PIN_R_X        9
#define PIN_R_Y        10

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
// ─────────────────────────────────────────────────────────────────────

#define LX_MID    90
#define LX_RANGE  35

#define LY_MID    90
#define LY_RANGE  30

#define L_LID_OPEN   60
#define L_LID_SHUT   120

#define RX_MID    90
#define RX_RANGE  35

#define RY_MID    90
#define RY_RANGE  30

#define R_LID_OPEN   120
#define R_LID_SHUT   60

// ─────────────────────────────────────────────────────────────────────
//  Derived limits
// ─────────────────────────────────────────────────────────────────────
#define LX_LEFT_LIM   (LX_MID + LX_RANGE / 2)
#define LX_RIGHT_LIM  (LX_MID - LX_RANGE / 2)
#define LY_UP_LIM     (LY_MID + LY_RANGE / 2)
#define LY_DOWN_LIM   (LY_MID - LY_RANGE / 2)

#define RX_LEFT_LIM   (RX_MID + RX_RANGE / 2)
#define RX_RIGHT_LIM  (RX_MID - RX_RANGE / 2)
#define RY_UP_LIM     (RY_MID - RY_RANGE / 2)
#define RY_DOWN_LIM   (RY_MID + RY_RANGE / 2)

// ─────────────────────────────────────────────────────────────────────
//  Speed floor — no servo step will ever be faster than this (ms/°)
//  Raise this value if movements still feel too quick on your hardware.
// ─────────────────────────────────────────────────────────────────────
#define MIN_SPD 10

// ─────────────────────────────────────────────────────────────────────
//  Mood / behavior state
// ─────────────────────────────────────────────────────────────────────
int topMoodShift = 0;
int moodMult     = 4;

// ─────────────────────────────────────────────────────────────────────
//  Timing
// ─────────────────────────────────────────────────────────────────────
const unsigned long MOOD_PERIOD = 13000;
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

  LtopLid.write(L_LID_OPEN);
  LxEye.write(LX_MID);
  LyEye.write(LY_MID);
  RtopLid.write(R_LID_OPEN);
  RxEye.write(RX_MID);
  RyEye.write(RY_MID);

  delay(1000);

  startMillis = millis();
  Serial.println("Ready!");
}

// ─────────────────────────────────────────────────────────────────────
//  Loop
// ─────────────────────────────────────────────────────────────────────
void loop() {
  currentMillis = millis();
  if (currentMillis - startMillis >= MOOD_PERIOD) {
    expressionCycler(random(1, 5));
    startMillis = currentMillis;
  }

  int weights[] = {25, 35, 30, 10};
  actionCycler(weightedRandom(weights, 4));
}

// ─────────────────────────────────────────────────────────────────────
//  calmSpd — clamps a computed speed to never exceed MIN_SPD
// ─────────────────────────────────────────────────────────────────────
int calmSpd(int spd) {
  return max(spd, MIN_SPD);
}

// ─────────────────────────────────────────────────────────────────────
//  sweepDual / sweepQuad
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
// ─────────────────────────────────────────────────────────────────────
void blink(int spd1, int gap, int spd2) {
  closeEye(spd1);
  delay(gap);
  openEye(spd2);
}

void closeEye(int spd) {
  sweepDual(LtopLid, L_LID_OPEN + topMoodShift, L_LID_SHUT,
            RtopLid, R_LID_OPEN - topMoodShift, R_LID_SHUT,
            spd);
}

void openEye(int spd) {
  sweepDual(LtopLid, L_LID_SHUT, L_LID_OPEN + topMoodShift,
            RtopLid, R_LID_SHUT, R_LID_OPEN - topMoodShift,
            spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Horizontal movement
// ─────────────────────────────────────────────────────────────────────
void lookLeftLim(int spd) {
  sweepDual(LxEye, LX_MID,       LX_LEFT_LIM,
            RxEye, RX_MID,       RX_LEFT_LIM,  spd);
}

void lookRightMid(int spd) {
  sweepDual(LxEye, LX_LEFT_LIM,  LX_MID,
            RxEye, RX_LEFT_LIM,  RX_MID,       spd);
}

void lookRightLim(int spd) {
  sweepDual(LxEye, LX_MID,       LX_RIGHT_LIM,
            RxEye, RX_MID,       RX_RIGHT_LIM, spd);
}

void lookLeftMid(int spd) {
  sweepDual(LxEye, LX_RIGHT_LIM, LX_MID,
            RxEye, RX_RIGHT_LIM, RX_MID,       spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Vertical movement
// ─────────────────────────────────────────────────────────────────────
void lookUpLim(int spd) {
  sweepDual(LyEye, LY_MID,      LY_UP_LIM,
            RyEye, RY_MID,      RY_UP_LIM,   spd);
}

void lookDownMid(int spd) {
  sweepDual(LyEye, LY_UP_LIM,   LY_MID,
            RyEye, RY_UP_LIM,   RY_MID,      spd);
}

void lookDownLim(int spd) {
  sweepDual(LyEye, LY_MID,      LY_DOWN_LIM,
            RyEye, RY_MID,      RY_DOWN_LIM, spd);
}

void lookUpMid(int spd) {
  sweepDual(LyEye, LY_DOWN_LIM, LY_MID,
            RyEye, RY_DOWN_LIM, RY_MID,      spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Micro-saccade — jitter is intentionally kept small and calm here
// ─────────────────────────────────────────────────────────────────────
void microSaccade() {
  int jx = random(-2, 3);
  int jy = random(-2, 3);
  int spd = calmSpd(5);
  sweepQuad(LxEye, LX_MID, LX_MID + jx,
            RxEye, RX_MID, RX_MID + jx,
            LyEye, LY_MID, LY_MID + jy,
            RyEye, RY_MID, RY_MID + jy, spd);
  delay(random(80, 250));
  sweepQuad(LxEye, LX_MID + jx, LX_MID,
            RxEye, RX_MID + jx, RX_MID,
            LyEye, LY_MID + jy, LY_MID,
            RyEye, RY_MID + jy, RY_MID, spd);
}

// ─────────────────────────────────────────────────────────────────────
//  Weighted random
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
//  Action cycler
//  spd is floored by calmSpd() so even low moodMult values stay gentle.
// ─────────────────────────────────────────────────────────────────────
void actionCycler(int action) {
  int spd      = calmSpd(random(3, 7) * moodMult);
  int holdTime = random(200, 600);   // was 100–400; longer pauses feel calmer

  int diagLX = random(min(LX_RIGHT_LIM, LX_LEFT_LIM), max(LX_RIGHT_LIM, LX_LEFT_LIM) + 1);
  int diagLY = random(min(LY_DOWN_LIM,  LY_UP_LIM),   max(LY_DOWN_LIM,  LY_UP_LIM)   + 1);
  int diagRX = random(min(RX_RIGHT_LIM, RX_LEFT_LIM), max(RX_RIGHT_LIM, RX_LEFT_LIM) + 1);
  int diagRY = random(min(RY_UP_LIM,    RY_DOWN_LIM), max(RY_UP_LIM,    RY_DOWN_LIM) + 1);

  microSaccade();

  switch (action) {
    case 1: // blink
      blink(calmSpd(random(4, 9) * moodMult),
            random(50, 250),
            calmSpd(random(6, 13) * moodMult));
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
//  Expression cycler
//  moodMult floor raised across all moods so the MIN_SPD guard is
//  rarely needed — movements stay naturally paced by moodMult alone.
//    Normal:    moodMult 4  → spd ~12–28 ms/°
//    Surprised: moodMult 3  → spd ~10–21 ms/° (fast but not twitchy)
//    Suspicious:moodMult 6  → spd ~18–42 ms/°
//    Tired:     moodMult 10 → spd ~30–70 ms/°
// ─────────────────────────────────────────────────────────────────────
void expressionCycler(int mood) {
  switch (mood) {
    case 1: // normal
      topMoodShift = 0;
      moodMult = 4;
      blink(calmSpd(12), 120, calmSpd(14));
      Serial.println("Mood: Normal");
      break;

    case 2: // surprised — wide eyes, quicker but still calm
      topMoodShift = 5;
      moodMult = 3;
      blink(calmSpd(10), 200, calmSpd(12));
      Serial.println("Mood: Surprised");
      break;

    case 3: // suspicious — squint, slow
      topMoodShift = -5;
      moodMult = 6;
      blink(calmSpd(18), 500, calmSpd(20));
      Serial.println("Mood: Suspicious");
      break;

    case 4: // tired — droopy, very slow
      topMoodShift = -5;
      moodMult = 10;
      blink(calmSpd(25), 0, calmSpd(25));
      Serial.println("Mood: Tired");
      break;
  }
}
