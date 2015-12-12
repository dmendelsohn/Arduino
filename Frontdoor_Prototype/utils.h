const int BLACK = 0;
const int WHITE = 1;
const int RED = 2;
const int GREEN = 3;
const int BLUE = 4;
const int TEAL = 5;
const int YELLOW = 6;
const int PURPLE = 7;

const int ID_LENGTH = 4;
const int NAME_LENGTH = 16;
const int LINE_LENGTH = ID_LENGTH + NAME_LENGTH + 1; //A line in the file consists of {id + TAB + name}

const byte lcd_clock_pin = 0;
const byte lcd_data_pin = 1;
const byte lcd_latch_pin = 2;
const byte red_pin = 6; // defines digital out pin for red LCD backlight
const byte green_pin = 5; // defines digital out pin for green LCD backlight
const byte blue_pin = 3; // defines digital out pin for blue LCD backlight
const byte sd_pin = 4;

char USER_FILEPATH[] = "CARDID.txt";

byte sigma[8] = { //defines character for sigma
  B11111,
  B10000,
  B01000,
  B00100,
  B01000,
  B10000,
  B11111,
  B00000
};

typedef struct {
  char name[NAME_LENGTH+1]; //Null terminated char array of length 17
  char id[ID_LENGTH+1]; //Null terminated char array of length 5
} user_t;

//Save regular messages in the program memory to save var memory space
prog_char msgs[][17] PROGMEM = 
{
  "Initializing... ",  //0
  "Updated: 1/2015 ",  //1
  "No Response     ",  //2
  "Welcome to      ",  //3
  "Welcome brother ",  //4
  "User Not        ",  //5
  "Authorized      ",  //6
  "                ",  //7
  "User Already    ",  //8
  "Exists          ",  //9
  "Could Not Find  ",  //10
  "Removed         ",  //11
  "Couldn't Read   ",  //12
  "Tap Card to Add ",  //13
  "Add Successful  ",  //14
  "Can't Connect   ",  //15
  "To Back Door    ",  //16
  "Time Out        ",  //17
  "User Not Added  "  //18
};
