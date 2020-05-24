/**************************************************************************************************
 * 
 * Rencoder.h
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
 */
#ifndef _RENCODER_H
#define _RENCODER_H

#include "Arduino.h"

#define ENCODER_DEBOUNCE_MICROS 10000    // in micros, 100ms debounce time on encoder button
#define ENCODER_DOUBLECLICKTIME 500000   // in micros, second button press in 500ms == double click


const byte statusButtonDoubleClickedBit = 4;
const byte statusButtonClickedBit = 2;
const byte statusButtonPressedBit = 1;
const byte statusEncoderMovedBit = 0;


class Encoder 
{
  public:
    Encoder(uint8_t a, uint8_t b);
    Encoder(uint8_t a, uint8_t b, uint8_t btn);
    
    volatile byte  statusRegister;
    volatile int16_t   count;
    volatile int16_t   diff;
    
    uint8_t pinA;
    uint8_t pinB;
    int pinButton;

    volatile byte lastEncoded;
    
    volatile unsigned long buttonLastClick;
    volatile unsigned long debounceMicros;
    volatile int buttonState;

    void encoderSetup(uint8_t a, uint8_t b, uint8_t btn);
    int16_t getCount();                                        // Returns the number of indents the user has turned the knob
    void setCount(int16_t amount);                             // Set the number of indents to a given amount
    int16_t getDiff(boolean clearValue = true);                // Returns the number of ticks since last check
    boolean isMoved(boolean clearValue = true);                // Returns true if knob has been twisted
    boolean isPressed(boolean clearValue = true);              // Return true if button is currently pressed.
    boolean isClicked(boolean clearValue = true);              // Return true if the button has been pressed and released
    boolean isDoubleClicked(boolean clearValue = true);        // Return true if the button has been double clicked
    void clear();                                              // Clear the status register

    void encoderInterrupt();
    void buttonInterrupt();

    static Encoder *thisencoder[1];

    static void encoderISR() { thisencoder[0]->encoderInterrupt(); }
    static void buttonISR() { thisencoder[0]->buttonInterrupt(); }
};



#endif
