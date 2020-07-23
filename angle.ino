#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

int button = 2;
int red =11;
int green = 10;
int blue = 9;

int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50; 

int start_z_value = 0;
int start_y_value = 0;

bool calibrating = false;

void setup(void)
{
  pinMode(button, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  
 Serial.begin(9600);  Serial.println("Orientation Sensor Test"); Serial.println("");  /* Initialise the sensor */  if(!bno.begin())  {    /* There was a problem detecting the BNO055 ... check your connections */    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");    while(1);  }  delay(1000);  bno.setExtCrystalUse(true);

 
}

void loop(void)
{
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

   int reading = digitalRead(button);
   if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

   if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  //Serial.println(ledState);
  lastButtonState = reading;

  if(ledState == 0){
    if(calibrating){
      start_z_value = euler.z();
    
      start_y_value = euler.y();
    }
    
    calibrating = false;
    
  }
  if(ledState == 1){
     RGB_color(0, 255, 255); // Cyan
     calibrating = true;
  }
  if(!calibrating){
    if((euler.z() >= start_z_value + 80 && euler.z()  <= start_z_value + 100) &&
       (euler.y() >= start_y_value - 10 && euler.y()  <= start_y_value + 10)){
       RGB_color(0, 255, 0); // Green
    }
    else{
       RGB_color(255, 0, 0); // Red
    }
  }
  Serial.print(start_z_value);
  Serial.print(" ");
  Serial.print(euler.z());
  Serial.print("\t");
  Serial.print(start_y_value);
  Serial.print(" ");
  Serial.print(euler.y());
  Serial.println();
  
 /* RGB_color(255, 0, 0); // Red
  delay(500);
  RGB_color(0, 255, 0); // Green
  delay(500);
  RGB_color(0, 0, 255); // Blue
  delay(500);
  RGB_color(255, 255, 125); // Raspberry
  delay(500);
  RGB_color(0, 255, 255); // Cyan
  delay(500);
  RGB_color(255, 0, 255); // Magenta
  delay(500);
  RGB_color(255, 255, 0); // Yellow
  delay(500);
  RGB_color(255, 255, 255); // White
  delay(500);*/
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red, red_light_value);
  analogWrite(green, green_light_value);
  analogWrite(blue, blue_light_value);
}
