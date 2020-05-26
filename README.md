# Rencoder
 
Rencoder is a simple, platform agnostic library that implements a basic quadrature encoder and, optionally, an integrated
click button. There are some notes in [Rencoder.cpp](https://github.com/davexre/Rencoder/blob/master/Rencoder.cpp) about one
way to hardware debounce the encoder pins - for me, this was required to make anything work with my encoder. 

The code here was heavily influenced by Nathan Seidle's code for SparkFun's Qwiic Twist encoder
[https://github.com/sparkfun/SparkFun_Qwiic_Twist_Arduino_Library](library) and 
[https://github.com/sparkfun/Qwiic_Twist](firmware). You might say most of this was directly lifted from Nathan's code (and
you'd be right). The implementation he used was elegant, and I liked his API calls for getting info from the encoder.

Unfortunately, I ran into some issues with using the Twist with my chosen SparkFun Redboard Artemis, so I had to fall back to 
a regular encoder. There's a lot of awesome libraries out there for encoders and certainly I didn't want to pollute the
ecosphere with yet another one, but every one that I looked at was ATmega-ish specific, and would need a larger effort than I
wanted to expend to supply support for the Apollo3 processor on my board. So, the "Re Encoder", or Rencoder library emerged. 

## Encoder Brick

If you don't have a way to hook up your encoder with debouncing circuitry, you might take a look at this board that I
put together: https://github.com/davexre/Encoder-Brick

OSH Park will make three of them for about $7.50. 
