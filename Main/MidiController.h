#include "Drawing.h"

#define MIDI_COMMAND_NOTE_OFF             0x80

#define MIDI_COMMAND_NOTE_ON              0x90

#define MIDI_COMMAND_CONTROL_CHANGE       0xB0

#define MIDI_COMMAND_PROGRAMM_CHANGE      0xC0

#define DEFAULT_OKTAVE 3

typedef struct {
  int value;
  const char* text;
} Note;

class MidiController {
  public:
    MidiController();
    void MidiController::setOktave(int oktave);
    int MidiController::currentOktave();

    void MidiController::playNotes(Key keys[5]);
    static Note MidiController::noteForKey(Key key);
    static Note MidiController::noteForLineRow(uint8_t line, uint8_t row);

  private:
    void MidiController::controlChange(uint8_t control, double value);
};
