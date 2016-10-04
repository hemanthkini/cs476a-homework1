// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#include <ctime>

#define NUM_OF_SINES 6

class MainContentComponent :
    public AudioAppComponent,
    private Slider::Listener,
    private ToggleButton::Listener
{
public:
    MainContentComponent() : gain (0.0), onOff (0), samplingRate(0.0)
    {
        // configuring frequency slider and adding it to the main window
        addAndMakeVisible (frequencySlider);
        frequencySlider.setRange (50.0, 5000.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0);
        frequencySlider.setValue(1000); // will also set the default frequency of the sine osc
        frequencySlider.addListener (this);

        // configuring frequency label box and adding it to the main window
        addAndMakeVisible(frequencyLabel);
        frequencyLabel.setText ("Base Frequency", dontSendNotification);
        frequencyLabel.attachToComponent (&frequencySlider, true);
        
		// configuring detune slider and adding it to the main window
		addAndMakeVisible(detuneSlider);
		detuneSlider.setRange(0.001, 500.0);
		detuneSlider.setSkewFactorFromMidPoint(10.0);
		detuneSlider.setValue(10); // will also set the default frequency of the sine osc
		detuneSlider.addListener(this);

		// configuring detune label box and adding it to the main window
		addAndMakeVisible(detuneLabel);
		detuneLabel.setText("Detune/Blend", dontSendNotification);
		detuneLabel.attachToComponent(&detuneSlider, true);

		// configuring detune mod slider and adding it to the main window
		addAndMakeVisible(detuneModSlider);
		detuneModSlider.setRange(0.000, 500.0);
		detuneModSlider.setSkewFactorFromMidPoint(10.0);
		detuneModSlider.setValue(0.0); // will also set the default frequency of the sine osc
		detuneModSlider.addListener(this);

		// configuring detune mod label box and adding it to the main window
		addAndMakeVisible(detuneModLabel);
		detuneModLabel.setText("Detune Mod Speed (hz)", dontSendNotification);
		detuneModLabel.attachToComponent(&detuneModSlider, true);


        // configuring gain slider and adding it to the main window
        addAndMakeVisible (gainSlider);
        gainSlider.setRange (0.0, 1.0);
        gainSlider.setValue(0.2); // will alsi set the default gain of the sine osc
        gainSlider.addListener (this);
        
        
        // configuring gain label and adding it to the main window
        addAndMakeVisible(gainLabel);
        gainLabel.setText ("Gain", dontSendNotification);
        gainLabel.attachToComponent (&gainSlider, true);


        
        
        // configuring on/off button and adding it to the main window
        addAndMakeVisible(onOffButton);
        onOffButton.addListener(this);
        
        
        // configuring on/off label and adding it to the main window
        addAndMakeVisible(onOffLabel);
        onOffLabel.setText ("On/Off", dontSendNotification);
        onOffLabel.attachToComponent (&onOffButton, true);
        
        setSize (600, 200);
        nChans = 2;
        setAudioChannels (0, nChans); // no inputs, one output
		srand(time(0));
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        // placing the UI elements in the main window
        // getWidth has to be used in case the window is resized by the user
        const int sliderLeft = 80;
        frequencySlider.setBounds (sliderLeft, 10, getWidth() - sliderLeft - 20, 20);
        gainSlider.setBounds (sliderLeft, 40, getWidth() - sliderLeft - 20, 20);
		detuneSlider.setBounds(sliderLeft, 70, getWidth() - sliderLeft - 20, 20);
		detuneModSlider.setBounds(sliderLeft, 100, getWidth() - sliderLeft - 20, 20);
        onOffButton.setBounds (sliderLeft, 130, getWidth() - sliderLeft - 20, 20);
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (samplingRate > 0.0){
            if (slider == &frequencySlider || slider == &detuneSlider || slider == &detuneModSlider){
				for (int i = 0; i < NUM_OF_SINES; i++) {
					// Set frequency based on number of sines - each one is detuned
					// based on the detuneSlider and detuneModSlider.
					sine[i].setFrequency(frequencySlider.getValue() + // Base frequency
						(i - ((NUM_OF_SINES / 2.0) + .5))*(detuneSlider.getValue() + // detune offset
							((float)rand() / (float)RAND_MAX) * detuneModSlider.getValue())); // detune random mod
				}
            }
            else if (slider == &gainSlider){
                gain = gainSlider.getValue();
            }	
        }
    }
    
    void buttonClicked (Button* button) override
    {
        // turns audio on or off
        if(button == &onOffButton && onOffButton.getToggleState()){
            onOff = 1;
        }
        else {
            onOff = 0;
        }
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        samplingRate = sampleRate;
		// set sample rate for each oscillator
		for (int i = 0; i < NUM_OF_SINES; i++) {
			sine[i].setSamplingRate(sampleRate);
		}
    }
    
    void releaseResources() override
    {
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // getting the audio output buffer to be filled
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
		float* const buffer2 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
			buffer[sample] = 0.0;
			if (onOff == 1) {
				for (int i = 0; i < NUM_OF_SINES; i++) {
					// stereo audio - we compute each one's gain differently, to pan the detuned sine waves out
					// We max each gain out at .166666666 (so that we ensure we won't clip)
					// and then furthermore scale each volume to create a stereo effect
					buffer[sample] += sine[i].tick() * (gain / ((float)NUM_OF_SINES)) 
						* ((NUM_OF_SINES / 2.0) + (i - ((NUM_OF_SINES / 2.0) + .5)) / (NUM_OF_SINES - 0.5));
					buffer2[sample] += sine[i].tick() * (gain / ((float)NUM_OF_SINES)) 
						* ((NUM_OF_SINES / 2.0) + (0 - i + ((NUM_OF_SINES / 2.0) + .5)) / (NUM_OF_SINES - 0.5));
				}

				// Sanity checks
				if (buffer[sample] > 1.0 || buffer[sample] < -1.0) {
					printf("Buffer out of bounds.\n");
				}
				if (buffer2[sample] > 1.0 || buffer[sample] < -1.0) {
					printf("Buffer2 out of bounds.\n");
				}
			}
            
        }
    }
    
    
private:
    // UI Elements
    Slider frequencySlider;
    Slider gainSlider;
	Slider detuneSlider;
	Slider detuneModSlider;
    ToggleButton onOffButton;
    
    Label frequencyLabel, gainLabel, onOffLabel, detuneLabel, detuneModLabel;

    Sine sine[NUM_OF_SINES]; // the sine wave oscillator bank

    // Global Variables
    float gain;
    int onOff, samplingRate, nChans, detuneModSwitch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
