#include "Drawing.h"

#define MIDI_COMMAND_NOTE_OFF             0x80

#define MIDI_COMMAND_NOTE_ON              0x90

#define MIDI_COMMAND_CONTROL_CHANGE       0xB0

#define MIDI_COMMAND_PROGRAMM_CHANGE      0xC0

class MidiController {
  public:
    MidiController();
    static void init();
    void MidiController::playNotes(Key keys[5]);

  private:
    int MidiController::noteForKey(Key key);
    int MidiController::noteForRow(uint8_t row);
};
