// 2 DO

// - reflash the usb host
// - more efficient handeling of displaying input notes
// - Make the display functions build on eachother



#include <MIDI.h>
#include <Bounce2.h>
#include <EEPROM.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,     midiUSB);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2,    midiDIN);

///////////////////////////////////////////////////////////
//
// EEPROM ADRESSES
//
///////////////////////////////////////////////////////////

enum {
  PAGE_EEPROM_ADRESS = 2,
  FEEDBACK_EEPROM_ADRESS = 3,
  PAGE1_CHANNEL_EEPROM_ADRESS,
  PAGE2_CHANNEL_EEPROM_ADRESS,
  PAGE3_CHANNEL_EEPROM_ADRESS,
  PAGE4_CHANNEL_EEPROM_ADRESS,
  PAGE1_COLOR_EEPROM_ADRESS,
  PAGE2_COLOR_EEPROM_ADRESS,
  PAGE3_COLOR_EEPROM_ADRESS,
  PAGE4_COLOR_EEPROM_ADRESS,
  PAGE1_OCTAVE_EEPROM_ADRESS,
  PAGE2_OCTAVE_EEPROM_ADRESS,
  PAGE3_OCTAVE_EEPROM_ADRESS,
  PAGE4_OCTAVE_EEPROM_ADRESS,
  PAGE1_LAYOUT_EEPROM_ADRESS,
  PAGE2_LAYOUT_EEPROM_ADRESS,
  PAGE3_LAYOUT_EEPROM_ADRESS,
  PAGE4_LAYOUT_EEPROM_ADRESS,
  SCALE_PRESET_EEPROM_ADRESS,
  PRESET1_TONIC_EEPROM_ADRESS,
  PRESET2_TONIC_EEPROM_ADRESS,
  PRESET3_TONIC_EEPROM_ADRESS,
  PRESET4_TONIC_EEPROM_ADRESS,
  PRESET1_SCALE_EEPROM_ADRESS,
  PRESET2_SCALE_EEPROM_ADRESS,
  PRESET3_SCALE_EEPROM_ADRESS,
  PRESET4_SCALE_EEPROM_ADRESS,
};

// SET A VALUE IN EEPROM
void eepromSet(byte adress, byte value)
{
  EEPROM.update(adress, value);
}
// GET A VALUE FROM EEPROM
byte eepromGet(byte adress, byte minValue = 0, byte maxValue = 255, byte defaultValue = 0)
{
  byte value = EEPROM.read(adress);
  if(value == defaultValue)
    return value;
  if(value < minValue || value > maxValue)
  {
    value = defaultValue;
    eepromSet(adress, value);
  }
  return value;
}



///////////////////////////////////////////////////////////
//
// COLORS
//
///////////////////////////////////////////////////////////

// RGB LEDS
// See this table http://pushmod.blogspot.se/p/pad-color-table.html

// RED GREEN LEDS
/*0 - Off (Black)
1 - Red Dim
2 - Red Dim Blink
3 - Red Dim Blink Fast
4 - Red
5 - Red Blink
6 - Red Blink Fast
7 - Orange Dim
8 - Orange Dim Blink
9 - Orange Dim Blink Fast
10 - Orange
11 - Orange Blink
12 - Orange Blink Fast
13 - Yellow (Lime) Dim
14 - Yellow Dim Blink
15 - Yellow Dim Blink Fast
16 - Yellow (Lime)
17 - Yellow Blink
18 - Yellow Blink Fast
19 - Green Dim
20 - Green Dim Blink
21 - Green Dim Blink Fast
22 - Green
23 - Green Blink
24 - Green Blink Fast
25 -> 127 - Green*/

#define UP_ARROW                            0
#define DOWN_ARROW                          1
#define THREE_STACKED_HORIZONTAL_LINES      2
#define VERTICAL_LINE_AND_HORIZONTAL_LINE   3
#define HORIZONTAL_LINE_AND_VERTICAL_LINE   4
#define TWO_VERTICAL_LINES                  5
#define TWO_SIDE_BY_SIDE_HORIZONTAL_LINES   6
#define FOLDER_SYMBOL                       7
#define SPLIT_VERTICAL_LINES                8
#define FLAT_SYMBOLS                        27
#define THREE_SIDE_BY_SIDE_DOTS             28
#define FULL_BLOCK                          29
#define RIGHT_ARROW                         30
#define LEFT_ARROW                          31



/////////////////////////////////////////////////
//
// SCALES----------------
//
/////////////////////////////////////////////////

//if you want to add more scales please update all these three constants
const int numberOfScales = 26;
const byte scales[][numberOfScales] = { //define scales on the form 'semitones added to tonic'
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // 0 'Chromatic'
  {0, 2, 4, 5, 7, 9, 11}, // 1 ‘Major’
  {0, 2, 3, 5, 7, 8, 10}, // 2 ‘Minor’
  {0, 2, 3, 5, 7, 9, 10}, // 3 ‘Dorian’
  {0, 2, 4, 5, 7, 9, 10}, // 4 ‘Mixolydian’
  {0, 2, 4, 6, 7, 9, 11}, // 5 ‘Lydian’
  {0, 1, 3, 5, 7, 8, 10}, // 6 ‘Phrygian’
  {0, 1, 3, 4, 7, 8, 10}, // 7 ‘Locrian’
  {0, 1, 3, 4, 6, 7, 9, 10}, // 8 ‘Diminished’
  {0, 2, 3, 5, 6, 8, 9, 11}, // 9 ‘Whole-half’
  {0, 2, 4, 6, 8, 10}, // 10 ‘Whole Tone’
  {0, 3, 5, 6, 7, 10}, // 11 ‘Minor Blues’
  {0, 3, 5, 7, 10}, // 12 ‘Minor Pentatonic’
  {0, 2, 4, 7, 9}, // 13 ‘Major Pentatonic’
  {0, 2, 3, 5, 7, 8, 11}, // 14 ‘Harmonic Minor’
  {0, 2, 3, 5, 7, 9, 11}, // 15 ‘Melodic Minor’
  {0, 1, 3, 4, 6, 8, 10}, // 16 ‘Super Locrian’
  {0, 1, 4, 5, 7, 8, 11}, // 17 ‘Bhairav’
  {0, 2, 3, 6, 7, 8, 11}, // 18 ‘Hungarian Minor’
  {0, 1, 4, 5, 7, 8, 10}, // 19 ‘Minor Gypsy’
  {0, 2, 3, 7, 8}, // 20 ‘Hirojoshi’
  {0, 1, 5, 7, 10}, // 21 ‘In-Sen’
  {0, 1, 5, 6, 10}, // 22 ‘Iwato’
  {0, 2, 3, 7, 9}, // 23 ‘Kumoi’
  {0, 1, 3, 4, 7, 8}, // 24 ‘Pelog’
  {0, 1, 3, 4, 5, 6, 8, 10} // 25 ‘Spanish’
};
const char* nameOfScales[numberOfScales] = {
  "Chromatic",
  "Major",
  "Minor",
  "Dorian",
  "Mixolydian",
  "Lydian",
  "Phrygian",
  "Locrian",
  "Diminished",
  "Whole-half",
  "Whole Tone",
  "Minor Blues",
  "Minor Pentatonic",
  "Major Pentatonic",
  "Harmonic Minor",
  "Melodic Minor",
  "Super Locrian",
  "Bhairav",
  "Hungarian Minor",
  "Minor Gypsy",
  "Hirojoshi",
  "In-Sen",
  "Iwato",
  "Kumoi",
  "Pelog",
  "Spanish"
};
const char* nameOfKeys[12] = {
  "C",
  "C#",
  "D",
  "D#",
  "E",
  "F",
  "F#",
  "G",
  "G#",
  "A",
  "A#",
  "B",
};
const byte scaleSizes[numberOfScales] = {12, 7, 7, 7, 7, 7, 7, 7, 8, 8, 6, 6, 5, 5, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 6, 8}; //havn't found a way to do this prettier yet

//////////////////////////////////////////////////
//
// COLOR SCHEMES
// 
//////////////////////////////////////////////////

const byte numberOfColorSchemes=8;
//with passive color, rootnote color and triggered color
byte colors[numberOfColorSchemes][3] = { 
  {51,80,48},
  {31,9,16},
  {55,38,30},
  {43,44,77},
  {47,69,36},
  {7,42,5},
  {15,18,21},
  {23,50,9},
}; 
const char* nameOfColors[numberOfColorSchemes] = {
  "Viol",
  "Turtle",
  "Fungus",
  "Avatar",
  "Passific",
  "Hero",
  "Slime",
  "TMNT",

};

/////////////////////////////////////////////////
//
// LAYOUTS
//
////////////////////////////////////////////////
// LAYOUTS
const byte numberOfLayouts=6;
byte layouts[numberOfLayouts][2]={
  {1,3}, // forths up
  {3,1}, // forths across
  {1,2}, // thirds up
  {2,1}, // thirds across
  {1,8}, // seq up
  {8,1}, // seq across
};
const char* nameOfLayouts[numberOfLayouts] = {
  "4up",
  "4across",
  "3up",
  "3across",
  "Seq-horizontal",
  "Seq-vertical",
};

///////////////////////////////////////////////
//
// GLOBAL VARIABLES
//
////////////////////////////////////////////////

// ACTIVE SCALE PRESET
byte scalePresetIndex; //what scale preset is currently selected for use

//ACTIVE INSTRUMENT PAGE
byte instrumentPageIndex; //what instrument page is currently selected for use


// listen to external midi feedback or internal button presses
bool internalFeedback = true;



// BUG HANDELING
byte displayBugDelay = 26;  // probably due to the usb host firmware
bool pressedNotes[128];

///////////////////////////////////////////////
//
// PAGE VARIABLES
//
////////////////////////////////////////////////
//Each page has a midi-channel, color scheme, octave and layout

const byte numberOfPages = 4; //how many different pages of individual keyboards should be managed
byte pageChannels[numberOfPages];
byte pageColors[numberOfPages]; //the color schemes associated with each page
byte pageOctaves[numberOfPages];
byte pageLayouts[numberOfPages];

const byte numberOfScalePresets = 4;
byte scalePresetTonics[numberOfPages];
byte scalePresetScales[numberOfPages];

///////////////////////////////////////////////
//
// CONNECTED DEVICE DETECTION
//
////////////////////////////////////////////////
Bounce debouncer = Bounce();
int devicePin = 2;


//////////////////////////////////////////////////
//
// SYSEX DATA
//
/////////////////////////////////////////////////
byte sysexUserModeMessage[] = {240, 71, 127, 21, 98, 0, 1, 0, 247};
byte sysexPolyPressure[] = {240, 71, 127, 21, 92, 0, 1, 0, 247};
byte sysexChannelPressure[] = {240, 71, 127, 21, 92, 0, 1, 1, 247};

byte displayBuffer[4][68];

byte sysexDisplayBuffer[77]={240, 71, 127, 21, 24, 0, 69, 0, //the outline for a display sysex message
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
247};

byte padSenseSyssexLow[]= {240, 71, 127, 21, 93, 0, 32, 0, 0, 6, 13, 0, 0, 7, 8, 16, 0, 0, 1, 4, 12, 0, 8, 0, 0, 0, 1, 13, 4, 12, 0, 32, 0, 0, 0, 14, 10, 6, 0, 247}; //sniffed midi message for setting pad sensitivity to -9

////////////////////////////////////////////////////////////////////////////////
//
//
// MATH AND NOTE FUNCTIONS
//
//
////////////////////////////////////////////////////////////////////////////////

int loopingNumbers(int number, int lastNumber){
  if(number>0 && number<=lastNumber)
    return number;
  else if(number<1)
    return lastNumber;
  else
    return 1;
}

int indexInLoopingPattern(int anyIndex, int patternSize){ // works like modulo to get out of bound indecies, but this one also takes negative values
  int maxPatternIndex = patternSize-1;
  if (anyIndex >= 0) {
    return anyIndex % patternSize;
  }
  else {
    anyIndex = abs(anyIndex)-1; // clearify calculations with "distance", we already know it's negative
    return maxPatternIndex - (anyIndex % patternSize); //find index relative to root in that new octave
  }
}


int loopingPatternOffset(int index, int patternSize){ //works like whole number division, but also works for negative values
  if (index >= 0) {
    return index / patternSize;
  }
  else {
    return ((index+1) / patternSize) - 1; //always -1 or less, never 0
  }
}

int getRelativeScaleNote(int index) {//returns midi value for a buttonIndex, with respect to scale and octave offset
  return loopingPatternOffset(index,scaleSizes[scalePresetScales[scalePresetIndex]]) * 12 + scales[scalePresetScales[scalePresetIndex]][indexInLoopingPattern(index,scaleSizes[scalePresetScales[scalePresetIndex]])]; 
}

byte pushButtonIndexToScaleIndex(byte index){ //index to absolute scale step, taking the layout into account
  return (index/8) * getYlayout(instrumentPageIndex) + (index%8) * getXlayout(instrumentPageIndex);
}

byte pushButtonIndexToAbsoluteNote(byte index){ //from index 0-63 to notes 0-127 depending on scale,root and layout
  return scalePresetTonics[scalePresetIndex] + pageOctaves[instrumentPageIndex]*12 + getRelativeScaleNote(pushButtonIndexToScaleIndex(index));
}

bool legalRange(byte value){ // Checks if a note request is within 0-127
  return (value>=0 && value<128);
}

void killAllPressedNotesDIN(byte channel){ 
  for(int i=0;i<128;i++){
    if(pressedNotes[i])
    midiDIN.sendNoteOff(i,0,channel);
  }
}


////////////////////////////////////////////////////////////////////////////////
//
//
// DISPLAY STUFF
//
//
////////////////////////////////////////////////////////////////////////////////


void displayButton(byte index, bool triggered){
  byte buttonState;
  byte note = pushButtonIndexToAbsoluteNote(index);
  if(note<0 || note>127){
    mySendNoteOff(index+36);
    return;
  }
  else if(triggered)
    buttonState = 2; // set button color to triggered
  else if(pushButtonIndexToScaleIndex(index)%scaleSizes[scalePresetScales[scalePresetIndex]]==0)
    buttonState = 1; // set button color to root
  else
    buttonState = 0; // set button color to non-root
  mySendNoteOn(index+36,colors[ pageColors[instrumentPageIndex] ][buttonState]);
}

void initPageSelectButtons(){
  delay(displayBugDelay);
  for(int i=0;i<numberOfPages;i++){
    mySendControlChange(i+106,colors[pageColors[i]][0]);
  }
}

void initScalePresetSelectButtons(){
  delay(displayBugDelay);
  for(int i=0;i<numberOfScalePresets;i++){
    if(i==scalePresetIndex)
      mySendControlChange(i+102,2);
    else
      mySendControlChange(i+102,1);
  }
}


void initPushSettings(){
  midiUSB.sendSysEx(9,sysexUserModeMessage,true);
  //midiUSB.sendSysEx(9,sysexChannelPressure,true);
  midiUSB.sendSysEx(40,padSenseSyssexLow,true);
  delay(30);
}


void initGrid(){
  for(int i=0;i<64;i++){
    displayButton(i,false);
  }
}


void initOtherButtons(){
  for(int i=54;i<56;i++){ //octave buttons
    mySendControlChange(i,4);
  }
  initPageSelectButtons(); // lower buttons
  initScalePresetSelectButtons();
  for(int i=20;i<22;i++){ //top row buttons global settings
    mySendControlChange(i,7);
  }
  for(int i=24;i<28;i++){ //top row buttons page page settings
    mySendControlChange(i,7);
  }
}

void initDisplay(){
  addDisplayTop();
  byte separationLines[68]={6,6,6,6,6,6,6,6,32,6,6,6,6,6,6,6,6,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,6,6,6,6,6,6,6,6,32,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,32,6,6,6,6,6,6,6,6};
  for(int i=0;i<68;i++){
    displayBuffer[1][i]=separationLines[i];
  }

  addButtonLabelAndData(1, String("Mode"), nameOfScales[scalePresetScales[scalePresetIndex]]);
  addButtonLabelAndData(2, String("Tonic"), nameOfKeys[scalePresetTonics[scalePresetIndex]]);

  addButtonLabelAndData(5, String("Channel"), pageChannels[instrumentPageIndex]);
  addButtonLabelAndData(6, String("Color"), nameOfColors[pageColors[instrumentPageIndex]]);
  addButtonLabelAndData(7, String("Layout"), nameOfLayouts[pageLayouts[instrumentPageIndex]]);
  addButtonLabelAndData(8, String("Octave"), pageOctaves[instrumentPageIndex]);
  showDisplayRow(0);
  showDisplayRow(1);
  showDisplayRow(2);
  showDisplayRow(3);

}

void addToDisplay(String text, byte row, byte section, byte position){
  byte byteBuffer[text.length()+1]; //create a byte array
  text.getBytes(byteBuffer,text.length()+1); //fill it with the message
  for(uint i=0;i<text.length();i++){ //move the text over to the displaybuffer
    displayBuffer[row][i+17*(section)+position]=byteBuffer[i];
  }
}

void addDisplayTop(){
  removeDisplayRow(0);
  addToDisplay(String("Scale preset: ")+(scalePresetIndex+1), 0, 0, 0);
  addToDisplay(String("Track preset: ")+(instrumentPageIndex+1), 0, 2, 0);
}

void addToDisplay8Section(String text, byte row, byte section, bool center){
  byte index=section-1;
  for(uint i=0;i<8;i++){ //clear section
    displayBuffer[row][i+9*index-index/2]=32;
  }
  byte byteBuffer[text.length()+1]; //create a byte array
  text.getBytes(byteBuffer,text.length()+1); //fill it with the message
  byte centerAlignText = 0; 
  if (text.length()<7 && center)
    centerAlignText = (4-(text.length()+1)/2);
  for(uint i=0;i<8 && i<text.length();i++){ //move the text over to the displaybuffer
    displayBuffer[row][i+9*index-index/2+centerAlignText]=byteBuffer[i];
  }
}

void addKnobLabelAndData(byte knob, String label, String value){
  addToDisplay8Section(label, 0, knob, true);
  addToDisplay8Section(value, 1, knob, true);
}
void addButtonLabelAndData(byte button, String label, String value){
  addToDisplay8Section(label, 2, button, true);
  addToDisplay8Section(value, 3, button, true);
}


void removeDisplaySection(byte row, byte section){
  for(int i=0;i<17;i++){
    displayBuffer[row][i+17*(section)]=32;
  }
}
void removeDisplayRow(byte row){
  for(int i=0;i<68;i++){
    displayBuffer[row][i]=32;
  }
}

void showDisplayRow(byte row){
  sysexDisplayBuffer[4] = row + 24; //change to correct row
  for(int i=0;i<68;i++){ // Fill the sysex with most recent data
    sysexDisplayBuffer[i+8] = displayBuffer[row][i];
  }
  midiUSB.sendSysEx(77,sysexDisplayBuffer,true); //send it
  delay(displayBugDelay);
}

////////////////////////////////////////////////////////////////
//
// Homemade USB MIDI functions because the library sucks
//
///////////////////////////////////////////////////////////////

void MIDImessage(byte command, byte MIDInote, byte MIDIvelocity) {
  Serial1.write(command);//send note on or note off command 
  Serial1.write(MIDInote);//send most segnificant byte
  Serial1.write(MIDIvelocity);//send least segnificant
}
void mySendNoteOn(byte note, byte velocity){
  MIDImessage(144, note, velocity);
}
void mySendNoteOff(byte note){
  MIDImessage(128, note, 0);
}

void mySendControlChange(byte number, byte value){
  MIDImessage(176, number, value);
}

void mySendAfterTouch(byte value){
  Serial2.write(0xD0);
  Serial2.write(value);
}

void killAllNotesUSB(){ 
  for(int i=0;i<128;i++){
    mySendNoteOff(i);
  }
}

////////////////////////////////////////////////////////////////
//
// LAYOUT GETTERS AND SETTERS
//
///////////////////////////////////////////////////////////////

//LAYOUTS

byte getXlayout(byte pageIndex){
  return layouts[pageLayouts[pageIndex]][0];
}
byte getYlayout(byte pageIndex){
  return layouts[pageLayouts[pageIndex]][1];
}
void setLayout(byte layoutIndex, byte pageIndex){
  pageLayouts[pageIndex] = layoutIndex;
}


///////////////////////////////////////////////////////////////////
//
// CALLBACKS USB MIDI
//
///////////////////////////////////////////////////////////////////


void handleNoteOnUSB(byte channel, byte pitch, byte velocity)
{
  if(pitch>35 && pitch<100){
    byte note = pushButtonIndexToAbsoluteNote(pitch-36);
    if(legalRange(note)){
      midiDIN.sendNoteOn(note,velocity,pageChannels[instrumentPageIndex]);
      pressedNotes[note]=true;
    }
    // if(internalFeedback)
    //   handleNoteOnDIN(pageChannels[instrumentPageIndex], note, velocity);
  }
}

void handleNoteOffUSB(byte channel, byte pitch, byte velocity)
{
  if(pitch>35 && pitch<100){
    byte note = pushButtonIndexToAbsoluteNote(pitch-36);
    if(legalRange(note)){
      midiDIN.sendNoteOff(note,0,pageChannels[instrumentPageIndex]);
      pressedNotes[note]=false;
    }
    // if(internalFeedback)
    //   handleNoteOffDIN(pageChannels[instrumentPageIndex], note, velocity);
  }

}

void handleAfterTouchPolyUSB(byte channel, byte note, byte pressure){
  midiDIN.sendPolyPressure(note, pressure, pageChannels[instrumentPageIndex]);
}

void handleAfterTouchChannelUSB(byte channel, byte pressure){
  //midiDIN.sendAfterTouch(pressure, pageChannels[instrumentPageIndex]);
  mySendAfterTouch(pressure);
}

void handleControlChangeUSB(byte channel, byte control, byte value){
  //SUSTAIN PEDAL
  if(control==64){
    midiDIN.sendControlChange(64,value,pageChannels[instrumentPageIndex]);
  }

  // OCTAVE CHANGE BUTTONS
  if(control==54 || control==55){
    killAllPressedNotesDIN(pageChannels[instrumentPageIndex]); // avoid hanging notes
    if(control==54 && value==127){
      pageOctaves[instrumentPageIndex]=constrain(pageOctaves[instrumentPageIndex]-1,0,8);
    }
    else if(control==55 && value==127){
      pageOctaves[instrumentPageIndex]=constrain(pageOctaves[instrumentPageIndex]+1,0,8);
    }
    eepromSet(PAGE1_OCTAVE_EEPROM_ADRESS+instrumentPageIndex,pageOctaves[instrumentPageIndex]);
    addButtonLabelAndData(8, String("Octave"), pageOctaves[instrumentPageIndex]);
    showDisplayRow(3);
    initGrid();
  }


  // TEMPO KNOB
  if(control==14){
    killAllPressedNotesDIN(pageChannels[instrumentPageIndex]); // avoid hanging notes
    if(control==14 && value<64){
      scalePresetScales[scalePresetIndex]=indexInLoopingPattern(scalePresetScales[scalePresetIndex]+1,numberOfScales);
    }
    else{
      scalePresetScales[scalePresetIndex]=indexInLoopingPattern(scalePresetScales[scalePresetIndex]-1,numberOfScales);
    }
    eepromSet(PRESET1_SCALE_EEPROM_ADRESS+scalePresetIndex,scalePresetScales[scalePresetIndex]);
    addButtonLabelAndData(1, String("Mode"), nameOfScales[scalePresetScales[scalePresetIndex]]);
    showDisplayRow(3);
    initGrid();
  }
  // // SWING KNOB
  // if(control==15){
  //   killAllNotesDIN(); // avoid hanging notes
  //   if(control==15 && value<64){
  //     scalePresetScales[scalePresetIndex]=constrain(scalePresetScales[scalePresetIndex]+1, 0, numberOfScales-1);
  //   }
  //   else{
  //     scalePresetScales[scalePresetIndex]=constrain(scalePresetScales[scalePresetIndex]-1, 0, numberOfScales-1);
  //   }
  //   addToDisplay8Section(nameOfScales[scalePresetScales[scalePresetIndex]],3,2,false);
  //   showDisplayRow(3);
  //   initGrid();
  // }
  // // ENCODER 1
  // if(control==71){
  //   if(value<64){
  //     colors[pageColors[instrumentPageIndex]][0]=constrain(colors[pageColors[instrumentPageIndex]][0]+1,0,127);
  //   }
  //   else{
  //     colors[pageColors[instrumentPageIndex]][0]=constrain(colors[pageColors[instrumentPageIndex]][0]-1,0,127);
  //   }
  //   addKnobLabelAndData(1, String("Color1"), colors[pageColors[instrumentPageIndex]][0]);
  //   showDisplayRow(0);
  //   showDisplayRow(1);
  //   initGrid();
  // }
  // // ENCODER 2
  //  if(control==72){
  //   if(value<64){
  //     colors[pageColors[instrumentPageIndex]][1]=constrain(colors[pageColors[instrumentPageIndex]][1]+1,0,127);
  //   }
  //   else{
  //     colors[pageColors[instrumentPageIndex]][1]=constrain(colors[pageColors[instrumentPageIndex]][1]-1,0,127);
  //   }
  //   addKnobLabelAndData(2, String("Color2"), colors[pageColors[instrumentPageIndex]][1]);
  //   showDisplayRow(0);
  //   showDisplayRow(1);
  //   initGrid();
  // }
  // // ENCODER 3
  // if(control==73){
  //   if(value<64){
  //     colors[pageColors[instrumentPageIndex]][2]=constrain(colors[pageColors[instrumentPageIndex]][2]+1,0,127);
  //   }
  //   else{
  //     colors[pageColors[instrumentPageIndex]][2]=constrain(colors[pageColors[instrumentPageIndex]][2]-1,0,127);
  //   }
  //   addKnobLabelAndData(3, String("Color3"), colors[pageColors[instrumentPageIndex]][2]);
  //   showDisplayRow(0);
  //   showDisplayRow(1);
  // }
 // ENCODER 4
  // if(control==74){
  //   killAllNotesDIN(); // avoid hanging notes
  //   if(value<64){
  //     pageChannels[instrumentPageIndex]=constrain(pageChannels[instrumentPageIndex]+1,1,16);
  //   }
  //  else{
  //     pageChannels[instrumentPageIndex]=constrain(pageChannels[instrumentPageIndex]-1,1,16);
  //   }
  //   addKnobLabelAndData(4, String("Channel"), pageChannels[instrumentPageIndex]);
  //   showDisplayRow(0);
  //   showDisplayRow(1);
  // }

  //  // ENCODER 5
  // if(control==75){
  //   killAllNotesDIN(); // avoid hanging notes
  //   if(value<64){
  //     pageLayouts[instrumentPageIndex]=constrain(pageLayouts[instrumentPageIndex]+1,0,numberOfLayouts-1);
  //   }
  //  else{
  //     pageLayouts[instrumentPageIndex]=constrain(pageLayouts[instrumentPageIndex]-1,0,numberOfLayouts-1);
  //   }
  //   addKnobLabelAndData(5, String("Layout"), nameOfLayouts[pageLayouts[instrumentPageIndex]]);
  //   showDisplayRow(0);
  //   showDisplayRow(1);
  //   initGrid();
  // }

  // // ENCODER 6
  // if(control==76){
  //   if(value<64){
  //     pageColors[instrumentPageIndex]=constrain(pageColors[instrumentPageIndex]+1,0,numberOfColorSchemes-1);
  //   }
  //  else{
  //     pageColors[instrumentPageIndex]=constrain(pageColors[instrumentPageIndex]-1,0,numberOfColorSchemes-1);
  //   }
  //   addKnobLabelAndData(6, String("Color"), nameOfColors[pageColors[instrumentPageIndex]]);
  //   showDisplayRow(0);
  //   showDisplayRow(1);
  //   initGrid();
  //   initOtherButtons();
  // }

  // Top BUTTON ROW - SCALE SETTINGS
  if(control==20 || control==21){
    killAllPressedNotesDIN(pageChannels[instrumentPageIndex]); // avoid hanging notes
    if(control==20 && value==127){ // change scale
      scalePresetScales[scalePresetIndex]=indexInLoopingPattern(scalePresetScales[scalePresetIndex]+1,numberOfScales);
      eepromSet(PRESET1_SCALE_EEPROM_ADRESS+scalePresetIndex,scalePresetScales[scalePresetIndex]);
      addButtonLabelAndData(1, String("Mode"), nameOfScales[scalePresetScales[scalePresetIndex]]);
      showDisplayRow(3);
      initGrid();
    }
    if(control==21 && value==127){ // Change tonic
      scalePresetTonics[scalePresetIndex]=indexInLoopingPattern(scalePresetTonics[scalePresetIndex]+1,12);
      eepromSet(PRESET1_TONIC_EEPROM_ADRESS+scalePresetIndex,scalePresetTonics[scalePresetIndex]);
      addButtonLabelAndData(2, String("Tonic"), nameOfKeys[scalePresetTonics[scalePresetIndex]]);
      showDisplayRow(3);
    }
  }

   // Top BUTTON ROW - PAGE SETTINGS
  if(control==24 || control==25 || control==26 || control==27){
    if(control==24 && value==127){
      killAllPressedNotesDIN(pageChannels[instrumentPageIndex]); // avoid hanging notes
      pageChannels[instrumentPageIndex]=loopingNumbers(pageChannels[instrumentPageIndex]+1,16);
      eepromSet(PAGE1_CHANNEL_EEPROM_ADRESS+instrumentPageIndex,pageChannels[instrumentPageIndex]);
      addButtonLabelAndData(5, String("Channel"), pageChannels[instrumentPageIndex]);
    }
    
    if(control==25 && value==127){
      pageColors[instrumentPageIndex]=indexInLoopingPattern(pageColors[instrumentPageIndex]+1,numberOfColorSchemes);
      eepromSet(PAGE1_COLOR_EEPROM_ADRESS+instrumentPageIndex,pageColors[instrumentPageIndex]);
      addButtonLabelAndData(6, String("Color"), nameOfColors[pageColors[instrumentPageIndex]]);
      initGrid();
      initPageSelectButtons();
    }

    if(control==26 && value==127){
      pageLayouts[instrumentPageIndex]=indexInLoopingPattern(pageLayouts[instrumentPageIndex]+1,numberOfLayouts);
      eepromSet(PAGE1_LAYOUT_EEPROM_ADRESS+instrumentPageIndex,pageLayouts[instrumentPageIndex]);
      addButtonLabelAndData(7, String("Layout"), nameOfLayouts[pageLayouts[instrumentPageIndex]]);
      initGrid();
    }

    if(control==27 && value==127){
      killAllPressedNotesDIN(pageChannels[instrumentPageIndex]); // avoid hanging notes
      pageOctaves[instrumentPageIndex]=indexInLoopingPattern(pageOctaves[instrumentPageIndex]+1,9);
      eepromSet(PAGE1_OCTAVE_EEPROM_ADRESS+instrumentPageIndex,pageOctaves[instrumentPageIndex]);
      addButtonLabelAndData(8, String("Octave"), pageOctaves[instrumentPageIndex]);
      initGrid();
    }
    showDisplayRow(2);
    showDisplayRow(3);
  }

// LOWER BUTTON ROW SCALE PRESETS
if(control==102 || control==103 || control==104 || control==105){
    killAllPressedNotesDIN(pageChannels[instrumentPageIndex]);
    if(value==127){
      scalePresetIndex=control-102;
      eepromSet(SCALE_PRESET_EEPROM_ADRESS,scalePresetIndex);
    }
    addDisplayTop();
    addButtonLabelAndData(1, String("Mode"), nameOfScales[scalePresetScales[scalePresetIndex]]);
    addButtonLabelAndData(2, String("Tonic"), nameOfKeys[scalePresetTonics[scalePresetIndex]]);
    initScalePresetSelectButtons();
    showDisplayRow(0);
    showDisplayRow(3);
    initGrid();
  }


    // LOWER BUTTON ROW Track select
  if(control==106 || control==107 || control==108 || control==109){
    killAllPressedNotesDIN(pageChannels[instrumentPageIndex]);
    if(value==127){
      instrumentPageIndex=control-106;
      eepromSet(PAGE_EEPROM_ADRESS,instrumentPageIndex);
    }
    addDisplayTop();
    addButtonLabelAndData(5, String("Channel"), pageChannels[instrumentPageIndex]);
    addButtonLabelAndData(6, String("Color"), nameOfColors[pageColors[instrumentPageIndex]]);
    addButtonLabelAndData(7, String("Layout"), nameOfLayouts[pageLayouts[instrumentPageIndex]]);
    addButtonLabelAndData(8, String("Octave"), pageOctaves[instrumentPageIndex]);
    showDisplayRow(0);
    showDisplayRow(3);
    initGrid();
  }
}

void handlePitchBendUSB(byte channel, int bend){
  midiDIN.sendPitchBend(bend,pageChannels[instrumentPageIndex]);
}

///////////////////////////////////////////////////////////////////
//
//CALLBACKS DIN MIDI
//
///////////////////////////////////////////////////////////////////

void handleNoteOnDIN(byte channel, byte pitch, byte velocity){
  if (channel == pageChannels[instrumentPageIndex]) {
    for(int i=0;i<64;i++){
      if((pitch)==pushButtonIndexToAbsoluteNote(i)){
        displayButton(i,true);
      }
    }
  }
}

void handleNoteOffDIN(byte channel, byte pitch, byte velocity){
  if (channel == pageChannels[instrumentPageIndex]) {
    for(int i=0;i<64;i++){
      if((pitch)==pushButtonIndexToAbsoluteNote(i)){
        displayButton(i,false);
      }
    }
  }
}


///////////////////////////////////////////////////////////////////
//
//SETUP
//
///////////////////////////////////////////////////////////////////


void setup() {
  Serial1.begin(31250);
  Serial2.begin(31250);
  Serial1.flush();
  Serial2.flush();

  midiUSB.begin(1); //the push is always sending on channel 1
  midiDIN.begin(MIDI_CHANNEL_OMNI); 
  midiUSB.turnThruOff();
  midiDIN.turnThruOff();


  //attach callbacks
  midiUSB.setHandleNoteOn(handleNoteOnUSB); // Listen for user input on the push
  midiUSB.setHandleNoteOff(handleNoteOffUSB);
  midiUSB.setHandleControlChange(handleControlChangeUSB);
  midiUSB.setHandlePitchBend(handlePitchBendUSB);
  midiUSB.setHandleAfterTouchChannel(handleAfterTouchChannelUSB);
  //midiUSB.setHandleAfterTouchPoly(handleAfterTouchPolyUSB);

  midiDIN.setHandleNoteOn(handleNoteOnDIN); // listen for note on/off feedback to visualize on the grid
  midiDIN.setHandleNoteOff(handleNoteOffDIN);

  //init display buffer
  for(int row=0;row<4;row++){
    for(int col=0;col<68;col++){
      displayBuffer[row][col]=32;
    }
  }

  // Init pressed notes memory
  for(int i=0;i<128;i++){
    pressedNotes[i] = false;
  }

  //Load EEPROM SETTINGS
  instrumentPageIndex = eepromGet(PAGE_EEPROM_ADRESS,0,numberOfPages-1,0);
  scalePresetIndex = eepromGet(SCALE_PRESET_EEPROM_ADRESS,0,numberOfScalePresets-1,0);
  for(int i=0;i<numberOfPages;i++){
    pageChannels[i]=eepromGet(PAGE1_CHANNEL_EEPROM_ADRESS+i,1,16,i+1);
    pageColors[i]=eepromGet(PAGE1_COLOR_EEPROM_ADRESS+i,0,numberOfColorSchemes-1,i);
    pageLayouts[i]=eepromGet(PAGE1_LAYOUT_EEPROM_ADRESS+i,0,numberOfLayouts-1,0);
    pageOctaves[i]=eepromGet(PAGE1_OCTAVE_EEPROM_ADRESS+i,0,8,3);
  }
  for(int i=0;i<numberOfScalePresets;i++){
    scalePresetScales[i]= eepromGet(PRESET1_SCALE_EEPROM_ADRESS+i,0,numberOfScales-1,1);
    scalePresetTonics[i]=eepromGet(PRESET1_TONIC_EEPROM_ADRESS+i,0,11,0);
  }

  //I don't understand why I need this shit, but without it I get some horrible note action
  killAllNotesUSB();

  //make connected devices hot-swappable 
  pinMode(devicePin, INPUT_PULLUP);
  debouncer.attach(devicePin);  
  debouncer.interval(50);

  //initial check for connected device
  if ( debouncer.read() ){
    initPushSettings();
    initGrid();
    initOtherButtons();
    initDisplay();
  }

}

///////////////////////////////////////////////////////////////////
//
//LOOP
//
///////////////////////////////////////////////////////////////////

void loop() {

  midiUSB.read();
  midiDIN.read();
  debouncer.update();
  if ( debouncer.rose() ){ //If a device was just connected, initialize it
    initPushSettings();
    initGrid();
    initOtherButtons();
    initDisplay();

  }
}

