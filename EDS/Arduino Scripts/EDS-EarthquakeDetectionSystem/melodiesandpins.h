/* This is the library of pre-assigned notes and pins in a manner.*/

//pre-defined pins and melodies
#define buzzerPin 12
#define toneNumber 3
#define F1 349
#define G2 440
#define A3 392

int notes[] = {F1, G2, A3};
int xsample=0;
int ysample=0;
int zsample=0;
long start;
int buz=0;


//pre-defined pins
#define ledPin 8
#define xPin A0
#define yPin A1
#define zPin A2
#define samples 20
#define maxVal 10   // max change limit
#define minVal -10    // min change limit
#define buzTime 1500  // buzzer on time
