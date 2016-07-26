# hardpush

Use an ableton push 1 to control synthesisers via DIN MIDI, without the need of a computer.

Original features supported:
- Play notes in scale with velocity sensitive pads
- All scales from original push
- Select root key
- Select octave
- Select layout
- Use sustain pedal

New features added:

- Custom color schemes
- Quick access to 4 instrument control presets with individual settings for: color scheme, midi channel, layout and octave.
- Quick access to 4 independent scale & root key presets
- External DIN MIDI note feedback
- EEPROM memory recalls all settings
- USB hot-swappable


---------------------------------------------------------------
DIY

To build your own you need:
- a teensy 3.1
- a usb host mini (http://www.hobbytronics.co.uk/usb-host-mini)
- a midi breakout board (http://www.hobbytronics.co.uk/midi-breakout?keyword=midi)
- a usb power source

Tools:
- Soldering iron


----------------------------------------------------------------

Known bugs

- The usb host mini firmware 1.13 has some issues sending out big midi messages at high speeds requiring some delays in the code
- Occasionally the led feedback on the last triggered pad on the push gets stuck, suspected hanging note bug in the usb host mini firmware 1.13