// -------- PIN DEFINITIONS (Your exact wiring) --------
#define LDR_PIN A0
#define PIEZO1_PIN A1
#define PIEZO2_PIN A2
#define LED_PWM_PIN A3     // MOSFET Gate
#define RADAR_PIN 2        // Doppler Radar OUT pin

// -------- SETTINGS (adjust during testing) --------
int nightThreshold = 600;     // Set after LDR calibration
int normalBrightness = 100;   // Base brightness at night
int motionBrightness = 255;   // Brightness when radar detects motion
int motionTime = 10000;        // 10s boost

unsigned long motionTimer = 0;
bool radarTriggered = false;

void setup() {
  Serial.begin(9600);
  pinMode(RADAR_PIN, INPUT);
  pinMode(LED_PWM_PIN, OUTPUT);
}

void loop() {

  // ---- READ SENSORS ----
  int ldrValue = analogRead(LDR_PIN);
  int pz1 = analogRead(PIEZO1_PIN);
  int pz2 = analogRead(PIEZO2_PIN);
  int radar = digitalRead(RADAR_PIN);

  // Debug output
  Serial.print("LDR: "); Serial.print(ldrValue);
  Serial.print(" | Piezo1: "); Serial.print(pz1);
  Serial.print(" | Piezo2: "); Serial.print(pz2);
  Serial.print(" | Radar: "); Serial.println(radar);

  // ---- PIEZO IMPACT DETECTION (optional logic) ----
  bool heavyImpact = (pz1 > 80 || pz2 > 80);  // tweak threshold

  // ---- NIGHT MODE LOGIC ----
  bool isNight = (ldrValue < nightThreshold);

  int brightness = 0;

  if (isNight) {

    brightness = normalBrightness;

    // RADAR OR PIEZO TRIGGER → BOOST BRIGHTNESS
    if (radar == HIGH || heavyImpact) {
      radarTriggered = true;
      motionTimer = millis();
    }

    if (radarTriggered) {
      brightness = motionBrightness;
      if (millis() - motionTimer >= motionTime) {
        radarTriggered = false;
      }
    }
  } 
  else {
    // DAYTIME → LIGHT OFF
    brightness = 0;
  }

  // ---- OUTPUT TO LED DRIVER (MOSFET) ----
  analogWrite(LED_PWM_PIN, brightness);

  delay(100);
}
