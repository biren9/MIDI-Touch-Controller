#include "MidiController.h"
#include <arduino.h>

MidiController::MidiController() {
  Serial3.begin(31250);
}


Key currentPlayingKeys[5];

void MidiController::playNotes(Key keys[5]) {

  for (int currentPlayingIndex = 0; currentPlayingIndex < 5; ++currentPlayingIndex) {
    bool hasFound = false;
    for (int keyIndex = 0; keyIndex < 5; ++keyIndex) {
      if (keys[keyIndex].tag == currentPlayingKeys[currentPlayingIndex].tag) {
        hasFound = true;
        break;
      }
    }

    if (!hasFound) {
      Serial3.write(MIDI_COMMAND_NOTE_ON | 0);
      Serial3.write(0x7F & noteForKey(currentPlayingKeys[currentPlayingIndex]));
      Serial3.write(0x7F & 0);
      currentPlayingKeys[currentPlayingIndex] = Key();
    }
  }


  for (int keyIndex = 0; keyIndex < 5; ++keyIndex) {
    bool hasFound = false;
    for (int currentPlayingIndex = 0; currentPlayingIndex < 5; ++currentPlayingIndex) {
      if (keys[keyIndex].tag == currentPlayingKeys[currentPlayingIndex].tag) {
        hasFound = true;
        break;
      }
    }

    if (!hasFound && keys[keyIndex].tag != 0) {
      Serial3.write(MIDI_COMMAND_NOTE_ON | 0);
      Serial3.write(0x7F & noteForKey(keys[keyIndex]));
      Serial3.write(0x7F & 127);
      currentPlayingKeys[keyIndex] = keys[keyIndex];      
    }
  }

controlChange(74, keys[0].xValue);
  
}

#define EFFECT_LEVEL 7
#define EFFECT_PAN 10
#define EFFECT_CUTOFF 74

void MidiController::controlChange(uint8_t control, double value) {
  Serial3.write(MIDI_COMMAND_CONTROL_CHANGE | 0);
  Serial3.write(control);
  Serial3.write((int)(value*127));
  Serial.println(value);
}


static Note MidiController::noteForRow(uint8_t row) {
  switch (row) {
    case 0:
      return {0, "C"};
    case 1:
      return {2, "D"};
    case 2:
      return {4, "E"};
    case 3:
      return {5, "F"};
    case 4:
      return {7, "G"};
    case 5:
      return {9, "A"};
    case 6:
      return {11, "B"};
    case 7:
      return {12, "C"};
    default:
      return {0, "C"};
  }
}


int MidiController::noteForKey(Key key) {
  return noteForRow(key.row).value + 12 * oktave;
}
