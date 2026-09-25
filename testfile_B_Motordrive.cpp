

/*
Copyright 2025 k-off (pacovali@42berlin.de)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <A4988.h>

volatile A4988 gStepper1(
    8,  // index of 'Enable'    pin
    3,  // index of 'Step'      pin
    2   // index of 'Direction' pin
);

void setup() {
    Serial.begin(9600);
    gStepper1.enable();
}

/*
    No if/else statements needed. Next task will only be added to the motor after it
    has finished all steps of the previous one.
*/

void loop() {
  // add other logic here

  // give stepper a task
  // trying to give another task before all steps were completed will not have any effect
  gStepper1.addTask(
    50,     // 50 steps
    Back,   // in the backward direction
    1.0/5   // at 20% of the maximum speed. Speed should be a float in range 0.001 .. 1.0
  );

  gStepper1.halfStep(); // toggle step pin of the motor 1 if all conditions are met
}
