#include <SPI.h>
#include <FreeStack.h>

//Add the SdFat Libraries
#include <SdFat.h>

//and the MP3 Shield Library
#include <vs1053_SdFat.h>

SdFat sd;
vs1053 MP3player;

byte result;

int needToPrint = 0;
int needToPrintAll = 0;
int count;
int totalNumber = 0;
int system_reset = 0;
int in = 12;
int in_reset = 5;
int lastState = LOW;
int trueState = LOW;
int lastState_reset = HIGH;
int trueState_reset = HIGH;
long lastStateChangeTime = 0;
long lastStateChangeTime_reset = 0;
int stoppedDialingMs = 3000;
int dialHasFinishedRotatingAfterMs = 100;
int debounceDelay = 10;

void setup()
{
    pinMode(in, INPUT);
    Serial.begin(115200);

    //Initialize the SdCard.
    if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
    // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
    if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

    //Initialize the MP3 Player Shield
    result = MP3player.begin();
    //check result, see readme for error codes.
    if(result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to start MP3 player"));
      if( result == 6 ) {
        Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
        Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
      }
    }
    MP3player.setVolume(2, 2);
    Serial.println("done");
}

void loop()
{


    int reading = digitalRead(in);
    int reading_reset = digitalRead(in_reset);

    if ((millis() - lastStateChangeTime) > stoppedDialingMs) {
        if (needToPrintAll) {
            Serial.println("total number is " + String(totalNumber));
            MP3player.stopTrack();
            if (totalNumber == 1980) {
                totalNumber = 2;
            }
            if (totalNumber == 0) {
                totalNumber = 10;
            }
            MP3player.playTrack(totalNumber);
            totalNumber = 0;
            needToPrintAll = 0;
        }
    }

    if ((millis() - lastStateChangeTime) > dialHasFinishedRotatingAfterMs) {
        if (needToPrint) {
            Serial.println(count % 10);
            totalNumber = 10 * totalNumber + (count % 10);
            Serial.println(totalNumber);
            needToPrint = 0;
            needToPrintAll = 1;
            count = 0;
        }
    }

    if (reading != lastState) {
        lastStateChangeTime = millis();
    }
    if ((millis() - lastStateChangeTime) > debounceDelay) {
        if (reading != trueState) {
            trueState = reading;
            if (trueState == HIGH) {
                count++;
                needToPrint = 1;
            }
        }
    }
    lastState = reading;

    // reset the phone by hanging up
    if (reading_reset != lastState_reset) {
        lastStateChangeTime_reset = millis();
    }
    if ((millis() - lastStateChangeTime_reset) > debounceDelay) {
        if (reading_reset != trueState_reset) {
            trueState_reset = reading_reset;
            if (trueState_reset == LOW) {
                Serial.println("reset the phone");
                MP3player.stopTrack();
                MP3player.playTrack(1);
            } else {
                MP3player.stopTrack();
            }
        }
    }
    lastState_reset = reading_reset;
}
