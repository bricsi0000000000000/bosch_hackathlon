#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT

#include <RemoteXY.h>
#include <LiquidCrystal.h>

#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "BOSCH"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,3,0,0,0,8,1,10,177,4,
  3,131,26,30,50,18,1,36,26,129,
  0,22,23,17,6,1,31,87,111,111,
  100,0,129,0,43,23,15,6,1,16,
  77,101,116,97,108,0,129,0,62,23,
  19,6,1,16,80,108,97,115,116,105,
  99,0,131,3,7,3,24,7,1,176,
  31,77,97,116,101,114,105,97,108,115,
  0,131,2,37,3,26,7,2,176,31,
  68,114,105,108,108,0,131,2,68,3,
  25,7,3,176,31,83,99,114,101,119,
  0,3,134,10,27,80,15,2,1,26,
  129,0,27,17,51,6,2,31,66,105,
  116,32,100,105,97,109,101,116,101,114,
  32,40,109,109,41,0,129,0,15,43,
  3,6,2,31,49,0,129,0,29,43,
  3,6,2,31,50,0,129,0,42,43,
  3,6,2,31,51,0,129,0,55,43,
  3,6,2,31,52,0,129,0,68,43,
  3,6,2,31,53,0,129,0,81,43,
  3,6,2,31,54,0,3,131,30,27,
  41,15,3,1,26,129,0,44,17,12,
  6,3,31,83,105,122,101,0,129,0,
  24,43,13,5,3,31,83,109,97,108,
  108,0,129,0,41,43,19,5,3,31,
  77,101,100,105,117,109,0,129,0,64,
  43,13,5,3,31,76,97,114,103,101,
  0 };
  
struct {
  uint8_t materials_select; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t drill_diameter_select; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t screw_diameter_select; // =0 if select position A, =1 if position B, =2 if position C, ... 

  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int menu_button_pin = 7;
int option_button_pin = 8;

bool text_on_screen = false;
int last_material_select_option = -1;
int last_drill_select_option = -1;
int last_screw_select_option = -1;

enum Materials {Wood, Aluminium, Steel};
enum Size {Small, Medium, Large};

Materials selected_material = Materials::Wood;
int selected_drill_diameter = 0;
Size selected_screw_size = Size::Small;

bool drill_or_screw = true; //true: drill, false: screw

unsigned int menu_state = HIGH;        
unsigned int menu_button_state;            
unsigned int last_menu_button_state = LOW;

unsigned int option_state = HIGH;        
unsigned int option_button_state;            
unsigned int last_option_button_state = LOW;

unsigned long last_debounce_time = 0; 
unsigned long debounce_delay = 50;

unsigned long option_last_debounce_time = 0; 
unsigned long option_debounce_delay = 50;

int potmeter_value = 0;

int menu = 4;
int option = 0;

int material_option = 0;
int diameter_option = 0;
int size_option = 0;

int analog_input_pin = A0;
int PWM_pin = 9;
int ENABLE_pin = 10;

double input_voltage = 0;
unsigned int duty_cycle = 0;
unsigned int analog_val = 0;
double avg_current = 0;
double current_sum = 0;
int i = 0;
int max_current = 10;
int disabled = 0;
long init_timer_val = 0;
long timer_val = 0;

void setup()
{
  RemoteXY_Init ();
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(menu_button_pin, INPUT); 
  pinMode(option_button_pin, INPUT); 

  pinMode(analog_input_pin, INPUT);
  pinMode(PWM_pin, OUTPUT);
  pinMode(ENABLE_pin, OUTPUT);
  digitalWrite(ENABLE_pin, HIGH);
}

void loop()
{
  int reading = digitalRead(menu_button_pin);
  if (reading != last_menu_button_state) {
    last_debounce_time = millis();
  }

  if ((millis() - last_debounce_time) > debounce_delay) {
    if (reading != menu_button_state) {
      menu_button_state = reading;

      if (menu_button_state == HIGH) {
        menu_state = !menu_state;
        text_on_screen = false;
        if(menu < 3){
          menu++;
        }
        else{
          menu = 0;
        }
      }
    }
  }

  last_menu_button_state = reading;

  int option_reading = digitalRead(option_button_pin);
  if (option_reading != last_option_button_state) {
    option_last_debounce_time = millis();
  }

  if ((millis() - option_last_debounce_time) > option_debounce_delay) {
    if (option_reading != option_button_state) {
      option_button_state = option_reading;

      if (option_button_state == HIGH) {
        option_state = !option_state;
        text_on_screen = false;
        switch(menu){
          case 0:
            if(material_option < 2){
              material_option++;
            }
            else{
              material_option = 0;
            }
          break;
          case 1:
            if(diameter_option < 5){
              diameter_option++;
            }
            else{
              diameter_option = 0;
            }
          break;
          case 2:
            if(size_option < 2){
              size_option++;
            }
            else{
              size_option = 0;
            }
          break;
        }
      }
    }
  }

  last_option_button_state = option_reading;
  
  RemoteXY_Handler ();

  if(menu == 4){
    if(RemoteXY.materials_select != last_material_select_option){
      text_on_screen = false;
    }
    if(RemoteXY.drill_diameter_select != last_drill_select_option){
      text_on_screen = false;
    }
     if(RemoteXY.screw_diameter_select != last_screw_select_option){
      text_on_screen = false;
    }
  
    if (RemoteXY.materials_select == 0) {
      selected_material = Materials::Wood;
    }
    else if (RemoteXY.materials_select == 1) {
      selected_material = Materials::Aluminium;
    }
    else if (RemoteXY.materials_select == 2) {
      selected_material = Materials::Steel;
    }
    material_option = RemoteXY.materials_select;
  
    selected_drill_diameter = RemoteXY.drill_diameter_select + 1;
    diameter_option = RemoteXY.drill_diameter_select;
  
    if(RemoteXY.screw_diameter_select == 0){
      selected_screw_size = Size::Small;
    }
    else if(RemoteXY.screw_diameter_select == 1){
      selected_screw_size = Size::Medium;
    }
    else if(RemoteXY.screw_diameter_select == 2){
      selected_screw_size = Size::Large;
    }
    size_option = RemoteXY.drill_diameter_select;

    last_material_select_option = RemoteXY.materials_select;
    last_drill_select_option = RemoteXY.drill_diameter_select;
    last_screw_select_option = RemoteXY.screw_diameter_select;
  }
  else {   
    switch(menu){
      case 0:
        switch(material_option){
          case 0:
            selected_material = Materials::Wood;
          break;
          case 1:
            selected_material = Materials::Aluminium;
          break;
          case 2:
            selected_material = Materials::Steel;
          break;
        }
      break;
      case 1:
        selected_drill_diameter = diameter_option + 1;
      break;
      case 2:
        switch(size_option){
          case 0:
            selected_screw_size = Size::Small;
          break;
          case 1:
            selected_screw_size = Size::Medium;
          break;
          case 2:
            selected_screw_size = Size::Large;
          break;
        }
       break;
    }

    last_material_select_option = material_option;
    last_drill_select_option = diameter_option;
    last_screw_select_option = size_option;
  }

  printLCD();

  input_voltage = (double)(analogRead(analog_input_pin) / 1024.0 * 5.0) / 0.002 / 20;
  if (i == 30)
  {
    i = 0;
    avg_current = current_sum / 30;
    current_sum = 0;
  }
  else
  {
    i++;
    current_sum += input_voltage;
  }
  if(avg_current >= max_current)
  {
    disabled = 1;
    init_timer_val = millis();
  }
  if (timer_val - init_timer_val >= 1000)
  {
    disabled = 0;
  }
  if (disabled)
  {
    digitalWrite(ENABLE_pin, LOW);
    timer_val = millis();
  }
  else
  {
    digitalWrite(ENABLE_pin, HIGH);
  }
  
   switch(selected_material){
    case Materials::Steel:
      switch(selected_drill_diameter){
        case 1:
          duty_cycle = 95;
        break;
        case 2:
          duty_cycle = 80;
        break;
        case 3:
          duty_cycle = 65;
        break;
        case 4:
          duty_cycle = 50;
        break;
        case 5:
          duty_cycle = 35;
        break;
        case 6:
          duty_cycle = 20;
        break;
      }
    break;
    case Materials::Aluminium:
      switch(selected_drill_diameter){
        case 1:
          duty_cycle = 100;
        break;
        case 2:
          duty_cycle = 85;
        break;
        case 3:
          duty_cycle = 70;
        break;
        case 4:
          duty_cycle = 55;
        break;
        case 5:
          duty_cycle = 40;
        break;
        case 6:
          duty_cycle = 25;
        break;
      }
    break;
    case Materials::Wood:
      switch(selected_drill_diameter){
        case 1:
          duty_cycle = 100;
        break;
        case 2:
          duty_cycle = 90;
        break;
        case 3:
          duty_cycle = 75;
        break;
        case 4:
          duty_cycle = 60;
        break;
        case 5:
          duty_cycle = 45;
        break;
        case 6:
          duty_cycle = 30;
        break;
      }
    break;
  }
  analog_val = (duty_cycle * 255 / 100);
  analogWrite(PWM_pin, analog_val);

  // do not call delay()
  // do not write on serial
}

void printLCD(){
  if(!text_on_screen){
    text_on_screen = true;
    
    lcd.clear();
     if(menu == 0){
      lcd.setCursor(0,0);
      lcd.print("*");
    }
    lcd.setCursor(1,0);
    switch(selected_material){
      case Materials::Wood:
        lcd.print("wood");
      break;
      case Materials::Aluminium:
        lcd.print("alu");
      break;
      case Materials::Steel:
        lcd.print("steel");
      break;
    }

   if(menu == 1){
      lcd.setCursor(9,0);
      lcd.print("*");
    }
    lcd.setCursor(10,0);
    lcd.print(String(selected_drill_diameter));
    lcd.setCursor(12,0);
    lcd.print("mm");

    if(menu == 2){
      lcd.setCursor(9,1);
      lcd.print("*");
    }
    lcd.setCursor(10,1);
    switch(selected_screw_size){
      case Size::Small:
        lcd.print("small");
      break;
      case Size::Medium:
        lcd.print("medium");
      break;
      case Size::Large:
        lcd.print("large");
      break;
    }
  }
}
