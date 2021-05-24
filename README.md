# Chicken-coop open/close door with Arduino

Two versions:
- Light-based control
- RealtimeClock control

-Needed Libraries:
- Wire.h
- BH1750.h
- LowPower : https://github.com/lbsm2017/Low-Power
- Dusk2Dawn : https://www.arduino.cc/reference/en/libraries/dusk2dawn/
- RTClib : https://www.arduino.cc/reference/en/libraries/rtclib/


It operates with a a BH1750 light meter and a 12vDC motor featured with slow-pace gearbox.
Arduino Uno-r3, - 12v DC motor with gearbox (appox 20rpm) and L298N h-bridge and motor driver.

_Light-based Version_
- BH1750 light-sensor

_RTC-based Version_
- DS3231 RTC module for RTC version

hardware schematic:
2: output -> to one of the two input pins on L298N 
4: output -> to the other input pin on L298N
A5 : to SCL pin on BH1750 light sensor  /or DS3231 
A4 : to SDA pin on BH1750 light sensor /or DS3231 
ADR and GND on BH1750 -> to GND on Arduino
Vin on light sensor -> to +5V on Arduino (some sensors are 3.3V, mine is 5V, check yours!)

See other projects for details about BH1750, eg: https://create.arduino.cc/projecthub/infoelectorials/project-017-arduino-bh1750-light-sensor-project-640075 

Arduino is powered with +5V from L298N 5v_out , to +5v pin in Arduino - this can be tricky if necessary caution is not used. 
L298N h-bridge is featured with decent ST 12->to->5v reducer, that provides stabilized and controlled 0.5A DC.
I do not suggesto to use the Vin pin to power Arduino-Uno with a +5V , as that pin can accept up to +20V, it has an internal recuder to +5v, which uses approximately 2v to stabilize this tension, hence, if you provide a +5V --> into Vin , your poor Arduino will only receive about 3V , which is too low to operate properly. 

BE CAUTIOUS: if you power Arduino on the 5V pin, BE *SURE* tension is stabilized and 5v max.

## Code description ##
Door is raised up with a function that operates the motor (up) for 15 seconds, using a simple delay instruction - it's not extremely elegant, but straight to the point, no need to complicate things with millis() in this specific case. Time lenght is found empirically. 

As per door closure, I put a "NO" magnetic swith on the door, so the arduino "knows" when door is closed down and can stop the engine. This is more elegant and efficient than a time-based function for a few reasons: gravity make an influence on the torque of the motor, lifting an object is slower than dropping (descending) it , hence it would be necessary to make many inefficient tests to calculate the perfect timing for closing the door - plus, magnetic switch is more secure and precise, stopping the motor and the right moment - so we are more sure door is securely closed down (even if battery voltage drops a bit lower and motor would take more than expected to close, eg.)

-- Lightmeter -- 
Day/night light (lux) threashold were found empirically, they can be set according to your needs, same goes for opening timings and delays.
I use a double reading for light-sensor, namely 'lux_a' and 'lux_b' , it's a safety to aboid peaks or false readings (imagine if one bird is in front of the light meter) - it give a simple avering reading. I also tried other types of smoothing (many are available online) but non really convinced me - feel free to use them though.
Light reading NEEDS to be as type as "unisgned int" as BH1750 reads from 0 to 65535. 

---- RTC ----
You can set a specifici location with GPS Lat and Long, it will return sunrise and sunset times for any day. Using DS3231 RTC module I obtain "now()" date and time. 
I give an offset for delaying opening the door after 60mins from sunrise and closing it 30mins after sunset. This is for safety, to avoid the happy chickens to run out too early in the morning while the hungry foxes want to have breakfast. At sunset, I allow an extra half-an-hour for all the chickens to go to bed, to avoid locking some out. 

