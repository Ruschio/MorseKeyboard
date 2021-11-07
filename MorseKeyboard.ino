#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define RX_PIN 5           // bt module TXD pin
#define TX_PIN 6           // bt module RXD pin
#define BTN_PIN 8          // input button pin
#define BUZ_PIN 11         // output buzzer pin
#define DOT_LENGTH 80      // dot length in milliseconds
#define DASH_LENGTH 240    // dash length in milliseconds
#define LETTER_INT 400     // letter interval in milliseconds
#define WORD_INT 600       // word interval in milliseconds
#define DICT_SIZE 94       // size of dictonary

long t3,t4;                // time variables
String code = "---------"; // string in which one alphabet is stored

// Dictonary
String dict[DICT_SIZE] = {".-", "A", "-...", "B", "-.-.", "C", "-..", "D", ".", "E", "..-.", "F", "--.", "G", "....", "H", "..", "I", ".---", "J", "-.-", "K", ".-..", "L", "--", "M", 
                          "-.", "N", "---", "O", ".--.", "P", "--.-", "Q", ".-.", "R", "...", "S", "-", "T", "..-", "U", "...-", "V", ".--", "W", "-..-", "X", "-.--", "Y", "--..", "Z", 
                          "-----", "0", ".----", "1", "..---", "2", "...--", "3", "....-", "4", ".....", "5", "-....", "6", "--...", "7", "---..", "8", "----.", "9",
                          ".-.-.-", ".", "--..--", ",", "---...", ":", "..--..", "?", "-.-.--", "!", "-...-", "=", "-....-", "-", "-.--.", "(", "-.--.-", ")", "-..-.", "/", ".--.-.", "@"
                         };

LiquidCrystal_I2C lcd(0x3f, 16, 2);
SoftwareSerial bluetooth(RX_PIN, TX_PIN, false);

/*
 * COMPONENT SETUP
 */
void setup() {
  Serial.begin(9600);
  bluetooth.begin(115200);
  pinMode(BUZ_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(16, 0);
  lcd.autoscroll();
}

/*
 * MAIN LOOP
 */
void loop() {
  while (digitalRead(BTN_PIN) == LOW) {}
  code = "";                                // reset code to blank string
  t3 = millis();
  while(reader()) {}                        // read button input
  translator();                             // function to decipher code into alphabet
  t4 = millis();
}

/*
 * READ INPUT SIGNAL
 */
boolean reader() {
  long t1 = millis();                       // time when button is pressed
  tone(BUZ_PIN, 1000);                      // turn on the buzzer
  while (digitalRead(BTN_PIN) == HIGH) {}
  long t2 = millis();                       // time when button is released
  noTone(BUZ_PIN);                          // turn off the buzzer
  long len = t2 - t1;                       // time for which button is pressed
  
  if (len >= DASH_LENGTH) code += '-';      // if signal length is equal or more than dash_length, it's a dash
  else if (len >= DOT_LENGTH) code += '.';  // if signal length is equal or more than dot_length, it's a dot
  
  while ((millis() - t2) < LETTER_INT) {    // if time between inputs is less than letter interval, it's same letter     
    if (digitalRead(BTN_PIN) == HIGH) return true;
  }
  return false;                             // if time between inputs is greater than letter interval, it's different letter
}

/*
 * TRANSLATE MORSE TO ALPHABET
 */
void translator() {
  if (code.length() == 0) return;
  int i = 0;
  while (i < DICT_SIZE) {                   // compare input with dictonary
    if (dict[i].equals(code)) {
      if (t4 > 0 && t3-t4 > WORD_INT) {     // if time between input is greater than word interval, print a space
        lcd.print(" ");
        bluetooth.write(" ");
        Serial.print(" ");
      }
      int size = dict[i+1].length() + 1;    
      char letter[size]; 
      dict[i+1].toCharArray(letter, size);  // convert string to char array
      lcd.print(letter);                    // print letter on lcd  screen
      bluetooth.write(letter);              // send letter through bluetooth
      Serial.print(letter);                 // print letter on serial monitor 
      break;
    }
    i += 2;
  }
  if (i == DICT_SIZE) Serial.println("<Wrong input>"); // if input code doesn't match any letter, error
}
