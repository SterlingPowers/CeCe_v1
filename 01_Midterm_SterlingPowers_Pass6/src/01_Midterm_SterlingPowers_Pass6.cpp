/* 
 * Project HueCeRock
 * Author: Sterling Powers
 * Date: 03_05_25
 */

 #include "Particle.h"
 #include "MP3_CNM.h"
 #include "hue.h"
 #include "Encoder.h"
 #include "Button.h"
 
 #define ENCODER_CLK D5
 #define ENCODER_DT D4
 #define ENCODER_SW D3
 
 DFRobotDFPlayerMini myDFPlayer;
 Button encoderButton(ENCODER_SW);
 Encoder volumeEncoder(ENCODER_CLK, ENCODER_DT);
 
 const int BULBS[] = {1, 2, 3, 4, 5, 6}; // Six bulbs
 const int numBulbs = 6;
 int volume = 15;
 long lastEncoderPos = 0;
 int currentColor = 0;
 unsigned long lastColorChange = 0;
 const int colorChangeInterval = 1000; // 1 second
 bool isPlaying = false;
 
 SYSTEM_MODE(MANUAL);
 
 void setup() {
     Serial.begin(9600);
     waitFor(Serial.isConnected, 10000);
     Serial1.begin(9600);
     delay(1000);
     
     Serial.printf("Are you ready to RAWK?!\n");
     Serial.printf("Initializing DFPlayer ... (May take 3~5 seconds)\n");
     
     if (!myDFPlayer.begin(Serial1)) {
         Serial.printf("Unable to begin:\n1. Please recheck the connection!\n2. Please insert the SD card!\n");
         while(true);
     }
     Serial.printf("Let's RAWK!\n");
     
     myDFPlayer.volume(volume);  //Set initial volume
     myDFPlayer.loop(1);  //Play first mp3
     myDFPlayer.enableLoopAll();
     isPlaying = true;
     
     WiFi.on();
     WiFi.setCredentials("IoTNetwork");
     WiFi.connect();
     while (WiFi.connecting()) {
         Serial.printf(".");
     }
     Serial.printf("\n\n");
 
     // Turn lights on when DFPlayer starts playing
     for (int i = 0; i < numBulbs; i++) {
         setHue(BULBS[i], true, currentColor, map(volume, 0, 95, 0, 255), 255); // Brightness scaled with volume
     }
 }
 
 void loop() {
     // Handle encoder for volume and brightness control
     long newPosition = volumeEncoder.read();
     if (newPosition != lastEncoderPos) {
         lastEncoderPos = newPosition;
         volume = constrain(map(newPosition, 0, 95, 0, 30), 0, 30); // Map encoder positions to DFPlayer volume
         myDFPlayer.volume(volume);
         Serial.printf("Loudness: %d\n", volume);
 
         // Adjust brightness of all lights to match volume
         for (int i = 0; i < numBulbs; i++) {
             setHue(BULBS[i], true, currentColor, map(volume, 0, 30, 0, 255), 255); // Sync all lights
         }
     }
     
     // Handle button press for next song
     if (encoderButton.isClicked()) {
         Serial.printf("Next Song\n");
         myDFPlayer.next();
         isPlaying = true;
     }
     
     // Sync all bulbs with the same color every second
     if (isPlaying && millis() - lastColorChange > colorChangeInterval) {
         lastColorChange = millis();
         currentColor = (currentColor + 500) % 65535; // Change color smoothly
         Serial.printf("Changing color: %d\n", currentColor);
         for (int i = 0; i < numBulbs; i++) {
             setHue(BULBS[i], true, currentColor, map(volume, 0, 30, 0, 255), 255);
         }
     }
     
     // Check if DFPlayer is still playing
     if (myDFPlayer.readState() != 1) {  // 1 means playing
         if (isPlaying) {
             Serial.printf("Music stopped. Turning off lights.\n");
             for (int i = 0; i < numBulbs; i++) {
                 setHue(BULBS[i], false, currentColor, 0, 255); // Turn off lights
             }
             isPlaying = false;
         }
     }
 }
 