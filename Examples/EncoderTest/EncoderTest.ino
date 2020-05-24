
#include <Arduino.h>
#include "Rencoder.h"

#define  ENCODER_A_PIN   10
#define  ENCODER_B_PIN   11
#define  ENCODER_BUTTON  7

Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON);

void setup() {

  Serial.begin(115200);
  while (!Serial) {};

  Serial.println("Encoder test");

}

void loop() {
  Serial.print("Count: ");
  Serial.print(encoder.getCount());

  if (encoder.isMoved()) {
    Serial.print(" Difference: ");
    Serial.print(encoder.getDiff());
  }
  
  if (encoder.isPressed()) Serial.print(" Pressed!");

  if (encoder.isClicked()) Serial.print(" Clicked!");

  if (encoder.isDoubleClicked()) Serial.print (" Double Clicked!");

  Serial.println();
  
  delay(250);
}
