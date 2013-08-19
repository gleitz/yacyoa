#include <SPI.h>


//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

SFEMP3Shield MP3player;

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
    Serial.begin(9600);
    MP3player.begin();
    MP3player.SetVolume(2, 2);
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
