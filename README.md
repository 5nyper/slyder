# camrail [![Build status](https://api.travis-ci.org/Vikaton/camrail.svg?branch=master)](https://travis-ci.org/Vikaton/camrail)
Motorized Motion Control Slider

A VEX motor is hooked up to one end of a rail, and the rotary encoder is hooked up on the other side. as the motor turns, the rotary encoder monitors the revolutions so it knows when to stop and take x amount of photos, and continous for the length of y. once it reaches its end, the motor turns counter clockwise and the rotary encoders goes back down to its starting point. There is also a push button to manually reset the slider.

####Parts

- Arduino Uno (1)
- Jumper Wires (>15)
- Optocoupler (1)
- VEX motor (1)
- rotary encoder (1)
- push button (1)
- 2.5mm TRS Jack (1)

# Setup
[![PoC](https://j.gifs.com/VO5GKM.gif)](https://www.youtube.com/watch?v=Rjd7jY60sRE&feature=youtu.be)

![Frizting](http://i.imgur.com/L8Fay1x.png)

# TODO
- [x] connect rotary encoder and monitor ticks
- [x] connect VEX motor and turn it both directions
- [x] add button to reset the device back to its starting point
- [x] add sample test scenario of this device in action
- [x] use higher voltage power supply and control motor speed
- [ ] finish motor logic (physical, not virtual)
- [x] add shutter automation
