
#include "fabgl.h"

// initialize VGA controller
VGAController vga;

// initialize canvas
Canvas canvas(&vga);

// initialize binary search variables
int left = 0;
int right = vga.getScreenWidth() - 1;
int top = 0;
int bottom = vga.getScreenHeight() - 1;
int midX = (left + right) / 2;
int midY = (top + bottom) / 2;
int x = -1; // light gun x position
int y = -1; // light gun y position

// initialize light gun variables
const int triggerPin = 2; // GPIO pin for light gun trigger button
const int sensorPin = 34; // GPIO pin for light gun sensor
const int threshold = 100; // threshold value for light intensity detection
const int maxIterations = 10; // maximum number of binary search iterations

void setup() {
  // set VGA mode
  vga.begin(VGA_640x480_60Hz);

  // set trigger pin as input with pull-up resistor
  pinMode(triggerPin, INPUT_PULLUP);

  // set sensor pin as input
  pinMode(sensorPin, INPUT);

  // clear canvas
  canvas.clear();

  // draw a crosshair at the middle of the screen
  canvas.setPenColor(Color::White);
  canvas.drawLine(midX - 5, midY, midX + 5, midY);
  canvas.drawLine(midX, midY - 5, midX, midY + 5);
}

void loop() {
  // check if light gun trigger is pressed
  if (digitalRead(triggerPin) == LOW) {
    // perform binary search to find the x and y position of the light gun
    bool success = binarySearch();

    // check if the binary search was successful
    if (success) {
      // draw a circle at the light gun position
      canvas.setPenColor(Color::Red);
      canvas.drawCircle(x, y, 10);
    } else {
      // draw an X at the middle of the screen to indicate failure
      canvas.setPenColor(Color::Red);
      canvas.drawLine(midX - 5, midY - 5, midX + 5, midY + 5);
      canvas.drawLine(midX - 5, midY + 5, midX + 5, midY - 5);
    }

    // reset the binary search variables
    left = 0;
    right = vga.getScreenWidth() - 1;
    top = 0;
    bottom = vga.getScreenHeight() - 1;
    midX = (left + right) / 2;
    midY = (top + bottom) / 2;
    x = -1;
    y = -1;
  }
}

// function to perform binary search and return true if successful or false if failed
bool binarySearch() {
  
  // initialize iteration counter
  int iteration = 0;

  while (left <= right && top <= bottom) {
    // increment iteration counter
    iteration++;

    // check if iteration counter exceeds maximum iterations or trigger is released
    if (iteration > maxIterations || digitalRead(triggerPin) == HIGH) {
      // return false to indicate failure
      return false;
    }

    // get the mid point of the current range
    midX = (left + right) / 2;
    midY = (top + bottom) / 2;

    // draw a rectangle at the current range
    canvas.setPenColor(Color::White);
    canvas.drawRect(left, top, right, bottom);

    // wait for the next frame
    vga.waitSync();

    // read the analog value of the light gun sensor at the mid point
    int value = analogRead(sensorPin);

    // clear the rectangle
    canvas.setPenColor(Color::Black);
    canvas.drawRect(left, top, right, bottom);

    // compare the value with a threshold value
    if (value > threshold) {
      // the light gun is pointing to one of the four quadrants around the mid point
      if (vga.scanLine() < midY) {
        // the light gun is pointing to the upper half of the screen
        bottom = midY - 1;
      } else {
        // the light gun is pointing to the lower half of the screen
        top = midY + 1;
      }
      if (vga.HSync()) {
        // the light gun is pointing to the right half of the screen
        left = midX + 1;
      } else {
        // the light gun is pointing to the left half of the screen
        right = midX - 1;
      }
    } else {
      // the light gun is pointing to the mid point or close to it
      x = midX;
      y = midY;

      // return true to indicate success
      return true;
    }
    
   }

   // return false to indicate failure 
   return false; 
}
