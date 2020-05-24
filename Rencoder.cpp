/****************************************************************************************************************
 *
 * Rencoder.cpp
 * Author: Dave Re
 * Inception: 05/20/2020
 *
 * I wasn't totally happy with the available encoder libraries, so I implemented my own here,
 * based on the implementation described here: https://bildr.org/2012/08/rotary-encoder-arduino/
 * and the firmware and library Nathan Seidle developed for the SparkFun Qwiic Twist: 
 * https://github.com/sparkfun/Qwiic_Twist and https://github.com/sparkfun/SparkFun_Qwiic_Twist_Arduino_Library
 * 
 * There are a bunch of encoder libraries out there, so I didn't do an exhaustive search. I needed
 * this to work in a way that it would be easily usable by both the main code and by the ArduinoMenu
 * system.
 * 
 * Includes a button, and ISRs, so we're not timer driven - and therefore, we don't pay to service
 * the encoder when it's not actually being moved. Current iteration only allows for a single
 * Encoder, but adding support for multiples could be done quickly in the fashion used in the 
 * Quadrature Encoder library: https://github.com/Saeterncj/QuadratureEncoder/blob/master/QuadratureEncoder.cpp
 *
 *
 * You will almost certainly need to debounce your encoder in hardware!
 * 
 *      (internal 20k)       10k
 * pin:<---/\/\--------*----/\/\----|
 *                     |            |
 *              0.1uf ===            / switch
 *                     |            /
 * gnd:<---------------*------------|
 *
 * This scheme works for me, when applied to the A/B pins on the encoder. I don't debounce
 * the switch with this scheme. Instead, the code below uses the internal pullup resistor
 * and just toggles the switch state.
 * 
 ****************************************************************************************************************/

#include "./Rencoder.h"

Encoder *Encoder::thisencoder[1] = { NULL };

Encoder::Encoder(uint8_t a, uint8_t b) {
  Encoder::encoderSetup(a, b, -1);
}

Encoder::Encoder(uint8_t a, uint8_t b, uint8_t btn) {
  Encoder::encoderSetup(a, b, btn);
}

void Encoder::encoderSetup(uint8_t a, uint8_t b, uint8_t btn) {
  pinA = a;
  pinB = b;
  pinButton = btn;

  statusRegister = 0;
  count = 0;
  diff = 0;
  lastEncoded = 0;
  buttonLastClick = micros();
  debounceMicros = buttonLastClick;
  buttonState = HIGH;
  
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(pinA, encoderISR, CHANGE);
  attachInterrupt(pinB, encoderISR, CHANGE);

  if (pinButton >= 0) {
    pinMode(pinButton, INPUT_PULLUP);
    attachInterrupt(pinButton, buttonISR, CHANGE);
  }

  thisencoder[0] = this;

}

/*
 * getCount
 *
 * Returns the number of indents that the encoder has been twisted. 
 *
 */
int16_t Encoder::getCount() {
  return (count);
}

/*
 * setCount
 *
 * Sets the current count to a specific value
 *
 */
void Encoder::setCount(int16_t amount) {
  count = amount;
}

/*
 * getDiff
 *
 * Returns the count difference vs. the last time getDiff was cleared. By default,
 * calling getDiff clears the difference, too. Calling with clearValue = false
 * will mimic a "peek" sort of Stream behavior, where it returns a value but
 * preserves the current state.
 *
 */
int16_t Encoder::getDiff(boolean clearValue) {
  int16_t diffReturn = diff;
  if (clearValue) diff = 0;
  return (diffReturn);
}

/*
 * isMoved
 *
 * Has the encoder moved since the last time this was checked. By default, clears
 * the flag. If clearValue is false, it will leave the flag intact, allowing a
 * "peek" sort of behavior, where state is left intact after the call.
 *
 */
boolean Encoder::isMoved(boolean clearValue) {
  boolean moved = statusRegister & (1 << statusEncoderMovedBit);
  if (clearValue) {
    statusRegister &= ~(1 << statusEncoderMovedBit); // clear the bit after we've read it
  }
  return (moved);
}

/*
 * isPressed
 *
 * Button has been pressed down, starting a click
 *
 * Clears the flag by default, but will leave it if clearValue is false.
 *
 */
boolean Encoder::isPressed(boolean clearValue) {
  boolean pressed = statusRegister & (1 << statusButtonPressedBit);
  if (clearValue) {
    statusRegister &= ~(1 << statusButtonPressedBit); // clear the bit after we've read it
  }
  return (pressed);
}

/*
 * isClicked
 *
 * Button has been pressed AND released, completing a click.
 *
 * Clears the flag by default, but will leave it if clearValue is false.
 *
 */
boolean Encoder::isClicked(boolean clearValue) {
  boolean clicked = statusRegister & (1 << statusButtonClickedBit);
  if (clearValue) {
    statusRegister &= ~(1 << statusButtonClickedBit); // clear the bit after we've read it
  }
  return (clicked);
}

/*
 * isDoubleClicked
 *
 * Button has been clicked twice within ENCODER_DOUBLECLICKTIME microseconds.
 *
 * Clears the flag by default, but will leave it if clearValue is false.
 *
 */
boolean Encoder::isDoubleClicked(boolean clearValue) {
  boolean doubleclicked = statusRegister & (1 << statusButtonDoubleClickedBit);
  if (clearValue) {
    statusRegister &= ~(1 << statusButtonDoubleClickedBit); // clear the bit after we've read it
  }
  return (doubleclicked);
}

/*
 * clear
 *
 * Clears the Moved, Pressed, Clicked, and DoubleClicked flags
 *
 */
void Encoder::clear() {
  statusRegister = 0;
}

/*
 * encoderInterrupt
 *
 * ISR for interrupts on the encoder pins
 *
 */
void Encoder::encoderInterrupt() {
  byte MSB = digitalRead(pinA);
  byte LSB = digitalRead(pinB);

  byte encoded = (MSB << 1) | LSB;
  
  lastEncoded = (lastEncoded << 2) | encoded;
  byte sum = (lastEncoded << 2) | encoded;

  if (lastEncoded == 0b01001011) //One indent clockwise
  {
    count++;
    diff++;
    statusRegister |= (1 << statusEncoderMovedBit);
  }
  else if (lastEncoded == 0b10000111) //One indent counter clockwise
  {
    count--;
    diff--;
    statusRegister |= (1 << statusEncoderMovedBit);
  }
  
}


/*
 * buttonInterrupt
 *
 * ISR for interrupts on the button
 *
 */
void Encoder::buttonInterrupt() {
  // we're here because the button pin has changed
  // remember, we're using a pullup resistor, here, so LOW = pressed
  
  if ((long) (micros() - debounceMicros) >= ENCODER_DEBOUNCE_MICROS) {
    
    debounceMicros = micros();

    if (buttonState == LOW) {
      buttonState = HIGH;
      statusRegister |= (1 << statusButtonClickedBit);
      
      if ((debounceMicros - buttonLastClick) <= ENCODER_DOUBLECLICKTIME) {
        // looks like we have a double click
        statusRegister |= (1 << statusButtonDoubleClickedBit);
       }

      buttonLastClick = debounceMicros;
      
    }
    else {
      buttonState = LOW;
      statusRegister |= (1 << statusButtonPressedBit);
    }
    
   } // debounce
  
}
