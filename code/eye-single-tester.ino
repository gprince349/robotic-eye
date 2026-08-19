/**********************************************************************
                     Single-Eye Lid Tester
  Animatronic single-eye mechanism using 2 SG90 micro-servos driven
  directly from Arduino Nano pins via the Servo library.

  Controls only:
    - 1 top eyelid servo
    - 1 bottom eyelid servo

  No X/Y eyeball movement. No PWM board required.
 **********************************************************************/

#include <Servo.h>

// ─────────────────────────────────────────────────────────────────────
//  Pin assignments  (Arduino Nano)
// ─────────────────────────────────────────────────────────────────────
#define PIN_TOP_LID  5    // Top eyelid
#define PIN_BOT_LID  6    // Bottom eyelid

// ─────────────────────────────────────────────────────────────────────
//  Servo objects
// ─────────────────────────────────────────────────────────────────────
Servo topLid;
Servo botLid;

// ─────────────────────────────────────────────────────────────────────
//  Calibration — tweak these to match your physical build
//  All values are in degrees (0–180)
// ─────────────────────────────────────────────────────────────────────

// Top eyelid
#define TOP_LID_OPEN   60     // Degrees when fully open
#define TOP_LID_SHUT   120    // Degrees when fully closed

// Bottom eyelid (typically inverted vs. top)
#define BOT_LID_OPEN   120    // Degrees when fully open
#define BOT_LID_SHUT   80     // Degrees when fully closed

// ─────────────────────────────────────────────────────────────────────
//  Mood / behavior state
// ─────────────────────────────────────────────────────────────────────
int topMoodShift = 0;          // Offsets the eyelid open position (+open, -squint)

// ─────────────────────────────────────────────────────────────────────
//  Setup
// ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("Attaching lid servos...");

  topLid.attach(PIN_TOP_LID);
  botLid.attach(PIN_BOT_LID);

  // Move lids to open position at startup
  topLid.write(TOP_LID_OPEN);
  botLid.write(BOT_LID_OPEN);

  delay(1000);   // Let servos reach start position before anything else runs

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
  sweepDual(topLid, TOP_LID_OPEN + topMoodShift, TOP_LID_SHUT,
            botLid, BOT_LID_OPEN + topMoodShift, BOT_LID_SHUT,
            spd);
}

void openEye(int spd) {
  sweepDual(topLid, TOP_LID_SHUT, TOP_LID_OPEN + topMoodShift,
            botLid, BOT_LID_SHUT, BOT_LID_OPEN + topMoodShift,
            spd);
}
