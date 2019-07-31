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

    if (!hasFound) {
      Serial3.write(MIDI_COMMAND_NOTE_ON | 0);
      Serial3.write(0x7F & noteForKey(keys[keyIndex]));
      Serial3.write(0x7F & 127);
      currentPlayingKeys[keyIndex] = keys[keyIndex];
    }
  }  
}


int MidiController::noteForKey(Key key) {
  return key.row + 12*(key.line+2);
}
