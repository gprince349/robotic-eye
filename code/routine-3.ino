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
   blink(10, 100, 40);
 
   delay(500);
 
   lookLeftLim(5 * moodMult);
   delay(300);
   lookRightMid(5 * moodMult);
   delay(300);
   lookRightLim(5 * moodMult);
   delay(300);
   lookLeftMid(5 * moodMult);
 
   delay(500);
 
   lookUpLim(5 * moodMult);
   delay(300);
   lookDownMid(5 * moodMult);
   delay(300);
   lookDownLim(5 * moodMult);
   delay(300);
   lookUpMid(5 * moodMult);
 
   delay(500);
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
 