#define PINS_PER_TEENSEY 5

#define POSITION_TYPE int16_t
#define FORCE_TYPE int16_t
#define TOUCH_TYPE boolean

typedef union{
  position_int POSITION_TYPE;
  HB byte;
  LB byte;
} position_t;

typedef union{
  force_int FORCE_TYPE;
  HB byte;
  LB byte;
}force_t;

typedef union{
  touched_boolean TOUCHED_TYPE;
  LB byte;
}touched_t;

typedef struct{
  position_t;
  force_t;
  touched_t;
}phyxel_t;

typedef union{
  phyxel_t[NUM_PINS];
  byte[NUM_PINS * sizeof(phyxel_t)];
}frame_t;

