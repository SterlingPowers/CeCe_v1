/* 
 * Project MP3 Player Test
 * Author: Brian Rashap
 * Date: 10-APR-2024
 */

/* Wiring Note
 * The MP3 Player is accessed through the Particle's UART pins (SERIAL1)
 * Connect the Tx of the Particle is wired to the Rx of the MP3 player
 * and visa-versoe for the Rx pin.
 * 
 * There is a fritzing diagram of the proper wiring in the class_materials/ParticleCode respository
*/

#include "Particle.h"
#include "DFRobotDFPlayerMini.h"
#include "Button.h"

DFRobotDFPlayerMini myDFPlayer;
Button nextButton(D0);
unsigned int lastSong;

// Declare Functions
void printDetail(uint8_t type, int value);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);
  Serial1.begin(9600);
  delay(1000);

  Serial.printf("DFRobot DFPlayer Mini Demo\n");
  Serial.printf("Initializing DFPlayer ... (May take 3~5 seconds)\n");
  
  if (!myDFPlayer.begin(Serial1)) {  //Use softwareSerial to communicate with mp3.
    Serial.printf("Unable to begin:\n");
    Serial.printf("1.Please recheck the connection!\n");
    Serial.printf("2.Please insert the SD card!\n");
    while(true);
  }
  Serial.printf("DFPlayer Mini online.\n");
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);     //Play the first track on the uSD card
  
  //The below code should loop each track, may or may not work
  //myDFPlayer.loop(1);
  //myDFPlayer.enableLoopAll();
}

void loop() {
  if(nextButton.isClicked()) {
    Serial.printf("Next Song\n");
    myDFPlayer.next();  /// myDFPlayter.prev() for previous track
  }
}

// MP3 Player Diagnostics
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.printf("Time Out!\n");
      break;
    case WrongStack:
      Serial.printf("Stack Wrong!\n");
      break;
    case DFPlayerCardInserted:
      Serial.printf("Card Inserted!\n");
      break;
    case DFPlayerCardRemoved:
      Serial.printf("Card Removed!\n");
      break;
    case DFPlayerCardOnline:
      Serial.printf("Card Online!\n");
      break;
    case DFPlayerPlayFinished:
      Serial.printf("Number %i Play Finished!\n",value);
      break;
    case DFPlayerError:
      Serial.printf("DFPlayerError:\n");
      switch (value) {
        case Busy:
          Serial.printf("Card not found\n");
          break;
        case Sleeping:
          Serial.printf("Sleeping\n");
          break;
        case SerialWrongStack:
          Serial.printf("Get Wrong Stack\n");
          break;
        case CheckSumNotMatch:
          Serial.printf("Check Sum Not Match\n");
          break;
        case FileIndexOut:
          Serial.printf("File Index Out of Bound\n");
          break;
        case FileMismatch:
          Serial.printf("Cannot Find File\n");
          break;
        case Advertise:
          Serial.printf("In Advertise\n");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
