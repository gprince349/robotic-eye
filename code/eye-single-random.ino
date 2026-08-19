/**********************************************************************
                     Single-Eye Lid Random
  Same mood/action cycler as eye-random, but for one eye with only
  top + bottom eyelid servos. No X/Y eyeball movement.
 **********************************************************************/

#include <Servo.h>

// ─────────────────────────────────────────────────────────────────────
//  Pin assignments  (Arduino Nano)
// ─────────────────────────────────────────────────────────────────────
#define PIN_TOP_LID  5
#define PIN_BOT_LID  6

// ─────────────────────────────────────────────────────────────────────
//  Servo objects
// ─────────────────────────────────────────────────────────────────────
Servo topLid;
Servo botLid;

// ─────────────────────────────────────────────────────────────────────
//  Calibration — tweak these to match your physical build
// ─────────────────────────────────────────────────────────────────────
#define TOP_LID_OPEN   60
#define TOP_LID_SHUT   120

#define BOT_LID_OPEN   120
#define BOT_LID_SHUT   80

// ─────────────────────────────────────────────────────────────────────
//  Speed floor — no servo step will ever be faster than this (ms/°)
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
  Serial.println("Attaching lid servos...");

  topLid.attach(PIN_TOP_LID);
  botLid.attach(PIN_BOT_LID);

  topLid.write(TOP_LID_OPEN);
  botLid.write(BOT_LID_OPEN);

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

  // blink, slow blink, double blink, flutter
  int weights[] = {35, 25, 25, 15};
  actionCycler(weightedRandom(weights, 4));
}

// ─────────────────────────────────────────────────────────────────────
//  calmSpd — clamps a computed speed to never exceed MIN_SPD
// ─────────────────────────────────────────────────────────────────────
int calmSpd(int spd) {
  return max(spd, MIN_SPD);
}

// ─────────────────────────────────────────────────────────────────────
//  sweepDual
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
//  Blink — bottom lid mirrors top (mood shift inverted like dual-eye R lid)
// ─────────────────────────────────────────────────────────────────────
void blink(int spd1, int gap, int spd2) {
  closeEye(spd1);
  delay(gap);
  openEye(spd2);
}

void closeEye(int spd) {
  sweepDual(topLid, TOP_LID_OPEN + topMoodShift, TOP_LID_SHUT,
            botLid, BOT_LID_OPEN - topMoodShift, BOT_LID_SHUT,
            spd);
}

void openEye(int spd) {
  sweepDual(topLid, TOP_LID_SHUT, TOP_LID_OPEN + topMoodShift,
            botLid, BOT_LID_SHUT, BOT_LID_OPEN - topMoodShift,
            spd);
}

// Current open targets given mood (used by partial/flutter moves)
int topOpenPos() { return TOP_LID_OPEN + topMoodShift; }
int botOpenPos() { return BOT_LID_OPEN - topMoodShift; }

// ─────────────────────────────────────────────────────────────────────
//  Micro-flutter — tiny lid jitter instead of eyeball saccade
// ─────────────────────────────────────────────────────────────────────
void microFlutter() {
  int jt = random(-2, 3);
  int jb = random(-2, 3);
  int spd = calmSpd(5);
  int tFrom = topOpenPos();
  int bFrom = botOpenPos();
  int tTo = tFrom + jt;
  int bTo = bFrom + jb;

  sweepDual(topLid, tFrom, tTo,
            botLid, bFrom, bTo, spd);
  delay(random(80, 250));
  sweepDual(topLid, tTo, tFrom,
            botLid, bTo, bFrom, spd);
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
//  Action cycler — lid-only variants
// ─────────────────────────────────────────────────────────────────────
void actionCycler(int action) {
  int holdTime = random(200, 600);

  microFlutter();

  switch (action) {
    case 1: // normal blink
      blink(calmSpd(random(4, 9) * moodMult),
            random(50, 250),
            calmSpd(random(6, 13) * moodMult));
      Serial.println("Action: Blink");
      break;

    case 2: // slow blink — longer hold shut
      blink(calmSpd(random(8, 14) * moodMult),
            random(300, 700),
            calmSpd(random(8, 14) * moodMult));
      Serial.println("Action: SlowBlink");
      break;

    case 3: // double blink
      blink(calmSpd(random(4, 8) * moodMult),
            random(40, 120),
            calmSpd(random(5, 10) * moodMult));
      delay(random(80, 180));
      blink(calmSpd(random(4, 8) * moodMult),
            random(40, 120),
            calmSpd(random(5, 10) * moodMult));
      Serial.println("Action: DoubleBlink");
      break;

    case 4: // partial squint — close partway, hold, reopen
      {
        int tOpen = topOpenPos();
        int bOpen = botOpenPos();
        // Midpoint between open and shut
        int tMid = (tOpen + TOP_LID_SHUT) / 2;
        int bMid = (bOpen + BOT_LID_SHUT) / 2;
        int spd  = calmSpd(random(5, 10) * moodMult);

        sweepDual(topLid, tOpen, tMid,
                  botLid, bOpen, bMid, spd);
        delay(holdTime);
        sweepDual(topLid, tMid, tOpen,
                  botLid, bMid, bOpen, spd);
        Serial.println("Action: Squint");
      }
      break;
  }
}

// ─────────────────────────────────────────────────────────────────────
//  Expression cycler
// ─────────────────────────────────────────────────────────────────────
void expressionCycler(int mood) {
  switch (mood) {
    case 1: // normal
      topMoodShift = 0;
      moodMult = 4;
      blink(calmSpd(12), 120, calmSpd(14));
      Serial.println("Mood: Normal");
      break;

    case 2: // surprised — wider open, quicker but still calm
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
