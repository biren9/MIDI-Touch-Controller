#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"

/* Global object for FT801 Implementation */
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
  drawGrid();
  FTImpl.GetCTouchXY(cTouchXY);

  if (cTouchXY.x0 != invalidTouch || cTouchXY.y0 != invalidTouch) {
    Serial.println("Touch 0");
  }

  if (cTouchXY.x1 != invalidTouch || cTouchXY.y1 != invalidTouch) {
    Serial.println("Touch 1");
  }

  if (cTouchXY.x2 != invalidTouch || cTouchXY.y2 != invalidTouch) {
    Serial.println("Touch 2");
  }
  if (cTouchXY.x3 != invalidTouch || cTouchXY.y3 != invalidTouch) {
    Serial.println("Touch 3");
  }
  if (cTouchXY.x4 != invalidTouch || cTouchXY.y4 != invalidTouch) {
    Serial.println("Touch 4");
  }

  Serial.println("_________________");
}
