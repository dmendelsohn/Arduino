//D0 = DP1
//D1 = C1 
//D2 = D1
//D3 = E1
//D4 = F1
//D5 = G1
//D6 = A1
//D7 = B1

//Digit 1 ordering: A B C D E F G DP
//Matches to pins : 6 7 9 2 3 4 5 8

int digit0_pin[7] = {6, 7, 9, 2, 3, 4, 5};
int digit1_pin[7] = {15, 16, 13, 11, 10, 14, 12}; //TODO: connect
int dp_pin = 8;
int seg_lookup[10][7] = {
/*int make0[7] =*/ {1, 1, 1, 1, 1, 1, 0},
/*int make1[7] =*/ {0, 1, 1, 0, 0, 0, 0},
/*int make2[7] =*/ {1, 1, 0, 1, 1, 0, 1},
/*int make3[7] =*/ {1, 1, 1, 1, 0, 0, 1},
/*int make4[7] =*/ {0, 1, 1, 0, 0, 1, 1},
/*int make5[7] =*/ {1, 0, 1, 1, 0, 1, 1},
/*int make6[7] =*/ {1, 0, 1, 1, 1, 1, 1},
/*int make7[7] =*/ {1, 1, 1, 0, 0, 0, 0},
/*int make8[7] =*/ {1, 1, 1, 1, 1, 1, 1},
/*int make9[7] =*/ {1, 1, 1, 0, 0, 1, 1}
};
/*int seg_lookup[10][7] = {make0, make1, make2, make3, make4, make5,
                        make6, make7, make8, make9};*/

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW); 
  }
  digitalWrite(dp_pin, HIGH); // Decimal point is always on
  
  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
}

void customWrite(int pin, int seg) {
  if (pin <= 13) {
    digitalWrite(pin, seg);
  } else if (pin == 14) {
    analogWrite(A1, 255*seg);
  } else if (pin == 15) {
    analogWrite(A2, 255*seg);
  } else if (pin == 16) {
    analogWrite(A3, 255*seg);
  }
}

void write_to_digit(int digit, int number) {
  int pin;
  for (int i = 0; i < 7; i++) {
   if (digit == 0)
     pin = digit0_pin[i];
   else
     pin = digit1_pin[i];
   int seg = seg_lookup[number][i];
   customWrite(pin, seg);
  }
}

void loop() {
  /*for (int i = 0; i < 10; i++) {
    write_to_digit(0, i);
    delay(500);
  }*/
  int value = analogRead(A0);
  float voltage = float(value)/1023 * 5.0;
  int first_digit = int(voltage);
  int second_digit = int(10*(voltage - first_digit));
  write_to_digit(0, first_digit);
  write_to_digit(1, second_digit);
}
