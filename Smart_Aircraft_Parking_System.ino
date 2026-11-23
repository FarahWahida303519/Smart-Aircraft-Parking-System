
// ======================================
// Library
// ======================================

#include <Wire.h>
#include <Adafruit_GFX.h> //to draw lines,shape
#include <Adafruit_SSD1306.h> //display information on the oled

// ======================================
// OLED SETTINGS the part
// ======================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C //defines the I2C address of the OLED(SDA AND SDCL (COMMUNICATION Line)  )

// Createand configure the OLED display object
Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1
);


// ======================================
// ESP32 PIN CONNECTIONS DEFINE ALL SENSOR FIRST
// ======================================

#define TRIG_PIN 5
#define ECHO_PIN 18
#define IR_SENSOR 19
#define TOUCH_PIN 27

// ======================================
// DEFINE THE  VARIABLES
// ======================================

float distanceCm = -1; // No valid distance reading(no signal)
int sensorState = HIGH; //1 == status no obstacle

int currentPage = 1;

const int TOUCH_THRESHOLD = 50; //recognise user touch male pin. below 50==touch detect
const int TOUCH_RELEASE_THRESHOLD = 65;//Detects when the user releases the pin. above 65== release pin

bool touchArmed = true;//determines whether the system is ready to detect a new touch
bool touchTiming = false;

unsigned long touchStartTime = 0; //Records when touch begins(3sec)
unsigned long lastTouchTime = 0;//Records previous touch activation
unsigned long lastSensorTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long lastSerialTime = 0;

const unsigned long TOUCH_HOLD_TIME = 3000; //hold pin for 3 sec to open page2
const unsigned long DEBOUNCE_TIME = 500;//Prevents rapid repeated touch activation
const unsigned long SENSOR_INTERVAL = 200;//ontrols how often the ESP32 reads the sensor
const unsigned long DISPLAY_INTERVAL = 200;//Controls how often the OLED refresh
const unsigned long SERIAL_INTERVAL = 1000;//serial monitor

// ======================================
// DRAW SMALL AIRPLANE OPENING  (Style )
// ======================================

void drawAirplane(int x, int y) {

  // This for  body Aircraft
  display.drawLine(x + 10, y, x + 10, y + 16, SSD1306_WHITE);

  //  wings
  display.drawLine(x + 10, y + 5, x, y + 10, SSD1306_WHITE);
  display.drawLine(x + 10, y + 5, x + 20, y + 10, SSD1306_WHITE);

  //  tail
  display.drawLine(x + 10, y + 13, x + 4, y + 17, SSD1306_WHITE);
  display.drawLine(x + 10, y + 13, x + 16, y + 17, SSD1306_WHITE);
}

// ======================================
// OPENING SCREEN
// ======================================

void showStartup() {

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Airport title
  display.setTextSize(1);

  display.setCursor(13, 3);
  display.println("AIRPORT GROUND");

  display.setCursor(42, 13);
  display.println("CONTROL");

  // Aircraft draw
  drawAirplane(54, 25);

  // System name
  display.setTextSize(2);
  display.setCursor(16, 43);
  display.println("AERO PARK");

  display.display();

  delay(2000);

  // System ready screen
  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(25, 15);
  display.println("SYSTEM READY");

  display.drawLine(10, 28, 118, 28, SSD1306_WHITE);

  display.setCursor(19, 39);
  display.println("BAY A-01 ONLINE");

  display.display();

  delay(1000);
}

// ======================================
// READ HC-SR04 DISTANCE
// ======================================

//function used to measure the distance between the sensor  sensor and an object
float readDistance() {

  //set trig pin to low for 2micrsec
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  //send ultrasonic trigger pulse
  //set trig pin to high 10microsec to active sensor
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);//set trig to low after 10 microsec

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);//measure how log dgital signal (time taken by the ultrasonic wave to travel to the object and return depends on the distance)

  // Return -1 when no echo is received
  //check sensor receive valid echo pulse within timeout
  if (duration == 0) {
    return -1;//If no echo is received return -1
  }

  // Calculate the distance in centimetres and return the result
  return duration * 0.0343 / 2.0;
}

// ======================================
// READ BOTH SENSORS Function
// ======================================

void readSensors() {

  distanceCm = readDistance();//hc sensor

  sensorState = digitalRead(IR_SENSOR);//fc sensor
}

// ======================================
// DETERMINE PARKING STATUS (FC Sensor)
// FC-51 LOW = OCCUPIED
// FC-51 HIGH = AVAILABLE
// ======================================

bool isOccupied() {

  return sensorState == LOW; //detect Obj
}

// ======================================
// AIRCRAFT DISTANCE STATUS
//function for assigned the status based on the distance then  call this function later
// ======================================

const char* getDistanceStatus() {

  if (distanceCm < 0) {
    return "NO SIGNAL";
  }

  if (distanceCm > 100) {
    return "AIRCRAFT INBOUND";
  }

  if (distanceCm > 5) {
    return "LANDING APPROACH";
  }

  if (distanceCm > 0) {
    return "AIRCRAFT ARRIVED - STOP";
  }

  return "AIRCRAFT ARRIVED";
}

// ======================================
// PAGE 1 - AIRCRAFT PARKING STATUS Module
// ======================================

void showPage1() {

  //Clear the OLED and set text colour
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // System heading
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("AERO PARK CONTROL");

  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // Flight information and aircraft
  drawAirplane(2, 15);

  display.setCursor(27, 15);
  display.println("FLIGHT MH102");

  display.setCursor(27, 26);
  display.println("AIRCRAFT ARRIVAL");

  display.drawLine(0, 36, 127, 36, SSD1306_WHITE);


  // Display the current parking status by call method isOccupied()
  display.setTextSize(2);

  if (isOccupied()) {

    display.setCursor(16, 39);
    display.println("OCCUPIED");

  } else {

    display.setCursor(10, 39);
    display.println("AVAILABLE");

  }

  // This part for Display parking bay number
  display.setTextSize(1);
  display.setCursor(37, 57);
  display.println("PARKING BAY A-01");

  display.display();
}

// ======================================
// PAGE 2 - AIRCRAFT LANDING MONITOR Module
// ======================================

void showPage2() {

  //Clear the OLED and set text colour
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // System heading
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("AERO PARK CONTROL");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // Page heading
  display.setCursor(20, 14);
  display.println("AIRCRAFT MONITOR");
  display.setCursor(0, 26);
  display.println("LANDING DISTANCE");

  // Display actual distance reading
  display.setTextSize(2);

  if (distanceCm < 0) {

    display.setCursor(28, 36);
    display.println("NO SIGNAL");

  } else {

    display.setCursor(22, 36);
    display.print(distanceCm, 0);
    display.println(" cm");

  }

  // Display aircraft approach status
  display.setTextSize(1);
  display.setCursor(0, 55);

  display.println(getDistanceStatus());// call the function getDistanceStatus to get the status distance

  display.display();
}

// ======================================
// CAPACITIVE TOUCH NAVIGATION
// ======================================

void handleTouch(int touchValue) {

  unsigned long currentTime = millis();

  if (touchArmed) {//true==detect new touch.false==waiting release pin

    if (touchValue < TOUCH_THRESHOLD) { //touch_thredshold == 50

      if (!touchTiming) {// Check whether touch timing has started. false ==system not start timing touch

        touchStartTime = currentTime;
        touchTiming = true;

        Serial.println("TOUCH STARTED - HOLD FOR 3 SECONDS");
      }

      // Change page after press male pin  3 seconds
      if (currentTime - touchStartTime >= TOUCH_HOLD_TIME && // ensures the user holds the pin for 3 seconds
          currentTime - lastTouchTime >= DEBOUNCE_TIME) {//prevents rapid repeated activation

        currentPage = (currentPage == 1) ? 2 : 1; //change pages

        touchArmed = false;//Temporarily disables new touch detection until the user releases the pin
        touchTiming = false;//Resets the touch timing state

        lastTouchTime = currentTime;

        Serial.print("PAGE CHANGED TO: ");
        Serial.println(currentPage);
      }

    } else if (touchValue > TOUCH_RELEASE_THRESHOLD) {//check when use release pin

      touchTiming = false;
      touchStartTime = 0;
    }

  } else {

    // Wait until the touch pin is release
    if (touchValue > TOUCH_RELEASE_THRESHOLD) {

      touchArmed = true; //allow new touch
      touchTiming = false; //reset timer
      touchStartTime = 0;

      Serial.println("TOUCH RELEASED - READY");
    }
  }
}

// ======================================
// SERIAL MONITOR
// ======================================

void printSerialData(int touchValue) {

  Serial.println("--------------------------------");
  Serial.println("AERO PARK GROUND CONTROL");

  Serial.println("FLIGHT: MH102");
  Serial.println("PARKING BAY: A-01");

  Serial.print("PARKING STATUS: ");

  if (isOccupied()) {
    Serial.println("OCCUPIED");
  } else {
    Serial.println("AVAILABLE");
  }

  Serial.print("AIRCRAFT DISTANCE: ");

  if (distanceCm < 0) {
    Serial.println("NO SIGNAL");
  } else {
    Serial.print(distanceCm, 1);
    Serial.println(" cm");
  }

  Serial.print("AIRCRAFT STATUS: ");

  Serial.println(getDistanceStatus());

  Serial.print("FC-51 SENSOR: ");

  if (isOccupied()) {
    Serial.println("OBSTACLE DETECTED");
  } else {
    Serial.println("CLEAR");
  }

  Serial.print("TOUCH VALUE: ");
  Serial.println(touchValue);

  Serial.print("DISPLAY PAGE: ");
  Serial.println(currentPage);
}

// ======================================
// SETUP FOR OLED PART (prepares the ESP32, sensors and OLED)
// ======================================

void setup() {

  Serial.begin(115200);

//configure sensor pin
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_SENSOR, INPUT);

  // Start OLED communication
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {//Check whether the OLED is detected

    Serial.println("OLED NOT DETECTED!");

    while (true) {
      delay(10);
    }
  }

  display.clearDisplay();
  display.display();

  // Show the airport welcome screen
  showStartup();

  // Read initial sensor values
  readSensors();

  Serial.println("==============================");
  Serial.println("AERO PARK SYSTEM STARTED");
  Serial.println("FLIGHT: MH102");
  Serial.println("PARKING BAY: A-01");
  Serial.println("HOLD TOUCH PIN FOR 3 SECONDS");
  Serial.println("==============================");

  showPage1();
}

// ======================================
// MAIN LOOP
// ======================================
//Run and monitor the system continuously for this function
void loop() {

  unsigned long currentTime = millis();

  // Update sensor readings regularly
  if (currentTime - lastSensorTime >= SENSOR_INTERVAL) {

    lastSensorTime = currentTime;

    readSensors();
  }

  // Read capacitive touch
  int touchValue = touchRead(TOUCH_PIN);

  handleTouch(touchValue);

  // Refresh the OLED display
  if (currentTime - lastDisplayTime >= DISPLAY_INTERVAL) {

    lastDisplayTime = currentTime;

    if (currentPage == 1) {
      showPage1();
    } else {
      showPage2();
    }
  }

  // Update Serial Monitor every second
  if (currentTime - lastSerialTime >= SERIAL_INTERVAL) {

    lastSerialTime = currentTime;

    printSerialData(touchValue);
  }

  delay(20);
}
