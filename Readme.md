PinChangeInterrupt Library 1.2.9
================================

![Header Picture](missing.png)



##### Features:
* Support 1, 2, or 3 ULN2003 controls and unipolar motors
* Half-stepping enabled
* Uses interrupts for fast control


#### Supported configurations:
```
Arduino Nano: All pins are usable
  INT0 (pin 2) and INT1 (pin3) are preferrentially used for step inputs
```


Installation
============

Install from the Arduino Library manager: search for "Uln2003DirStep"

Download the zip, extract and remove the "-master" of the folder.
Install the library [as described here](http://arduino.cc/en/pmwiki.php?n=Guide/Libraries).

How to use
==========

Bipolar stepper drivers like the **A4988** have a simple input interface: 
a direction pin to indicate if the motor should rotate forward or reverse, 
a step pin to take a single step, and an enable pin that can put the motor 
into freewheel (disabled) or brake (enabled) mode.

Unipolar drivers like the ULN2003 are really just FET arrays: the user has 
to do all of the timing work to energize the motor windings.  This simple 
library uses an Arduino Nano to provide dir, step, and enable control of a ULN2003!

* direction = 1 means move forward, direction = 0 means reverse (clockwise 
vs counter clockwise depends on how you connect the motor pins).
* step is a clock signal, stepping on the rising edge.  Prefentially connect
this to **INT0** (Nano pin 2) or **INT1** (pin 3) to use the Arduino's interrupts.
The ULN2003 is pretty slow, 1kHz is the max speed supported.
* _enable (optional) can be set high to disable the ULN2003 outputs, 
allowing the motor to coast.

My impetus for this project was that some popular motion control software, 
like Marlin and grbl, had no support for unipolar motors or drivers like the
ULN2003.  So I created this hardware interfaces to dress up ULN2003 like 
a supported stepper driver.


### Examples


### API Reference

##### Simple example
```cpp

#define step 3
#define dir  4
#define mot1 5
#define mot2 6
#define mot3 7
#define mot4 8

Uln2003DirStep stepper0(mot1, mot2, mot3, mot4, dir, step);

setup(){
  for (int i=0; i<100; i++)
    stepper0.step();
}

loop(){}

```


Version History
===============
```
0.0.1
  Works, converting to arduino library
```


License and Copyright
=====================
```
Copyright (c) 2024 JeremiahHeilman
Credit to TinyStepper, for how the motor control lookup table is stored.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
