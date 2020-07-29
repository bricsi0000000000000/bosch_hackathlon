#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

unsigned int button_pin = 12;
unsigned int RGB_red_led_pin = 9;
unsigned int RGB_green_led_pin = 10;
unsigned int RGB_blue_led_pin = 11;
unsigned int right_led_pin = 7;
unsigned int forward_led_pin = 4;
unsigned int back_led_pin = 8;
unsigned int left_led_pin = 2;

int reading;

unsigned int led_state = HIGH;        
unsigned int button_state;            
unsigned int last_button_state = LOW;
unsigned long last_debounce_time = 0; 
unsigned long debounce_delay = 50;

int start_x_value = 0;
int start_y_value = 0;

bool calibrating = false;

enum Color {Red, Green, Blue, Nothing};

void setup()
{  
  pinMode(button_pin, INPUT);
  pinMode(RGB_red_led_pin, OUTPUT);
  pinMode(RGB_green_led_pin, OUTPUT);
  pinMode(RGB_blue_led_pin, OUTPUT);
  pinMode(forward_led_pin, OUTPUT);
  pinMode(right_led_pin, OUTPUT);
  pinMode(back_led_pin, OUTPUT);
  pinMode(left_led_pin, OUTPUT);
  
  Serial.begin(9600);  
  Serial.println("Orientation Sensor Test"); 
  Serial.println("");
  if (!bno.begin())  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  
  delay(1000);  
  bno.setExtCrystalUse(true);
}
void loop()
{
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  if (euler.x() < 0)
    {
      euler.x() = -(180 - fabsf(euler.x()));
    }
    else
    {
      euler.x() = 180 - fabsf(euler.x());
    }
  int reading = digitalRead(button_pin);
  if (reading != last_button_state) {
    last_debounce_time = millis();
  }
  if ((millis() - last_debounce_time) > debounce_delay) {
    if (reading != button_state) {
      button_state = reading;
      if (button_state == HIGH) {
        led_state = !led_state;
      }
    }
  }
  last_button_state = reading;
  if (led_state == 0) {
    if (calibrating) {
      start_x_value = euler.x();
      start_y_value = euler.y();
    }
    calibrating = false;
  }
  
  if (led_state == 1) {
    RGBColor(Color::Blue);
    digitalWrite(back_led_pin, LOW);
    digitalWrite(forward_led_pin, LOW);
    digitalWrite(left_led_pin, LOW);
    digitalWrite(right_led_pin, LOW);
    
    calibrating = true;
  }
  
  if (!calibrating) {    
    if ((euler.x() >= start_x_value - 5 && euler.x()  <= start_x_value + 5) &&
        (euler.y() >= start_y_value - 5 && euler.y()  <= start_y_value + 5)) 
    {
      RGBColor(Color::Green);
      digitalWrite(back_led_pin, LOW);
      digitalWrite(forward_led_pin, LOW);
      digitalWrite(left_led_pin, LOW);
      digitalWrite(right_led_pin, LOW);
    }
    else {
      RGBColor(Color::Nothing);
      
      if(euler.x() > start_x_value + 5){
        digitalWrite(left_led_pin, HIGH);
      }
      else{
        digitalWrite(left_led_pin, LOW);
      }
      if(euler.x() < start_x_value - 5){
        digitalWrite(right_led_pin, HIGH);
      }
      else{
        digitalWrite(right_led_pin, LOW);
      }
      if(euler.y() > start_y_value + 5){
        digitalWrite(back_led_pin, HIGH);
      }
      else{
        digitalWrite(back_led_pin, LOW);
      }
      if(euler.y() < start_y_value - 5){
        digitalWrite(forward_led_pin, HIGH);
      }
      else{
        digitalWrite(forward_led_pin, LOW);
      }
    }
  }
}
void RGBColor(Color color)
{
  switch(color){
    case Color::Red:
      analogWrite(RGB_red_led_pin, 255);
      analogWrite(RGB_green_led_pin, 0);
      analogWrite(RGB_blue_led_pin, 0);
    break;
    case Color::Green:
      analogWrite(RGB_red_led_pin, 0);
      analogWrite(RGB_green_led_pin, 255);
      analogWrite(RGB_blue_led_pin, 0);
    break;
    case Color::Blue:
      analogWrite(RGB_red_led_pin, 0);
      analogWrite(RGB_green_led_pin, 0);
      analogWrite(RGB_blue_led_pin, 255);
    break;
    default:
      analogWrite(RGB_red_led_pin, 0);
      analogWrite(RGB_green_led_pin, 0);
      analogWrite(RGB_blue_led_pin, 0);
    break;
  }
}
