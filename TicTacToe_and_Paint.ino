
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>

#include <SPI.h>
#include <SD.h> 
#define SD_CS     10
char *name = "Please Calibrate.";  //edit name of shield

const int TS_LEFT=127,TS_RT=903,TS_TOP=940,TS_BOT=93;
#define YP A3
#define XM A2
#define YM 9 
#define XP 8 

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
uint8_t Orientation = 0;    //PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


#define BMPIMAGEOFFSET 54
#define BUFFPIXEL      20
#define PALETTEDEPTH   0


int gameArr[9];
int gameChoice;
int turn = 1; //1 for husky and 2 for basketball
int gameover = 0;
int stat = 0;

void show_tft(void)
{
    tft.setCursor(0, 0);
    tft.setTextSize(1);
    tft.print(F("ID=0x"));
    tft.println(ID, HEX);
    tft.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
    tft.println("");
    tft.setTextSize(2);
    tft.println(name);
    tft.setTextSize(1);
    tft.println("PORTRAIT Values:");
    tft.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
    tft.println("TOP  = " + String(TS_TOP)  + " BOT = " + String(TS_BOT));
    tft.println("\nWiring is: ");
    tft.println("YP=" + String(YP)  + " XM=" + String(XM));
    tft.println("YM=" + String(YM)  + " XP=" + String(XP));
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.setCursor((tft.width() - 48) / 2, (tft.height() * 2) / 4);
    tft.print("EXIT");
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(0, (tft.height() * 6) / 8);
    tft.print("Touch screen for loc");
    while (1) {
        tp = ts.getPoint();
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        if (tp.z < MINPRESSURE || tp.z > MAXPRESSURE) continue;
        if (tp.x > 450 && tp.x < 570  && tp.y > 450 && tp.y < 570) break;
        tft.setCursor(0, (tft.height() * 3) / 4);
        tft.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "   ");
    }
}

void initDrawing()
{
    tft.setRotation(Orientation);
    BOXSIZE = tft.width() / 6;
    tft.fillScreen(BLACK);
    tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
    tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
    tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, GREEN);
    tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, CYAN);
    tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, BLUE);
    tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, MAGENTA);
    tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
    currentcolor = RED;
    delay(1000);
}

int whoWon() // 0=not yet 1=husky 2=basketball 3=tie
{
  int tie = 1;
  for (int i = 0 ; i < 9 ; i++)
  {
    if (gameArr[i] == 0) tie = 0;
  }
  if (tie == 1) return 3;

  if (gameArr[0] == gameArr[1] && gameArr[1] == gameArr[2] && gameArr[0] != 0) return gameArr[0];
  if (gameArr[3] == gameArr[4] && gameArr[4] == gameArr[5] && gameArr[3] != 0) return gameArr[3];
  if (gameArr[6] == gameArr[7] && gameArr[7] == gameArr[8] && gameArr[6] != 0) return gameArr[6];
  if (gameArr[0] == gameArr[3] && gameArr[3] == gameArr[6] && gameArr[0] != 0) return gameArr[0];
  if (gameArr[1] == gameArr[4] && gameArr[4] == gameArr[7] && gameArr[1] != 0) return gameArr[1];
  if (gameArr[2] == gameArr[5] && gameArr[5] == gameArr[8] && gameArr[2] != 0) return gameArr[2];
  if (gameArr[0] == gameArr[4] && gameArr[4] == gameArr[8] && gameArr[0] != 0) return gameArr[0];
  if (gameArr[2] == gameArr[4] && gameArr[4] == gameArr[6] && gameArr[2] != 0) return gameArr[2];
  return 0;
}
int gamePick()
{
    tft.fillRect(0, 0, tft.width(), tft.height()/2, BLUE);
    tft.fillRect(0, tft.height()/2, tft.width(), tft.height()/2, MAGENTA);
    tft.drawRect(0, 0, tft.width(), tft.height()/2, WHITE);
    tft.drawRect(0, tft.height()/2, tft.width(), tft.height()/2, WHITE);
    tft.setCursor(29, (tft.height() * 3) / 4 - 10) ;
    tft.setTextColor(YELLOW);
    tft.setTextSize(4);
    tft.print("Tic Tac Toe");
    tft.setCursor(97, (tft.height() / 4 - 10)) ;
    tft.print("Paint");
   
    while(1)
      {
        tp = ts.getPoint();
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        if (tp.z < MINPRESSURE || tp.z > MAXPRESSURE) continue;
        Serial.println("X=" + String(tp.x)  + " Y=" + String(tp.y));
        
        if (tp.y > 525 && tp.y < 940)
        {
          tft.drawRect(0, 0, tft.width(), tft.height()/2, RED);
          delay(500);
          return 1;
          
        }
        else if (tp.y > 90 && tp.y < 520)  
        {
          tft.drawRect(0, tft.height()/2, tft.width(), tft.height()/2, RED);
          delay(500);
          return 2;
        }
      }
}
int pressedPos()
{
  
  TSPoint mapPt;
  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) 
  {
        switch (Orientation) {
            case 0:
                mapPt.x = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
                mapPt.y = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
                break;
            case 1:
                mapPt.x = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
                mapPt.y = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
                break;
            case 2:
                mapPt.x = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
                mapPt.y = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
                break;
            case 3:
                mapPt.x = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
                mapPt.y = map(tp.y, TS_LEFT, TS_RT, 0, tft.height());
                break;
        }
        Serial.print(mapPt.x);
        Serial.print(" ");
        Serial.print(mapPt.y);
        Serial.println();
        
//        if (mapPt.x > 0 && mapPt.x < 80  && mapPt.y > 0 && mapPt.y < 80) return 0;
//        else if (mapPt.x > 80 && mapPt.x < 160  && mapPt.y > 0 && mapPt.y < 80) return 1;
//        else if (mapPt.x > 160 && mapPt.x < 240  && mapPt.y > 0 && mapPt.y < 80) return 2;
//        else if (mapPt.x > 0 && mapPt.x < 80  && mapPt.y > 80 && mapPt.y < 160) return 3;
//        else if (mapPt.x > 80 && mapPt.x < 160  && mapPt.y > 80 && mapPt.y < 160) return 4;
//        else if (mapPt.x > 160 && mapPt.x < 240  && mapPt.y > 80 && mapPt.y < 160) return 5;
//        else if (mapPt.x > 0 && mapPt.x < 80  && mapPt.y > 160 && mapPt.y < 240) return 6;
//        else if (mapPt.x > 80 && mapPt.x < 160  && mapPt.y > 160 && mapPt.y < 240) return 7;
//        else if (mapPt.x > 160 && mapPt.x < 240  && mapPt.y > 160 && mapPt.y < 240) return 8;
//        else return -1;
        if (mapPt.x > 0 && mapPt.x < 106  && mapPt.y > 0 && mapPt.y < 106) return 0;
        else if (mapPt.x > 106 && mapPt.x < 212  && mapPt.y > 0 && mapPt.y < 106) return 1;
        else if (mapPt.x > 212 && mapPt.x < 320  && mapPt.y > 0 && mapPt.y < 106) return 2;
        else if (mapPt.x > 0 && mapPt.x < 106  && mapPt.y > 106 && mapPt.y < 212) return 3;
        else if (mapPt.x > 106 && mapPt.x < 212  && mapPt.y > 106 && mapPt.y < 212) return 4;
        else if (mapPt.x > 212 && mapPt.x < 320  && mapPt.y > 106 && mapPt.y < 212) return 5;
        else if (mapPt.x > 0 && mapPt.x < 106  && mapPt.y > 212 && mapPt.y < 320) return 6;
        else if (mapPt.x > 106 && mapPt.x < 212  && mapPt.y > 212 && mapPt.y < 320) return 7;
        else if (mapPt.x > 212 && mapPt.x < 320  && mapPt.y > 212 && mapPt.y < 320) return 8;
        else return -1;
        
        
        
      
        
  }
}

void clearBottom()
{
  tft.fillRect(0, tft.width(), tft.width(), tft.height() - tft.width() - 40, BLACK);
}
void initGame()
{
  tft.fillScreen(WHITE);
  tft.drawRect(0, 0, tft.width(), tft.width(), BLACK);
  tft.drawRect(tft.width() / 3, 0, tft.width() / 3 , tft.width(), BLACK);
  tft.drawRect(0, tft.width() / 3, tft.width() , tft.width() / 3, BLACK);
  tft.fillRect(0, tft.width(), tft.width(), tft.height() - tft.width(), BLACK);//clear bottom
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(0, tft.width()) ;
  tft.print("It's player 1 turn");
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(0, tft.height() - 40) ;
  tft.print("This is a two player Tic Tac Toe Game");
  tft.setCursor(0, tft.height() - 30) ;
  tft.print("Developed by Ayman Braik");
  tft.setCursor(0, tft.height() - 20) ;
  tft.print("Supervised by Prof. Jhon Chandy");
  tft.setCursor(0, tft.height() - 10) ;
  tft.print("Electrical and Computer Engineering Dept");
  for (int i = 0 ; i < 9 ; i++){
      gameArr[i] = 0;
  }

}
void printPic(int player, int pos)
{
  int xPos, yPos;
  switch (pos)
  {
    case 0:
    xPos = 3;
    yPos = 3;
    break;
    case 1:
    xPos = 109;
    yPos = 3;
    break;
    case 2:
    xPos = 217;
    yPos = 2;
    break;
    case 3:
    xPos = 3;
    yPos = 109;
    break;
    case 4:
    xPos = 109;
    yPos = 109;
    break;
    case 5:
    xPos = 217;
    yPos = 109;
    break;
    case 6:
    xPos = 3;
    yPos = 217;
    break;
    case 7:
    xPos = 109;
    yPos = 217;
    break;
    case 8:
    xPos = 217;
    yPos = 217;
    break; 
  }
  if (player == 1) showBMP("/husky.bmp", xPos, yPos);
  else showBMP("/ball.bmp", xPos, yPos);
      
}
void gameInput(int choice, int player)
{
  
  if (gameArr [choice] != 0 || choice == -1) return;
  if (gameArr [choice] == 0) 
  {
    gameArr [choice] = player;
    printboard();
    printPic(turn, choice);
    if (turn == 1) turn = 2;
    else if (turn == 2) turn = 1;
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(0, tft.width()) ;
    clearBottom();
    tft.print("It's player " + String(turn) + " turn");
    
    
  }
}
void printboard(){
    for (int i = 0 ; i < 9 ; i++)
  {
      Serial.print(gameArr[i]);
      Serial.print(", ");  
  }
    Serial.println("");
}
void gameHandler(int mode)
{
  if (mode == 0) return;
  if (mode == 1)//husky won
  {
    clearBottom();
    tft.setTextSize(2);
    tft.setCursor(0, tft.width()) ;
    tft.print("Husky Won!");
    
    
    
  }
  if (mode == 2)//basketball won
  {
    clearBottom();
    tft.setTextSize(2);
    tft.setCursor(0, tft.width()) ;
    tft.print("Basketball Won!");
  }
  if (mode == 3)//tie
  {
    clearBottom();
    tft.setTextSize(2);
    tft.setCursor(0, tft.width()) ;
    tft.print("It's a tie :(");
    
  }
}
void setup(void)
{
    uint16_t tmp;
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    Serial.begin(9600);
    SD.begin(SD_CS);
    SD.open("/");

    gameChoice = gamePick();
    
    if (gameChoice == 2)
    {
      initGame();
//      tp.x = 531;
      tp.y = 1100;
    }
    
    if (gameChoice == 1) 
    {
      initDrawing();
    }
 
}

void loop()
{
  
    uint16_t xpos, ypos;  //screen coordinates
    
    
    
if (gameChoice == 2){

  int choice = -1;
  choice = pressedPos();
  //Serial.println(choice);
  gameInput(choice, turn);
  stat = whoWon();
  
  if (stat != 0)
  {
    gameHandler(stat);
    //if(ts.pressure() > 50 && ts.pressure() < 500) initGame();
    delay(3500);
    
    tp.x = 531;
    tp.y = 1200;
    turn = 1;
    initGame();
  }
  else 
  {
    tp = ts.getPoint();   //tp.x, tp.y are ADC values
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  }
  
  
}
if (gameChoice == 1)
{    
    tp = ts.getPoint();   //tp.x, tp.y are ADC values
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        // most mcufriend have touch (with icons) that extends below the TFT
        // screens without icons need to reserve a space for "erase"
        // scale the ADC values from ts.getPoint() to screen values e.g. 0-239
        //
        // Calibration is true for PORTRAIT. tp.y is always long dimension 
        // map to your current pixel orientation
        switch (Orientation) {
            case 0:
                xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
                ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
                break;
            case 1:
                xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
                ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
                break;
            case 2:
                xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
                ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
                break;
            case 3:
                xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
                ypos = map(tp.y, TS_LEFT, TS_RT, 0, tft.height());
                break;
        }

        // are we in top color box area ?
        if (ypos < BOXSIZE) {               //draw white border on selected color box
            oldcolor = currentcolor;

            if (xpos < BOXSIZE) {
                currentcolor = RED;
                tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 2) {
                currentcolor = YELLOW;
                tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 3) {
                currentcolor = GREEN;
                tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 4) {
                currentcolor = CYAN;
                tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 5) {
                currentcolor = BLUE;
                tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, WHITE);
            } else if (xpos < BOXSIZE * 6) {
                currentcolor = MAGENTA;
                tft.drawRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, WHITE);
            }

            if (oldcolor != currentcolor) { //rub out the previous white border
                if (oldcolor == RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
                if (oldcolor == YELLOW) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
                if (oldcolor == GREEN) tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, GREEN);
                if (oldcolor == CYAN) tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, CYAN);
                if (oldcolor == BLUE) tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, BLUE);
                if (oldcolor == MAGENTA) tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, MAGENTA);
            }
        }
        // are we in drawing area ?
        if (((ypos - PENRADIUS) > BOXSIZE) && ((ypos + PENRADIUS) < tft.height())) {
            tft.fillCircle(xpos, ypos, PENRADIUS, currentcolor);
        }
        // are we in erase area ?
        // Plain Touch panels use bottom 10 pixels e.g. > h - 10
        // Touch panels with icon area e.g. > h - 0
        if (ypos > tft.height() - 10) {
            // press the bottom of the screen to erase
            tft.fillRect(0, BOXSIZE, tft.width(), tft.height() - BOXSIZE, BLACK);
        }
    }
}

}
uint16_t read16(File& f) {
    uint16_t result;         // read little-endian
    f.read((uint8_t*)&result, sizeof(result));
    return result;
}

uint32_t read32(File& f) {
    uint32_t result;
    f.read((uint8_t*)&result, sizeof(result));
    return result;
}

uint8_t showBMP(char *nm, int x, int y)
{
    File bmpFile;
    int bmpWidth, bmpHeight;    // W+H in pixels
    uint8_t bmpDepth;           // Bit depth (currently must be 24, 16, 8, 4, 1)
    uint32_t bmpImageoffset;    // Start of image data in file
    uint32_t rowSize;           // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel in buffer (R+G+B per pixel)
    uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
    uint8_t bitmask, bitshift;
    boolean flip = true;        // BMP is stored bottom-to-top
    int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
    uint32_t pos;               // seek position
    boolean is565 = false;      //

    uint16_t bmpID;
    uint16_t n;                 // blocks read
    uint8_t ret;

    if ((x >= tft.width()) || (y >= tft.height()))
        return 1;               // off screen

    bmpFile = SD.open(nm);      // Parse BMP header
    bmpID = read16(bmpFile);    // BMP signature
    (void) read32(bmpFile);     // Read & ignore file size
    (void) read32(bmpFile);     // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);       // Start of image data
    (void) read32(bmpFile);     // Read & ignore DIB header size
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    n = read16(bmpFile);        // # planes -- must be '1'
    bmpDepth = read16(bmpFile); // bits per pixel
    pos = read32(bmpFile);      // format
    if (bmpID != 0x4D42) ret = 2; // bad ID
    else if (n != 1) ret = 3;   // too many planes
    else if (pos != 0 && pos != 3) ret = 4; // format: 0 = uncompressed, 3 = 565
    else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH) ret = 5; // palette 
    else {
        bool first = true;
        is565 = (pos == 3);               // ?already in 16-bit format
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
        if (bmpHeight < 0) {              // If negative, image is in top-down order.
            bmpHeight = -bmpHeight;
            flip = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x + w) >= tft.width())       // Crop area to be loaded
            w = tft.width() - x;
        if ((y + h) >= tft.height())      //
            h = tft.height() - y;

        if (bmpDepth <= PALETTEDEPTH) {   // these modes have separate palette
            bmpFile.seek(BMPIMAGEOFFSET); //palette is always @ 54
            bitmask = 0xFF;
            if (bmpDepth < 8)
                bitmask >>= bmpDepth;
            bitshift = 8 - bmpDepth;
            n = 1 << bmpDepth;
            lcdbufsiz -= n;
            palette = lcdbuffer + lcdbufsiz;
            for (col = 0; col < n; col++) {
                pos = read32(bmpFile);    //map palette to 5-6-5
                palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
            }
        }

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
        for (row = 0; row < h; row++) { // For each scanline...
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            uint8_t r, g, b, *sdptr;
            int lcdidx, lcdleft;
            if (flip)   // Bitmap is stored bottom-to-top order (normal BMP)
                pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            else        // Bitmap is stored top-to-bottom
                pos = bmpImageoffset + row * rowSize;
            if (bmpFile.position() != pos) { // Need seek?
                bmpFile.seek(pos);
                buffidx = sizeof(sdbuffer); // Force buffer reload
            }

            for (col = 0; col < w; ) {  //pixels in row
                lcdleft = w - col;
                if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
                for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) { // buffer at a time
                    uint16_t color;
                    // Time to read more pixel data?
                    if (buffidx >= sizeof(sdbuffer)) { // Indeed
                        bmpFile.read(sdbuffer, sizeof(sdbuffer));
                        buffidx = 0; // Set index to beginning
                        r = 0;
                    }
                    switch (bmpDepth) {          // Convert pixel from BMP to TFT format
                        case 24:
                            b = sdbuffer[buffidx++];
                            g = sdbuffer[buffidx++];
                            r = sdbuffer[buffidx++];
                            color = tft.color565(r, g, b);
                            break;
                        case 16:
                            b = sdbuffer[buffidx++];
                            r = sdbuffer[buffidx++];
                            if (is565)
                                color = (r << 8) | (b);
                            else
                                color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
                            break;
                        case 1:
                        case 4:
                        case 8:
                            if (r == 0)
                                b = sdbuffer[buffidx++], r = 8;
                            color = palette[(b >> bitshift) & bitmask];
                            r -= bmpDepth;
                            b <<= bmpDepth;
                            break;
                    }
                    lcdbuffer[lcdidx] = color;

                }
                tft.pushColors(lcdbuffer, lcdidx, first);
                first = false;
                col += lcdidx;
            }           // end cols
        }               // end rows
        tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); //restore full screen
        ret = 0;        // good render
    }
    bmpFile.close();
    return (ret);
}
