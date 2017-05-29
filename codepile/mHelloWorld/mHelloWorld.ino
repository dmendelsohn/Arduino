/* 
*HELLO EARTH
*5/20/2013
*Zane Boven & Faye Tran
*/

#include <ezLCD.h>                   //these are libraries that allows you to use commands defined in them
#include <SoftwareSerial.h>          //include these in all your arLCD sketches  

EzLCD3_SW lcd(10,11);        //the LCD is connected to the arduino in pins 10&11


void setup(){        //use the setup function to initialize themes, strings, and et al.
  
  lcd.begin(EZM_BAUD_RATE);    //must have in all your sketches  
  lcd.font(2);    //font 2 is one of the default fonts, check out different fonts in the manual or in ARLCD>EZSYS>FONTS  
  
  //THEMES ( ID, DARKEMBOSS, LIGHTEMBOSS, TEXTCOLOR0, TEXTCOLOR1, TEXTCOLORDISABLED, COLOR0, COLOR1, COLORDISABLED, BACKGROUND, FONTW )
  lcd.theme( 1, 5, 20, 3, 3, 3, 4, 4, 4, 2, 1);            //theme for button
  lcd.theme( 2, 155, 152, 3, 3, 3, 1, 4, 5, 0, 16);        //theme for analog meter
  lcd.theme( 3, 9, 3, 0, 0, 0, 8, 8, 8, 1, 1);            //theme for slider
  
  /* FONTW must be specified before the theme it is used in. However, there are default FONTW already initialized so FONTW's were not 
 created in this sketch. */
  
  //when colors are initialized in the parameters, make sure they are CAPITALIZED
  lcd.cls(BLACK);    //clears the whole screen to black; default color is black by not specifiying a color in the parameters
  lcd.color(WHITE);  //sets a color to write in
  
  //STRING ( ID, STRING )
  lcd.string(1, "LAUNCH");          //launch button
  lcd.string(2, "SPEED");           //speedometer---analog meter
  lcd.string(3, "HELLO EARTH");     //string for image    
  
}


void loop(){    //loop function is the main part of your program, create the widgets here
  
  //1st - Launch Sequence
  lcd.println("  Welcome to the Space Shuttle JonJon");        //some more text that will appear on the launch screen
  lcd.println("      We will launch on your command");         //alternative to using "println" you could plot a starting point
  lcd.println("                      Hold to launch");         //using lcd.xy(XPOSITION, YPOSITION) then print text with lcd.print("...")
  
  //BUTTON(ID, XPOS, Y, WIDTH, HEIGHT, OPTION, ALIGN, RADIUSofedges, THEME, STRING ID)
  lcd.button(1, 80, 80, 150, 150, 1, 0 ,75, 1, 1);
  
  while(1){
    if( lcd.isPressed(1) ){      //if the LCD is pressed, then do all that is in the parameters
                                 //in the parameter is the widget ID for the button
      lcd.cls();
      lcd.font("sans72");
      for (int i=5; i>=0; i--){    //"For loop" for the countdown
        lcd.cls();                //clears screen
        lcd.xy(145, 72);          //plots a starting point
        lcd.print(i);             //to print
        delay(1000);              //1000 milliseconds = 1 second
      }
      for(int k=0; k<=5; k++){    //loop for a "blastoff effect"
        lcd.cls(YELLOW);          //clears screen to yellow
        delay(100);               //delays for 1/10 th of a second
        lcd.cls(RED);             //clears screen to red
        delay(100);              //need delay to differentiate colors are changing
      }
      
      //2nd - Lift Off
      lcd.cls();       
      lcd.println("We have");
      lcd.println("lift off!!!!");
      delay(2000);                //delay 1 second
      
      //3rd - Start Speed Control
      lcd.cls();
      lcd.font("sans36");         //sets font
      lcd.println("");            //acts as a newline
      lcd.println("");
      lcd.println("We need to increase");  //prints text to tell user what to do 
      lcd.println("        our speed!!!"); //on the next screen
      delay(2000);                //delays 2 secs
      
      //4th - Speed Control
      lcd.cls();
      
      //ANALOG METER (ID, X, Y, WIDTH, HEIGHT, OPTION, INITIAL VALUE, MAX VAL, THEME, STRING ID, TYPE)
      // TYPE  0 = FULL, 1= HALF, 2 = QUARTER
      lcd. analogMeter( 2, 60, 15, 200, 150, 1, 900, 0, 9000, 2, 2, 0);
      
      //SLIDER ( ID, X, Y, WIDTH, HEIGHT, OPTION, MAX, INCREMENTS, INITIAL VALUE, THEME)
      lcd.slider(3, 35, 175, 250, 45, 5, 9000, 10, 900, 3);
      
      int speed = lcd.getWidgetValue(3);    //this sets the value of the slider to a variable
      while(speed <= 8000){                 //while slider value is less than or equal to 8000
        speed = lcd.getWidgetValue(3);      //get numerical value of slider
        lcd.wvalue(2, speed);               //sets analog meter to that value
      }
      
      //5th - ARRIVAL TO EARTH!!!
      lcd.cls();
      //image( FILENAME, X, Y, OPTION )
      //OPTIONS 1=align to center, 2 down scale image to screen, 3=both
      lcd.image("earth.gif", 60, 40);  //shows picture of the earth  
      lcd.font("sans36");              //selects a new font
      lcd.xy(60, 100);                 //plots a starting point
      lcd.println("HELLO EARTH!!");    //prints "HELLO EARTH!!"
      
    }
  }   
  
}
