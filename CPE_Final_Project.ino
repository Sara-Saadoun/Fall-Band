#include "Adafruit_CircuitPlayground.h"

#define N 5 // higher N = less sensitivity
bool alarmOn = false;

// Arrays to store acceleration parameters
float x_values[N];
float y_values[N];
float z_values[N];
int counter = 0;

void setup() 
{
  // Initializing all values in array to 0
  for (int i = 0; i < N; i++) {
    x_values[i] = 0;
    y_values[i] = 0;
    z_values[i] = 0;
  }
  
  CircuitPlayground.begin(); // Initialize the Circuit Playground library
  CircuitPlayground.strip.clear(); //Clear LEd light strip
  CircuitPlayground.strip.show(); // Update LED strip
}

void loop() 
{
  // Check if the right button is pressed to start the alarm
  if (CircuitPlayground.rightButton() && !alarmOn) {
    alarmOn = true;
    soundAlarm(); // Start sounding the alarm when right button is pressed
  }

  // Fall detection and alarm using built-in accelerometer functions
  if (isFallDetected() && !alarmOn) {
    alarmOn = true;
    soundAlarm(); // Start sounding the alarm when fall is detected
  }

  // Check if the left button is pressed to stop the alarm
  if (alarmOn && CircuitPlayground.leftButton()) {
    stopAlarm();
  }
}

// Stores most recent reading from the accelerometer in the buffer at position "counter"
void updateBuffer(float x, float y, float z)
{
  x_values[counter] = x;
  y_values[counter] = y;
  z_values[counter] = z;
  counter = (counter + 1) % N; // Increment the counter and wrap it around when it reaches N
}

float calculateAverage(float arr[])
{
  float sum = 0;
  // Add each element of the array to sum
  for (int i = 0; i < N; i++) {
    sum += arr[i];
  }
  // Calculate and return the average
  return sum / 5;
}

// Function to detect a fall based on accelerometer readings
bool isFallDetected() 
{
  // Getting x, y, and z parameters from accelerometer
  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  float z = CircuitPlayground.motionZ();

  updateBuffer(x, y, z); // Updating values in buffer

  // Finding average of x, y, and z values collected
  float avg_x = calculateAverage(x_values);
  float avg_y = calculateAverage(y_values);
  float avg_z = calculateAverage(z_values);

  // Calculate the magnitude of the acceleration vector
  float magnitude = sqrt(avg_x * avg_x + avg_y * avg_y + avg_z * avg_z);
  
  // Return true if a fall is detected
  return (magnitude < 0.981); // Threshold can be adjusted for fall sensitivity
}

// Sound the alarm by turning on the LEDs and playing a tone in a loop
void soundAlarm() 
{
  Serial.println("Alarm triggered! Sounding alarm...");
  
  // Turn on red LEDs to signal alarm
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0); // Red color
  }
  CircuitPlayground.strip.show(); // Update LED strip
  
  // Keep sounding the alarm in a loop until manually stopped
  while (alarmOn) {
    // Play a tone to signal the alarm
    CircuitPlayground.playTone(1000, 100); // Play tone for 100 ms
    delay(100); // Short pause between beeps
    
    // Check if the left button is pressed to stop the alarm
    if (CircuitPlayground.leftButton()) {
      stopAlarm();
      break;
    }
  }
}

// Stop the alarm by turning off the LEDs and stopping the sound
void stopAlarm() 
{
  Serial.println("Alarm stopped.");
  
  // Turn off the LEDs
  CircuitPlayground.clearPixels(); // Clear LED strip
  CircuitPlayground.strip.show(); // Update LED strip
  
  // Reset the flag
  alarmOn = false;
}

