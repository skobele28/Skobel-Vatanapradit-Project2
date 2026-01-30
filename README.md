## Project 2: Automatic Headlight Control System for Driver's Ed Vehicle
Team members: Erika Skobel and Mia Vatanapradit
### System Behavior
The system includes the driver’s ed system that requires four safety requirements to be fulfilled before ignition: passenger and driver present, both wearing seatbelts. When the driver sits, a welcome message displays, when all conditions are met, the green LED lights up, and if ignition is successful, the red LED lights up. If the driver does not follow the requirements, pushing the ignition does not start the engine, a buzzer alarm is triggered, and warning messages are displayed. We modified the system to allow additional start attempts if the ignition is inhibited and the ability to stop the engine after the car is started by pressing the ignition button again. We also implemented a headlight control system. This system includes a set of low beam headlights controlled by a potentiometer knob to set the headlights to on, auto, or off. For the auto setting, the headlights turn on after a delay of one second if the light sensor reads a low enough light level, they turn off after a delay of two seconds if the light sensor reads a high enough light level, and they maintain their previous state if the light level is in between. We also included a set of highbeams which can be switched on and off as long as the lowbeams are on. The headlight system only works when the engine is running. Pressing the ignition button to turn off the engine also turns off the headlight system.

### Starting Repository
[GitHub Link](https://github.com/skobele28/Belliard-Skobel-Project1.git)

### Summary of Testing Results
**<ins>Ignition Subsystem</ins>**
|Specification|Test Process|Results|
|-------------|------------|-------|
|1. Enable engine start (i.e., light the green LED) while both seats are occupied (PS, DS) and seatbelts fastened (PB, DB). Otherwise print appropriate messages and sound the alarm.|4 buttons: DS, DB, PS, PB<br>1. All buttons pressed<br>2. All but one button (PS) pressed<br>3. No buttons pressed|All tests passed.<br>1. Green light on<br>2. Print "Ignition inhibited" and "Passenger seatbelt not fastened." Alarm activated.<br>3. All error messages printed. Alarm activated.|
|2. Start the engine (i.e., light the red LED, turn off green) when ignition is enabled (green LED) and th eignition button is pressed (I.E., before the button is released).|All buttons pressed, green LED is on, ignition button pressed|Test passed.<br>Red engine light illuminates and stays on after the ignition button is released.|
