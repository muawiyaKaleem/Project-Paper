
// Define Ultrasonic Sensor Pins
const int trigPin1 = 5, echoPin1 = 6;  // Sensor S1 (Step 1) - 3 inches (~7.5 cm)
const int trigPin2 = 7, echoPin2 = 8;  // Sensor S2 (Step 2) - 9.5 inches (~24 cm)
const int trigPin3 = 9, echoPin3 = 10; // Sensor S3 (Step 3) - 15.5 inches (~39 cm)

// Water Sensor Pin
const int waterSensorPin = A0;

// Buzzer and Vibration Motor Pins
const int buzzer = 11;
const int vibrationMotor = 12; // Connected via transistor

// Constants
const int safeDistance = 100;  // Safe threshold in cm
const int stepRise = 17;       // Updated expected step rise in cm (~6.5-7 inches)
const int stepRun = 26;        // Expected step run in cm (~10-11 inches)
const int tolerance = 3;       // Allowed sensor variation (±3 cm for ultrasonic errors)
const int waterThreshold = 300; // Water sensor calibration value

// Function to Measure Distance
long measureDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    //delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    //delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH);
    long distance = duration * 0.0343 / 2; // Convert to cm

    // Return safe max if invalid reading
    if (distance <= 0) {
        return safeDistance + 1;
    }
    return distance;
}

// Function to Detect Steps and Obstacles
void detectStepsAndObstacles() {
    long D1 = measureDistance(trigPin1, echoPin1);  // Sensor 1
    long D2 = measureDistance(trigPin2, echoPin2);  // Sensor 2
    long D3 = measureDistance(trigPin3, echoPin3);  // Sensor 3

    Serial.print("S1: "); Serial.print(D1); Serial.print(" cm, ");
    Serial.print("S2: "); Serial.print(D2); Serial.print(" cm, ");
    Serial.print("S3: "); Serial.println(D3);

    // Ignore invalid readings
    if (D1 <= 0 || D2 <= 0 || D3 <= 0) {
        Serial.println("Invalid sensor readings detected.");
        return;
    }

    // Safety Alert for Nearby Obstacles (keep vibration and buzzer ON)
    if (D1 < safeDistance || D2 < safeDistance || D3 < safeDistance) {
        Serial.println("Warning: Object detected within unsafe range!");
        digitalWrite(vibrationMotor, HIGH);  // Continuous vibration
        tone(buzzer, 1000);  // Continuous buzzer sound
        
    }

    // Case 1: Large Obstacle (Wall) - All sensors detect close
    if (abs(D1 - D2) <= tolerance && abs(D2 - D3) <= tolerance && D1 < safeDistance) {
        Serial.println("Obstacle Detected (Wall)!");
        digitalWrite(vibrationMotor, HIGH);  // Continuous vibration
        tone(buzzer, 1000);  // Continuous buzzer sound
        return;
    }

    // Case 2: Low Object - Only S1 detects close, others are far
    if (D1 < safeDistance && D2 > safeDistance && D3 > safeDistance) {
        Serial.println("Low Object Detected (Curb, Small Rock)");
        digitalWrite(vibrationMotor, HIGH);  // Continuous vibration
        tone(buzzer, 1200);  // Continuous buzzer sound
        return;
    }

    // Case 3: Mid-Level Object - S1 and S2 close, S3 far
    if (D1 < safeDistance && D2 < safeDistance && D3 > safeDistance) {
        Serial.println("Mid-Level Obstacle Detected (Bench, Table)");
        digitalWrite(vibrationMotor, HIGH);  // Continuous vibration
        tone(buzzer, 1400);  // Continuous buzzer sound
        return;
    }

    // Case 4: Knee-High Object - Only S2 detects close, others are far
    if (D1 > safeDistance && D2 < safeDistance && D3 > safeDistance) {
        Serial.println("Knee-High Object Detected (Chair, Railing)");
        digitalWrite(vibrationMotor, HIGH);  // Continuous vibration
        tone(buzzer, 1600);  // Continuous buzzer sound
        return;
    }

    // Case 5: Hanging Object - Only S3 detects close, others are far
    if (D1 > safeDistance && D2 > safeDistance && D3 < safeDistance) {
        Serial.println("Hanging Object Detected (Tree Branch, Sign)");
        digitalWrite(vibrationMotor, HIGH);  // Continuous vibration
        tone(buzzer, 1800);  // Continuous buzzer sound
        return;
    }

 

    // No object detected, turn off buzzer and vibration motor
    digitalWrite(vibrationMotor, LOW);
    noTone(buzzer);
    Serial.println("No object detected.");
}

// Function to check for water hazards - Buzzer only (no vibration)
void checkWater() {
    int waterLevel = analogRead(waterSensorPin);  // Read water sensor value
    if (waterLevel > waterThreshold) {  // If water level exceeds threshold
        Serial.println("Water Detected! Avoid stepping!");
        tone(buzzer, 1200);  // High-pitched sound for water hazard
        // No vibration motor here for water detection
        while (waterLevel > waterThreshold) {  // Keep checking until water level decreases
            delay(500);  // Continue alerting while water is detected
            waterLevel = analogRead(waterSensorPin);  // Recheck water level
        }
        noTone(buzzer);  // Stop buzzer once water level goes back down
    }
}

void setup() {
    Serial.begin(9600);  // Initialize Serial Monitor
    
    // Define Pins
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);

    pinMode(waterSensorPin, INPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(vibrationMotor, OUTPUT);
}

void loop() {
    detectStepsAndObstacles();  // Detect steps and obstacles
    checkWater();  // Check for water hazard
    delay(1000);  // Delay to avoid sensor misreads
}
