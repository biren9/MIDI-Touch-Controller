#include "MidiController.h"
#include "Settings.h"
#include <arduino.h>

#define EFFECT_LEVEL 7
#define EFFECT_PAN 10
#define EFFECT_DELAY 72
#define EFFECT_CUTOFF 74


Key currentPlayingKeys[5];
int currentSelectedOktave = DEFAULT_OKTAVE;

MidiController::MidiController() {
  Serial3.begin(31250);
}

void MidiController::setOktave(int oktave) {
  currentSelectedOktave = oktave;

  for (int currentPlayingIndex = 0; currentPlayingIndex < 5; ++currentPlayingIndex) {
    if (currentPlayingKeys[currentPlayingIndex].tag != 0) {
      Serial3.write(MIDI_COMMAND_NOTE_ON | 0);
      Serial3.write(0x7F & noteForKey(currentPlayingKeys[currentPlayingIndex]).value);
      Serial3.write(0x7F & 0);
    }
  }
}

int MidiController::currentOktave() {
  return currentSelectedOktave;
}

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
      Serial3.write(0x7F & noteForKey(currentPlayingKeys[currentPlayingIndex]).value);
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
      Serial3.write(0x7F & noteForKey(keys[keyIndex]).value);
      Serial3.write(0x7F & 127);
      currentPlayingKeys[keyIndex] = keys[keyIndex];
    }
  }

  for (int keyIndex = 4; keyIndex >= 0; --keyIndex) {
    if (keys[keyIndex].xValue != -1 && keys[keyIndex].yValue != -1) {
      controlChange(EFFECT_CUTOFF, keys[keyIndex].xValue);
      controlChange(EFFECT_PAN, keys[keyIndex].yValue);
      break;
    }
  }
}

void MidiController::controlChange(uint8_t control, double value) {
  Serial3.write(MIDI_COMMAND_CONTROL_CHANGE | 0);
  Serial3.write(control);
  Serial3.write((int)(value * 127));
}

static Note MidiController::noteForKey(Key key) {
  uint16_t i = key.line + key.row * numberOfLines;
  Note note;
  switch (i % 12) {
    case 0:
      note = {0, "C"};
      break;
    case 1:
      note = {2, "D"};
      break;
    case 2:
      note = {4, "E"};
      break;
    case 3:
      note = {5, "F"};
      break;
    case 4:
      note = {7, "G"};
      break;
    case 5:
      note = {9, "A"};
      break;
    case 6:
      note = {11, "B"};
      break;
    case 7:
      note = {12, "C"};
      break;
    default:
      note = {0, "C"};
      break;
  }
  note.value = note.value + 12 * key.oktave;
  return note;
}

static char* MidiController::noteForLineRow(uint8_t line, uint8_t row) {
  Note note = noteForKey({0, line, row, 0, -1, -1});
  return note.text;
}
