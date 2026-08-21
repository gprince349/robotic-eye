/**********************************************************************
                     Lid-Only Tester (Left + Right)
  Drives only the left and right top eyelid servos.
  No X/Y eyeball movement. No PWM board required.
 **********************************************************************/

#include <Servo.h>

// ─────────────────────────────────────────────────────────────────────
//  Pin assignments  (same lid pins as eye-tester)
// ─────────────────────────────────────────────────────────────────────
#define PIN_L_TOP_LID  5    // Left  top eyelid
#define PIN_R_TOP_LID  8    // Right top eyelid

// ─────────────────────────────────────────────────────────────────────
//  Servo objects
// ─────────────────────────────────────────────────────────────────────
Servo LtopLid;
Servo RtopLid;

// ─────────────────────────────────────────────────────────────────────
//  Calibration — tweak these to match your physical build
//  All values are in degrees (0–180)
// ─────────────────────────────────────────────────────────────────────
#define L_LID_OPEN   60     // Left  — degrees when fully open
#define L_LID_SHUT   120    // Left  — degrees when fully closed

#define R_LID_OPEN   120    // Right — degrees when fully open (mirrored)
#define R_LID_SHUT   60     // Right — degrees when fully closed

// ─────────────────────────────────────────────────────────────────────
//  Mood / behavior state
// ─────────────────────────────────────────────────────────────────────
int topMoodShift = 0;          // Offsets the eyelid open position

// ─────────────────────────────────────────────────────────────────────
//  Setup
// ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("Attaching lid servos...");

  LtopLid.attach(PIN_L_TOP_LID);
  RtopLid.attach(PIN_R_TOP_LID);

  LtopLid.write(L_LID_OPEN);
  RtopLid.write(R_LID_OPEN);

  delay(1000);

  Serial.println("Ready!");
}

// ─────────────────────────────────────────────────────────────────────
//  Loop — blink only
// ─────────────────────────────────────────────────────────────────────
void loop() {
  blink(10, 100, 40);
  delay(1000);
}

// ─────────────────────────────────────────────────────────────────────
//  Internal helper — move two servos simultaneously, 1° per step
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
