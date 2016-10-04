## Homework1

Created a bank of six sine waves, complete with detuning controls. The sine waves are spread out between the left and right channels to create a pleasing stereo effect.

The Detune/Blend slider provides the frequency difference between each oscillator. (i.e. if set to 10 Hz and there are two oscillators with a base frequency of 1000 Hz, one will be at 995 Hz and one will be at 1005 Hz.)
The Detune Mod slider adds an additional, **random** amount of detuning to each oscillator. Use with care!

With more knowledge, I would have added envelope support, as well as reverb/delay - I think this instrument would be a useful start for creating textures and pads.

Hemanth Kini (hkini@stanford.edu)

This project borrows from the _BasicAudio_ starter code provided by Prof. Michon. Original writeup below.

---

## BasicAudio

Demonstrates how to deal with audio in JUCE. Implements a simple sine wave oscillator from scratch where the frequency and the gain can be controlled using sliders and a checkbox allows to turn it on or off.

---

Implemented by Romain Michon (rmichonATccrmaDOTstanfordDOTedu) for Music 256a / CS 476a (fall 2016).