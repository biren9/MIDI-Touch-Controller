#include "SPI.h"
#include "Wire.h"

/* Touchscreen display*/
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

/* API to display points, lines and rectangles on the screen */
void Primitives() {
  uint16_t LineHeight = 40;

  FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.


  /* display list for line strip */
  FTImpl.ColorRGB(0xFF, 0xFF, 0xFF); //set the color of the line strip to white
  FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive
  FTImpl.Vertex2f(((FT_DISPLAYWIDTH / 5) - (LineHeight / 2)) * 16, ((FT_DISPLAYHEIGHT - LineHeight) * 2 / 3) * 16); //starting coordinates
  FTImpl.Vertex2f(((FT_DISPLAYWIDTH / 5) - (LineHeight / 2)) * 16, ((FT_DISPLAYHEIGHT + LineHeight) * 2 / 3) * 16);
  FTImpl.Vertex2f(((FT_DISPLAYWIDTH / 5) + (LineHeight / 2)) * 16, ((FT_DISPLAYHEIGHT - LineHeight) * 2 / 3) * 16);
  FTImpl.Vertex2f(((FT_DISPLAYWIDTH / 5) + (LineHeight / 2)) * 16, ((FT_DISPLAYHEIGHT + LineHeight) * 2 / 3) * 16); //ending coordinates
  FTImpl.End();//end line strip primitive

  FTImpl.DLEnd();//end the display list
  FTImpl.Finish();//render the display list and wait for the completion of the DL
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
    Primitives();
  }
  Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop() {
  //
}
