/*****************************************************************
   Electronic Voting Machine.ino

    By: Raihanuzzaman Rasel
    Date: October 25, 2018
    
              GT-521F52 
             

   
  ---------------------------------------------------------------
*****************************************************************/


#include "FPS_GT511C3.h"
#include "SoftwareSerial.h" //#include <SoftwareSerial.h>
#include <LiquidCrystal.h>




//SoftwareSerial fps(6, 7); // (Arduino SS_RX = pin 6, Arduino SS_TX = pin 7)
FPS_GT511C3 fps(6, 7);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);



// FPS (TX) is connected to pin 10 (Arduino's Software RX)
// FPS (RX) is connected through a converter to pin 11 (Arduino's Software TX)
//SoftwareSerial fps(10, 11); // (Arduino SS_RX = pin 10, Arduino SS_TX = pin 11)


// Constants analog a0 to a5
const int bp1 = 14;
const int bp2 = 15;
const int bp3 = 16;
const int bp4 = 18;
const int bp5 = 17;
const int bp6 = 19;

// States
bool bs1 = 0;
bool bs2 = 0;
bool bs3 = 1;
bool bs4 = 0;
bool bs5 = 0;
bool bs6 = 0;

int voterID = -1;

bool isVoted[50] = {false};
int x = -1;
int dispState = 1;

int candidate0 = 0;
int candidate1 = 0;
int candidate2 = 0;

void setup()
{
  
  Serial.begin(9600); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  

  //Defining Buttons
  pinMode(bp1, INPUT);
  pinMode(bp2, INPUT);
  pinMode(bp3, INPUT);
  pinMode(bp4, INPUT);
  pinMode(bp5, INPUT);
  pinMode(bp6, INPUT);

  
}

void loop()
{
  delay(1);
  switch (dispState) {
    case 1:
      disp1();
      dispState = 0;
      break;
  }
  // Checking the button States.
  bs1 = digitalRead(bp1);
  bs2 = digitalRead(bp2);
  bs6 = digitalRead(bp6);
  Serial.print(bs1);
  Serial.print(bs2);
  Serial.print(bs3);
  Serial.print(bs4);
  Serial.print(bs5);
  Serial.println(bs6);

  if (bs1 == HIGH) {
    dispEnroll();
    Enroll();
    dispState = 1;
  }

  if (bs2 == HIGH) {
    x = myVote();

    if (isVoted[x]) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You have voted");
      lcd.setCursor(0, 1);
      lcd.print("before.");
      delay(3000);
      dispState = 1;
    } else {
      selectCandidate();
      dispState = 1;
    }
  }

  if (bs6 == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Trump Putin Kim");
    lcd.setCursor(1, 1);
    lcd.print(candidate0);
    lcd.setCursor(7, 1);
    lcd.print(candidate1);
    lcd.setCursor(13, 1);
    lcd.print(candidate2);
    delay(7000);
    dispState = 1;
  }


  //checkVoted(x);
}


void selectCandidate() {
  for (; !isVoted[x];) {
    dispCandidates();
    bs3 = digitalRead(bp3);
    bs4 = digitalRead(bp4);
    bs5 = digitalRead(bp5);
    if (bs3 == 1) {
      delay(500);
      candidate0++;
      isVoted[x] = true;
    }
    if (bs4 == 1) {
      delay(500);
      candidate1++;
      isVoted[x] = true;
    }
    if (bs5 == 1) {
      delay(500);
      candidate2++;
      isVoted[x] = true;
    }
  }
  lcd.clear();
}


void disp1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EVM by 150921,37");
  lcd.setCursor(0, 1);
  lcd.print("Enroll:1  Vote:2");
}

void dispEnroll() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enrolling finger");
  lcd.setCursor(0, 1);
}

void dispVoting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Getting data....");
  lcd.setCursor(0, 1);
}

void dispCandidates() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Trump Putin Kim");
  lcd.setCursor(0, 1);
  lcd.print("  1     2    3");
}


void checkVoted() {
  for (int i = 0; i < 50; i++) {
    if (isVoted[i])
      Serial.println(i);
  }
}

int myVote() {
  fps.Open();
  fps.SetLED(true);
  int retID = -1;
  while (voterID < 10000) {
    voterID = Vote();
  }
  retID = voterID - 10000;
  delay(3000);
  fps.SetLED(false);
  voterID = -1;
  return retID;
}


void Enroll()
{
  // Enroll test
  fps.Open();
  fps.SetLED(true);
  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid == true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  Serial.print("Press finger to Enroll #");
  dispEnroll();
  lcd.print("Press finger");
  Serial.println(enrollid);
  while (fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove finger");
    dispEnroll();
    lcd.print("Remove finger");
    fps.Enroll1();
    while (fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    dispEnroll();
    lcd.print("Finger again");
    while (fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      dispEnroll();
      lcd.print("Remove finger");
      fps.Enroll2();
      while (fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      dispEnroll();
      lcd.print("Finger yet again");
      while (fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        dispEnroll();
        lcd.print("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successful");
          dispEnroll();
          lcd.print("Success!!  #");
          lcd.print(enrollid);
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          dispEnroll();
          lcd.print("Failed!! -_-");
          Serial.println(iret);
        }
      }
      else Serial.println("Failed to capture third finger");
    }
    else Serial.println("Failed to capture second finger");
  }
  else Serial.println("Failed to capture first finger");


  delay(5000);
  fps.SetLED(false);
}


int Vote() {
  int id = -1;
  int myID = 0;
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);
    id = fps.Identify1_N();

    if (id < 3000) //<- change id value depending model you are using
    { //if the fingerprint matches, provide the matching template ID
      Serial.print("Verified ID:");

      if (id == 200) {
        Serial.println("Not Found!");
        dispVoting();
        lcd.print("Not Found!");
        myID = -1;
      } else {
        Serial.println(id);
        dispVoting();
        lcd.print("Data matched!");
        myID = id + 10000;
      }

    }
    else
    { //if unable to recognize
      Serial.println("Finger not found");
      dispVoting();
      lcd.print("No match found!");
      myID = -1;
    }
  }
  else
  {
    Serial.println("Please press finger");
    dispVoting();
    lcd.print("Press finger...");
  }
  delay(1000);
  return myID;
}

