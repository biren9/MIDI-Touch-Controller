#include "Drawing.h"

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
    void MidiController::setOktave(int oktave);
    int MidiController::currentOktave();

    uint8_t MidiController::currentControllerX();
    void MidiController::setControllerX(uint8_t controllerX);
    uint8_t MidiController::currentControllerY();
    void MidiController::setControllerY(uint8_t controllerY);

    void MidiController::playNotes(Key keys[5]);
    static Note MidiController::noteForKey(Key key);
    static char* MidiController::noteForLineRow(uint8_t line, uint8_t row);

  private:
    void MidiController::controlChange(uint8_t control, double value);
    void MidiController::noteOn(uint8_t index);
    void MidiController::noteOff(uint8_t index);
};
