#include "Settings.h"
#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"
#include "Drawing.h"
#include "MidiController.h"


FT801IMPL_SPI FTImpl(FT_CS_PIN, FT_PDN_PIN, FT_INT_PIN);

/* Api to bootup FT801, verify FT801 hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure() {
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
  return 0;
}

/* API for calibration on FT801 */
void calibrate() {
  /*************************************************************************/
  /* Below code demonstrates the usage of calibrate function. Calibrate    */
  /* function will wait untill user presses all the three dots. Only way to*/
  /* come out of this api is to reset the coprocessor bit.                 */
  /*************************************************************************/

  /* Construct the display list with grey as background color, informative string "Please Tap on the dot" followed by inbuilt calibration command */
  FTImpl.DLStart();
  FTImpl.ClearColorRGB(64, 64, 64);
  FTImpl.Clear(1, 1, 1);
  FTImpl.ColorRGB(0xff, 0xff, 0xff);
  FTImpl.Cmd_Text((FT_DISPLAYWIDTH / 2), (FT_DISPLAYHEIGHT / 2), 27, FT_OPT_CENTER, "Please Tap on the dot");
  FTImpl.Cmd_Calibrate(0);

  /* Wait for the completion of calibration - either finish can be used for flush and check can be used */
  FTImpl.Finish();
}

/* bootup the module and display primitives on screen */


MidiController midi;

void setup() {
  /* Initialize serial print related functionality */
  Serial.begin(9600);

  /* Set the Display Enable pin*/
  Serial.println("--Start Application--");
  if (BootupConfigure()) {
    //error case - do not do any thing
    Serial.println("Error");
  }
  else {
    //calibrate();
    FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);  //set mode to extended for FT801
  }
  midi = MidiController::MidiController();

  pinMode(PORT_OKTAVE_INCREASE, INPUT_PULLUP);
  pinMode(PORT_OKTAVE_DECREASE, INPUT_PULLUP);
  
  Serial.println("End Setup");
}

int32_t  wbutton, hbutton, tagval, tagoption;
int16_t yValue, xValue, pendown;
sCTouchXY cTouchXY;
/*wbutton = FT_DISPLAYWIDTH / 8;
  hbutton = FT_DISPLAYHEIGHT / 8;*/

int32_t invalidTouch = -pow(2, 15);


Key keys[5];
bool hasOktaveIncreased = false;
bool hasOktaveDecreased = false;

void loop() {
  /* Update Menu */
  MenuInformation information = MenuInformation();
  information.oktave = midi.currentOktave();
  Drawing::drawGrid(keys, information);

  /* reset selected keys */
  keys[0] = {0,0,0,0,-1,-1};
  keys[1] = {0,0,0,0,-1,-1};
  keys[2] = {0,0,0,0,-1,-1};
  keys[3] = {0,0,0,0,-1,-1};
  keys[4] = {0,0,0,0,-1,-1};
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

  /* Handle oktave increase */
  int oktavePinValue = digitalRead(PORT_OKTAVE_INCREASE);
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

      for(int k=0; k < 5; ++k) {
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
    uint16_t xPos = (((FT_DISPLAYWIDTH - menuOffset) * 16) / (float)numberOfLines) * line + menuOffset*16;
    
    FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
    FTImpl.Vertex2f(xPos, 0); //starting coordinates
    FTImpl.Vertex2f(xPos, displayHeight);
    FTImpl.End();//end line strip primitive
  }

  /* Draw rows of grid*/
  for (int row = 1; row < numberOfRows; ++row) {
    uint16_t yPos = displayHeight / numberOfRows * row;
    FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
    FTImpl.Vertex2f(menuOffset*16, yPos); //starting coordinates
    FTImpl.Vertex2f(displayWidth, yPos);
    FTImpl.End();//end line strip primitive
  }


  /* DRAW Menu */
  FTImpl.Cmd_Number(10, 10, 31, 0, information.oktave);

  FTImpl.DLEnd();//end the display list
  FTImpl.Finish();//render the display list and wait for the completion of the DL
}


static Key Drawing::selectedKey(Position position, int oktave) {
  uint16_t menuOffset = MENU_WIDTH;  
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
  key.row= selectedRow;
  return key;
}
