
void drawGrid() {
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
      FTImpl.Cmd_Button(xPos, yPos, buttonWidth,buttonHight,16,FT_OPT_FLAT,"A");
      Serial.println(xPos);
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
