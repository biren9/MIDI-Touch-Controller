#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"
#include "Drawing.h"
#include "TouchScreen.h"

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


void setup() {
  /* Initialize serial print related functionality */
  Serial.begin(9600);

  /* Set the Display Enable pin*/
  Serial.println("--Start Application--");
  if (BootupConfigure()) {
    //error case - do not do any thing
  }
  else {
    /*calibrate();*/
    FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);  //set mode to extended for FT801
  }
}

int32_t  wbutton, hbutton, tagval, tagoption;
int16_t yValue, xValue, pendown;
sCTouchXY cTouchXY;
/*wbutton = FT_DISPLAYWIDTH / 8;
  hbutton = FT_DISPLAYHEIGHT / 8;*/

int32_t invalidTouch = -pow(2, 15);

void loop() {
  /* Read the touch screen xy and tag from GetCTouchXY API */
  Drawing::drawGrid();
  FTImpl.GetCTouchXY(cTouchXY);

  uint8_t touchedButtonTag = FTImpl.Read( REG_TOUCH_TAG);
  Serial.println(touchedButtonTag);

  int array[5] = {};

  if (cTouchXY.x0 != invalidTouch || cTouchXY.y0 != invalidTouch) {
    /*Serial.println("Touch 0");*/
  }

  if (cTouchXY.x1 != invalidTouch || cTouchXY.y1 != invalidTouch) {
    /*Serial.println("Touch 1");*/
  }

  if (cTouchXY.x2 != invalidTouch || cTouchXY.y2 != invalidTouch) {
    /*Serial.println("Touch 2");*/
  }
  if (cTouchXY.x3 != invalidTouch || cTouchXY.y3 != invalidTouch) {
    Serial.println("Touch 3");
  }
  if (cTouchXY.x4 != invalidTouch || cTouchXY.y4 != invalidTouch) {
    Serial.println("Touch 4");
  }

  Serial.println("_________________");
}









static void Drawing::drawGrid() {
  uint16_t numberOfLines = 8;
  uint16_t numberOfRows = 8;
  uint16_t displayHeight = FT_DISPLAYHEIGHT * 16;
  uint16_t displayWidth = FT_DISPLAYWIDTH * 16;

  FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.

  FTImpl.Cmd_FGColor(0x008000);
  int32_t buttonWidth = FT_DISPLAYWIDTH / numberOfLines;
  int32_t buttonHight = FT_DISPLAYHEIGHT / numberOfRows;
  for (uint16_t line = 0; line < numberOfLines; ++line) {
    for (int row = 0; row < numberOfRows; ++row) {
      uint16_t xPos = FT_DISPLAYWIDTH / numberOfLines * line;
      uint16_t yPos = FT_DISPLAYHEIGHT / numberOfRows * row;
      FTImpl.Tag(line * numberOfLines + row);
      FTImpl.Cmd_Button(xPos, yPos, buttonWidth, buttonHight, 16, FT_OPT_FLAT, "C");
    }
  }

  FTImpl.ColorRGB(0xFF, 0xFF, 0xFF); //set the color of the line strip to white

  /* Draw lines of grid*/
  for (uint16_t line = 1; line < numberOfLines; ++line) {
    uint16_t xPos = displayWidth / numberOfLines * line;
    FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
    FTImpl.Vertex2f(xPos, 0); //starting coordinates
    FTImpl.Vertex2f(xPos, displayHeight);
    FTImpl.End();//end line strip primitive
  }

  /* Draw rows of grid*/
  for (int row = 1; row < numberOfRows; ++row) {
    uint16_t yPos = displayHeight / numberOfRows * row;
    FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
    FTImpl.Vertex2f(0, yPos); //starting coordinates
    FTImpl.Vertex2f(displayWidth, yPos);
    FTImpl.End();//end line strip primitive
  }

  FTImpl.DLEnd();//end the display list
  FTImpl.Finish();//render the display list and wait for the completion of the DL
}





struct TouchScreen::Position {
  int16_t x;
  int16_t y;
};

struct TouchScreen::Key {
  uint8_t tag;
  uint8_t xValue;
  uint8_t yValue;
};

TouchScreen::Key TouchScreen::selectedKey(Position position) {
  Key key = Key();
  key.tag = 0;
  key.xValue = 0;
  key.yValue = 0;
  return key;
}
