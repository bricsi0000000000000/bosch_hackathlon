# bosch_hackathlon
Bosch hackathlon

# Angle measurement

We use the **Adafruit BNO055** sensor, to measure the angle of the drill.

On line 6, we define the sensor.

```c
Adafruit_BNO055 bno = Adafruit_BNO055(55);
```

In the `setup` function we check, if the sensor is ready to use.

```c
if (!bno.begin()) {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
}
```

```c
bno.setExtCrystalUse(true);
```

We define the states of the drill with an enum. This called `Color`

```c
enum Color {Red, Green, Blue, Nothing};
```

In the `loop` function we get the **euler vector** of the sensor.

```c
imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
```

Because the sensor is flipped over on the bottom of the drill, we have to handle this.

```c
if (euler.x() < 0)
{
    euler.x() = -(180 - fabsf(euler.x()));
}
else
{
    euler.x() = 180 - fabsf(euler.x());
}
```

We use **de-bouncing** for the *push-button*.

After we pushed the button, the program saves the current rotation of the drill.

We adjusted a 3 degree threshold on both axes.

```c
if ((euler.x() >= start_x_value - 3 && euler.x()  <= start_x_value + 3) &&
    (euler.y() >= start_y_value - 3 && euler.y()  <= start_y_value + 3)) 
{}
```

After that, we check on the **x** and **y** axes the right rotation.

```c
if(euler.x() > start_x_value + 3){
    digitalWrite(right_led_pin, HIGH);
}
else{
    digitalWrite(right_led_pin, LOW);
}

if(euler.x() < start_x_value - 3){
    digitalWrite(left_led_pin, HIGH);
}
else{
    digitalWrite(left_led_pin, LOW);
}

if(euler.y() > start_y_value + 3){
    digitalWrite(back_led_pin, HIGH);
}
else{
    digitalWrite(back_led_pin, LOW);
}

if(euler.y() < start_y_value - 3){
    digitalWrite(forward_led_pin, HIGH);
}
else{
    digitalWrite(forward_led_pin, LOW);
}
```

For the RGB LED we defined a function, that makes the right color for the operation. It's called `RGBColor` and it takes that `enum`, that has the colors predefined.

```c
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
```

# Docking station

For this project we used a framework called **RemoteXY**. With this framework, we were able to connect through the **ESP-01** modul with our phone, to the Arduino.

```c
#include <RemoteXY.h>
```

We used an LCD display too.

```c
#include <LiquidCrystal.h>
```

Basically we defined **4 menus**.

1. materials
2. drill diameter
3. screw size
4. nothing

If the **4.** menu is selected, you can control the settings with your phone.
If not, you can choose menus and options manually.

After we choose the right options, the program will tell the right drilling options for the user.

You can choose three materials.

```c
enum Materials {Wood, Aluminium, Steel};
```

```c
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
```

For the printing of the LCD display, we defined a function called `printLCD`.

It writes the right data to the right position on the LCD display.