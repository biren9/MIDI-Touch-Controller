#include "Settings.h"
#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"
#include "Drawing.h"
#include "MidiController.h"


FT801IMPL_SPI FTImpl(FT_CS_PIN, FT_PDN_PIN, FT_INT_PIN);

MidiController midi;

int32_t  wbutton, hbutton, tagval, tagoption;
int16_t yValue, xValue, pendown;
sCTouchXY cTouchXY;
/*wbutton = FT_DISPLAYWIDTH / 8;
  hbutton = FT_DISPLAYHEIGHT / 8;*/

int32_t invalidTouch = -pow(2, 15);

Key keys[5];
bool hasOktaveIncreased = false;
bool hasOktaveDecreased = false;

bool hasControllerXSet = false;
int controllerXValue;
bool hasControllerYSet = false;
int controllerYValue;

/* Api to bootup FT801, verify FT801 hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t touchScreenConfigure() {
  uint32_t chipid = 0;
  FTImpl.Init(FT_DISPLAY_RESOLUTION);//configure the display to the WQVGA

  delay(20);//for safer side
  chipid = FTImpl.Read32(FT_ROM_CHIPID);

  /* Identify the chip */
  if (FT801_CHIPID != chipid) {
    Serial.print("Error in chip id read ");
    Serial.println(chipid, HEX);
    return 1;
  }

  /* Set the Display & audio pins */
  FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
  FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN);
  FTImpl.DisplayOn();
  FTImpl.AudioOn();
  FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);
  return 0;
}

void setup() {
  Serial.begin(9600);
  if (touchScreenConfigure()) {
    //error case - do not do any thing
    Serial.println("Error occured");
  }
 
  midi = MidiController::MidiController();
  controllerXValue = midi.currentControllerX();
  controllerYValue = midi.currentControllerY();

  pinMode(PORT_OKTAVE_INCREASE, INPUT_PULLUP);
  pinMode(PORT_OKTAVE_DECREASE, INPUT_PULLUP);
  pinMode(PORT_X_Controller, INPUT);
  pinMode(PORT_Y_Controller, INPUT);
  pinMode(PORT_X_ENABLE, INPUT_PULLUP);
  pinMode(PORT_Y_ENABLE, INPUT_PULLUP);

  Serial.println("End Setup");
}

void loop() {
  /* reset selected keys */
  for (int i=0; i < 5; ++i) {
    keys[i] = {0, 0, 0, 0, -1, -1};  
  }
  
  FTImpl.GetCTouchXY(cTouchXY);

  // Map the touch events to a key struct and store them for the highlighting
  int currentOktave = midi.currentOktave();
  if (cTouchXY.x0 != invalidTouch || cTouchXY.y0 != invalidTouch) {
    Position pos = {cTouchXY.x0, cTouchXY.y0};
    Key key = Drawing::selectedKey(pos, currentOktave);
    keys[0] = key;
  }

  if (cTouchXY.x1 != invalidTouch || cTouchXY.y1 != invalidTouch) {
    Position pos = {cTouchXY.x1, cTouchXY.y1};
    Key key = Drawing::selectedKey(pos, currentOktave);
    keys[1] = key;
    Serial.println("Touch 1");
  }

  if (cTouchXY.x2 != invalidTouch || cTouchXY.y2 != invalidTouch) {
    Position pos = {cTouchXY.x2, cTouchXY.y2};
    Key key = Drawing::selectedKey(pos, currentOktave);
    keys[2] = key;
    Serial.println("Touch 2");
  }
  if (cTouchXY.x3 != invalidTouch || cTouchXY.y3 != invalidTouch) {
    Position pos = {cTouchXY.x3, cTouchXY.y3};
    Key key = Drawing::selectedKey(pos, currentOktave);
    keys[3] = key;
    Serial.println("Touch 3");
  }
  if (cTouchXY.x4 != invalidTouch || cTouchXY.y4 != invalidTouch) {
    Position pos = {cTouchXY.x4, cTouchXY.y4};
    Key key = Drawing::selectedKey(pos, currentOktave);
    keys[4] = key;
    Serial.println("Touch 4");
  }

  midi.playNotes(keys);
  readPins();
}

void readPins() {
  /* Handle oktave increase */
  int oktavePinValue = digitalRead(PORT_OKTAVE_INCREASE);
  Serial.println(oktavePinValue);
  if (oktavePinValue == 0) {
    int newOktave = midi.currentOktave() + 1;
    if (newOktave <= 7 && hasOktaveIncreased == false) {
      midi.setOktave(newOktave);
      hasOktaveIncreased = true;
    }
  } else {
    hasOktaveIncreased = false;
  }

  /* Handle oktave decrease */
  oktavePinValue = digitalRead(PORT_OKTAVE_DECREASE);
  if (oktavePinValue == 0) {
    int newOktave = midi.currentOktave() - 1;
    if (newOktave >= 0 && hasOktaveDecreased == false) {
      midi.setOktave(newOktave);
      hasOktaveDecreased = true;
    }
  } else {
    hasOktaveDecreased = false;
  }

  MenuInformation information = MenuInformation();

  if (digitalRead(PORT_X_ENABLE) == 0) {
    hasControllerXSet = false;
    controllerXValue = smoothAnalog(PORT_X_Controller);
  } else if (!hasControllerXSet) {
    hasControllerXSet = true;
    midi.setControllerX(controllerXValue);
  }


  if (digitalRead(PORT_Y_ENABLE) == 0) {
    hasControllerYSet = false;
    controllerYValue = smoothAnalog(PORT_Y_Controller);
  } else if (!hasControllerYSet) {
    hasControllerYSet = true;
    midi.setControllerY(controllerYValue);
  }

  /* Update Menu */
  information.oktave = midi.currentOktave();
  information.controllerX = controllerXValue;
  information.controllerY= controllerYValue;
  Drawing::drawGrid(keys, information);
}

int smoothAnalog(int port) {
  int i;
  int value = 0;
  int numReadings = 10;

  for (i = 0; i < numReadings; i++) {
    value = value + analogRead(port);
    delay(1); // 1ms pause adds more stability between reads.
  }

  value = value / numReadings;
  value = value >> 3;
  return value;
}








//################################################
//################################################
//############ Drawing implementation ############
//################################################
//################################################
// Due to a bug in the FT_NHD_43CTP_SHIELD Library.
// For more infos, see the documentation

static void Drawing::drawGrid(Key keys[5], MenuInformation information) {

  uint16_t displayHeight = FT_DISPLAYHEIGHT * 16;
  uint16_t displayWidth = FT_DISPLAYWIDTH * 16;
  uint16_t menuOffset = MENU_WIDTH;

  FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.

  /* DRAW the buttons */
  int32_t buttonWidth = (FT_DISPLAYWIDTH - menuOffset) / numberOfLines;
  int32_t buttonHight = FT_DISPLAYHEIGHT / numberOfRows;

  for (uint16_t line = 0; line < numberOfLines; ++line) {
    for (int row = 0; row < numberOfRows; ++row) {
      uint16_t xPos = (FT_DISPLAYWIDTH - menuOffset) / numberOfLines * line + menuOffset;
      uint16_t yPos = FT_DISPLAYHEIGHT / numberOfRows * row;
      uint8_t tag = line * numberOfLines + row + 1;

      FTImpl.Tag(tag);
      FTImpl.Cmd_FGColor(0x008000);

      for (int k = 0; k < 5; ++k) {
        if (keys[k].tag == tag) {
          FTImpl.Cmd_FGColor(0x900000);
          break;
        }
      }

      const char* text = MidiController::noteForLineRow(line, row);
      FTImpl.Cmd_Button(xPos, yPos, buttonWidth, buttonHight, 31, FT_OPT_FLAT, text);
    }
  }

  FTImpl.ColorRGB(0xFF, 0xFF, 0xFF); //set the color of the line strip to white

  /* Draw lines of grid*/
  for (uint16_t line = 1; line <= numberOfLines; ++line) {
    uint16_t xPos = (((FT_DISPLAYWIDTH - menuOffset) * 16) / (float)numberOfLines) * line + menuOffset * 16;

    FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
    FTImpl.Vertex2f(xPos, 0); //starting coordinates
    FTImpl.Vertex2f(xPos, displayHeight);
    FTImpl.End();//end line strip primitive
  }

  /* Draw rows of grid*/
  for (int row = 1; row < numberOfRows; ++row) {
    uint16_t yPos = displayHeight / numberOfRows * row;
    FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
    FTImpl.Vertex2f(menuOffset * 16, yPos); //starting coordinates
    FTImpl.Vertex2f(displayWidth, yPos);
    FTImpl.End();//end line strip primitive
  }


  /* DRAW Menu */
  FTImpl.Cmd_Number(10, 10, 31, 0, information.oktave);
  FTImpl.Cmd_Number(10, 110, 28, 0, information.controllerX);
  FTImpl.Cmd_Number(10, 210, 28, 0, information.controllerY);

  FTImpl.DLEnd();//end the display list
  FTImpl.Finish();//render the display list and wait for the completion of the DL
}


static Key Drawing::selectedKey(Position position, int oktave) {
  uint16_t menuOffset = MENU_WIDTH;

  if (position.x < menuOffset) {
    return {0, 0, 0, 0, -1, -1};
  }
  
  Key key = Key();
  int32_t buttonWidth = (FT_DISPLAYWIDTH - menuOffset) / numberOfLines;
  int32_t buttonHight = FT_DISPLAYHEIGHT / numberOfRows;
  uint8_t selectedLine = (position.x - menuOffset) / buttonWidth;
  uint8_t selectedRow = position.y / buttonHight;
  double xValue = (double)((position.x - menuOffset) - selectedLine * buttonWidth) / (double)buttonWidth;
  double yValue = (double)(position.y - selectedRow * buttonHight) / (double)buttonHight;

  key.tag = selectedLine * numberOfLines + selectedRow + 1;
  key.xValue = xValue;
  key.yValue = yValue;
  key.oktave = oktave;
  key.line = selectedLine;
  key.row = selectedRow;
  return key;
}
