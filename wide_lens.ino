
#include "fabgl.h"

// initialize VGA controller
VGAController vga;

// initialize canvas
Canvas canvas(&vga);

// initialize gradient descent and binary search variables
int x = vga.getScreenWidth() / 2; // light gun x position
int y = vga.getScreenHeight() / 2; // light gun y position
int step = 10; // step size for moving the x and y position
int value = 0; // light intensity value at the current position
int bestValue = 0; // best light intensity value so far
int bestX = x; // best x position so far
int bestY = y; // best y position so far
int left = 0; // left boundary of the binary search range
int right = vga.getScreenWidth() - 1; // right boundary of the binary search range
int top = 0; // top boundary of the binary search range
int bottom = vga.getScreenHeight() - 1; // bottom boundary of the binary search range
int midX = (left + right) / 2; // mid point of the binary search range along x axis
int midY = (top + bottom) / 2; // mid point of the binary search range along y axis

// initialize light gun variables
const int triggerPin = 2; // GPIO pin for light gun trigger button
const int sensorPin = 34; // GPIO pin for light gun sensor
const int threshold = 100; // threshold value for light intensity detection
const int maxIterations = 10; // maximum number of gradient descent and binary search iterations

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
  canvas.drawLine(x - 5, y, x + 5, y);
  canvas.drawLine(x, y - 5, x, y + 5);
}

void loop() {
  // check if light gun trigger is pressed
  if (digitalRead(triggerPin) == LOW) {
    // perform gradient descent and binary search to find the x and y position of the light gun
    bool success = gradientDescentBinarySearch();

    // check if the search was successful
    if (success) {
      // draw a circle at the light gun position
      canvas.setPenColor(Color::Red);
      canvas.drawCircle(x, y, 10);
    } else {
      // draw an X at the middle of the screen to indicate failure
      canvas.setPenColor(Color::Red);
      canvas.drawLine(x - 5, y - 5, x + 5, y + 5);
      canvas.drawLine(x - 5, y + 5, x + 5, y - 5);
    }

    // reset the search variables
    x = vga.getScreenWidth() / 2;
    y = vga.getScreenHeight() / 2;
    step = 10;
    value = 0;
    bestValue = 0;
    bestX = x;
    bestY = y;
    left = 0;
    right = vga.getScreenWidth() -1 ;
    top =0 ;
    bottom = vga.getScreenHeight() -1 ;
    midX =( left + right )/2 ;
    midY =( top + bottom )/2 ;

    // wait for the trigger to be released before continuing
    while (digitalRead(triggerPin) == LOW) {
      delay(10);
    }
    
    // clear the screen
    canvas.clear();

    // redraw the crosshair at the middle of the screen
    canvas.setPenColor(Color::White);
    canvas.drawLine(x -5 ,y ,x +5 ,y );
    canvas.drawLine(x ,y -5 ,x ,y +5 );
    
   }
}

// function to perform gradient descent and binary search and return true if successful or false if failed 
bool gradientDescentBinarySearch() {
  
   // initialize iteration counter 
   int iteration =0 ;

   while (true) {
     // increment iteration counter 
     iteration++;

     // check if iteration counter exceeds maximum iterations or trigger is released 
     if (iteration > maxIterations || digitalRead(triggerPin) == HIGH) {
       // return false to indicate failure 
       return false;
     }

     // draw a rectangle at the current position with the step size as half width and height 
     canvas.setPenColor(Color::White);
     canvas.drawRect(x - step /2 ,y - step /2 ,x + step /2 ,y + step /2 );

     // wait for the next frame 
     vga.waitSync();

     // read the analog value of the light gun sensor at the current position 
     value = analogRead(sensorPin);

     // clear the rectangle 
     canvas.setPenColor(Color::Black);
     canvas.drawRect(x - step /2 ,y - step /2 ,x + step /2 ,y + step /2 );

     // compare the value with a threshold value 
     if (value > threshold) {
       // check if the value is better than the best value so far 
       if (value > bestValue) {
         // update the best value and position 
         bestValue = value;
         bestX = x;
         bestY = y;
       }

       // check if the value is close to the maximum possible value 
       if (value >4000 ) {
         // return true to indicate success 
         return true; 
       }

       // move the position in the direction of increasing light intensity using binary search 
       
       // get the mid point of the current range along x axis 
       midX =( left + right )/2 ;

       // draw a vertical line at the mid point along x axis 
       canvas.setPenColor(Color::White);
       canvas.drawLine(midX ,0 ,midX ,vga.getScreenHeight() -1 );

       // wait for the next frame 
       vga.waitSync();

       // read the analog value of the light gun sensor at the mid point along x axis and current y position 
       int valueX = analogRead(sensorPin);

       // clear the vertical line 
       canvas.setPenColor(Color::Black);
       canvas.drawLine(midX ,0 ,midX ,vga.getScreenHeight() -1 );

       // compare the valueX with a threshold value 
       if (valueX > threshold) {
         // check if the valueX is better than the best value so far 
         if (valueX > bestValue) {
           // update the best value and position along x axis 
           bestValue = valueX;
           bestX = midX;
         }

         // check if the valueX is close to the maximum possible value 
         if (valueX >4000 ) {
           // return true to indicate success 
           return true; 
         }

         // move the position along x axis in the direction of increasing light intensity 
         if (vga.HSync()) {
           // move right 
           left = midX +1 ;
         } else {
           // move left 
           right = midX -1 ;
         }
         
       } else {
         // reduce the step size by half along x axis 
         step /=2 ;

         // check if the step size is too small along x axis 
         if (step <1 ) {
           break;
         }

         // move randomly by step size along x axis within current range  
         x += random(-step ,step );
         x= constrain(x ,left ,right );
         
       }

       // repeat same process for y axis

       // get the mid point of the current range along y axis 
       midY =( top + bottom )/2 ;

       // draw a horizontal line at the mid point along y axis 
       canvas.setPenColor(Color::White);
       canvas.drawLine(0 ,midY ,vga.getScreenWidth() -1 ,midY );

       // wait for the next frame 
       vga.waitSync();

       // read the analog value of the light gun sensor at the current x position and mid point along y axis 
       int valueY = analogRead(sensorPin);

       // clear the horizontal line 
       canvas.setPenColor(Color::Black);
       canvas.drawLine(0 ,midY ,vga.getScreenWidth() -1 ,midY );

       // compare the valueY with a threshold value 
       if (valueY > threshold) {
         // check if the valueY is better than the best value so far 
         if (valueY > bestValue) {
           // update the best value and position along y axis 
           bestValue = valueY;
           bestY = midY;
         }

         // check if the valueY is close to the maximum possible value 
         if (valueY >4000 ) {
           // return true to indicate success 
           return true; 
         }

         // move the position along y axis in the direction of increasing light intensity 
         if (vga.scanLine() < midY) {
           // move up 
           bottom = midY -1 ;
         } else {
           // move down 
           top = midY +1 ;
         }
         
       } else {
         // reduce the step size by half along y axis 
         step /=2 ;

         // check if the step size is too small along y axis 
         if (step <1 ) {
           break;
         }

         // move randomly by step size along y axis within current range  
         y += random(-step ,step );
         y= constrain(y ,top ,bottom );
         
       }
       
     } else {
       // reduce the step size by half 
       step /=2 ;

       // check if the step size is too small 
       if (step <1 ) {
         break;
       }

       // move randomly by step size within current range  
       x += random(-step ,step );
       y += random(-step ,step );
       x= constrain(x ,left ,right );
       y= constrain(y ,top ,bottom );
       
     }
     
   }

   // check if there is a best value found 
   if (bestValue >0 ) {
     // set the position to the best position found 
     x = bestX;
     y = bestY;

     // return true to indicate success 
     return true; 
   } else {
     // return false to indicate failure 
     return false; 
   }
   
}
