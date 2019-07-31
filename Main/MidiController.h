#include "Drawing.h"

#define oktave 6

#define MIDI_COMMAND_NOTE_OFF             0x80

#define MIDI_COMMAND_NOTE_ON              0x90

#define MIDI_COMMAND_CONTROL_CHANGE       0xB0

#define MIDI_COMMAND_PROGRAMM_CHANGE      0xC0

typedef struct {
  int value;
  const char* text;
} Note;

class MidiController {
  public:
    MidiController();
    static void init();
    void MidiController::playNotes(Key keys[5]);
    static Note MidiController::noteForRow(uint8_t row);

  private:
    int MidiController::noteForKey(Key key);
    void MidiController::controlChange(uint8_t control, double value);
};
