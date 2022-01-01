#include <TM1638plus_Model2.h>

#define  STROBE_TM 8  // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 9  // clock = GPIO connected to clock line of module
#define  DIO_TM 10 // data = GPIO connected to data line of module

#define START_A 0
#define START_B 0

#define INPUT_A 1
#define INPUT_B 2
#define SHOW_RESULT 3

#define KEY_1 9
#define KEY_2 10
#define KEY_3 11
#define KEY_4 5
#define KEY_5 6
#define KEY_6 7
#define KEY_7 1
#define KEY_8 2
#define KEY_9 3
#define KEY_0 13
#define KEY_ADD 8
#define KEY_SUB 12
#define KEY_MUL 14
#define KEY_DIV 15
#define KEY_EQ 16
#define KEY_RESET 4

bool swap_nibbles = false; //Default is false if left out, see issues section in readme at URL
bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true. 

// Constructor object Init the module
TM1638plus_Model2 tm(STROBE_TM, CLOCK_TM , DIO_TM, swap_nibbles, high_freq);

char buf[100];

long A = START_A;
long B = START_B;
long result;
char inputState = INPUT_A;
char operation = 0;
unsigned char oldButtonsState = 0;

long inputDigit(long digit, unsigned char btn) {
    //сдвиг числа влево если нажата не сброс
  if (btn != 0 && btn!= KEY_RESET && btn!= KEY_MUL && btn!= KEY_DIV && btn!= KEY_ADD && btn!= KEY_SUB && btn!= KEY_EQ && digit <= 9999999L) {
    digit = digit * 10L;

    long add = 0;

    switch(btn) {
      case KEY_1: 
        add = 1;
        break;
      case KEY_2:
        add = 2;
        break;
      case KEY_3:
        add = 3;
        break;
      case KEY_4:
        add = 4;
        break;
      case KEY_5:
        add = 5;
        break;
      case KEY_6:
        add = 6;
        break;
      case KEY_7:
        add = 7;
        break;
      case KEY_8:
        add = 8;
        break;
      case KEY_9:
        add = 9;
        break;

    }

    digit = digit + add;
    
  }

  return digit;

}

void reset() {
    A = START_A;
    B = START_B;
    inputState = INPUT_A;
    operation = 0;
}


// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  tm.displayBegin();
  tm.DisplayStr("00000000", 0);
}

// the loop routine runs over and over again forever:
void loop() {

  unsigned char buttons = tm.ReadKey16();

  sprintf(buf, "Buttons: %d %u %u %u %d", inputState, result, A, B, buttons);

  Serial.println(buf);


  if (oldButtonsState != buttons) {
  //сброс чисел
    if (buttons == KEY_RESET) {
      reset();
    }
  
    if (inputState == INPUT_A) {
      A = inputDigit(A, buttons);
      sprintf(buf, "%08lu", A);
    } else if (inputState == INPUT_B){
      B = inputDigit(B, buttons);
      sprintf(buf, "%08lu", B);
    } else {
      sprintf(buf, "%08lu", result);
    }
    tm.DisplayStr(buf, 0);  
  
    switch(buttons) {
      case KEY_MUL:
      case KEY_DIV:
      case KEY_ADD:
      case KEY_SUB:
        operation = buttons;
        inputState = INPUT_B;
        break;
      case KEY_EQ:
        if (inputState == INPUT_B) {
          inputState = SHOW_RESULT;
         
          switch(operation) {
            case KEY_ADD:
              result = A + B;
              break;
            case KEY_SUB:
              result = A - B;
              break;
            case KEY_MUL:
              result = A * B;
              break;
            case KEY_DIV:
              if (B == 0) {
                result = 99999999L;
              } else {
                result = A / B;
              }
              break;
          }
        }
        break;
    }
  }
  
  delay(10);        // delay in between reads for stability
  oldButtonsState = buttons;
}
