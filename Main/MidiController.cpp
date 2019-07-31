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


int MidiController::noteForRow(uint8_t row) {
  switch (row) {
    case 0:
      return 0;
    case 1:
      return 2;
    case 2:
      return 3;
    case 3:
      return 4;
    case 4:
      return 5;
    case 5:
      return 7;
    case 6:
      return 9;
    case 7:
      return 11;
    default:
      return 0;
  }
}


int MidiController::noteForKey(Key key) {
  return noteForRow(key.row) + 12 * (key.line + 2);
}
