/*
 * Playa Online
 * A Telephone System for Burning Man
 *
 * Benjamin Gleitzman (gleitz@mit.edu)
 */


#include <SPI.h>
#include <avr/pgmspace.h>
// Interaction with the SD card
#include <SdFat.h>
#include <SdFatUtil.h>
// MP3 Shield Library
#include <SFEMP3Shield.h>

SFEMP3Shield MP3player;

int count; // The number being dialed
String lastTrackPlayed = ""; // The number of the track last played
String totalNumber = ""; // The running total of numbers dialed

boolean singleNumberDialed = false; // Single number is finished dialing
boolean totalNumberDialed = false; // Total number finished dialing

int rotaryInput = 12; // Input for the rotary device
int resetInput = 5; // Input for the phone reset

int rotaryLastState = LOW; // The previous state of the rotary
int rotaryLastTrueState = LOW; // The previous state of the rotary (stabilized)
int resetLastState = HIGH; // The previous state of the reset
int resetLastStateTrue = HIGH; // The previous state of the reset (stabilized)

long rotaryChangeTime = 0; // Time since last rotary change
long resetChangeTime = 0; // Time since last reset change

int dialHasFinishedRotatingAfterMs = 100; // ms to take dial to stop turning
int stoppedDialingMs = 3000; // ms to take before reading entire number
int debounceDelay = 10; // ms for dial reading to stabilize

int currentGame = 0; // The current game being played

// Tracks (stored in PROGMEM)
// http://arduino.cc/en/Reference/PROGMEM
prog_char TRACK_ADVENTURE[] PROGMEM = "1";
prog_char TRACK_CARELESS[] PROGMEM = "202";
prog_char TRACK_ELVISH[] PROGMEM = "203";
prog_char TRACK_WHACKEY[] PROGMEM = "204";
prog_char TRACK_JENNY[] PROGMEM = "205";
prog_char TRACK_GHOST[] PROGMEM = "206";
prog_char TRACK_BABY_BACK[] PROGMEM = "207";
prog_char TRACK_B52[] PROGMEM = "208";
prog_char TRACK_777[] PROGMEM = "209";
prog_char TRACK_PICKETT[] PROGMEM = "210";
prog_char TRACK_MIKE_JONES[] PROGMEM = "211";
prog_char TRACK_LUDACRIS[] PROGMEM = "212";
prog_char TRACK_BEECHWOOD[] PROGMEM = "213";
prog_char TRACK_BIGELOW[] PROGMEM = "214";
prog_char TRACK_411[] PROGMEM = "215";
prog_char TRACK_SUBLIME[] PROGMEM = "216";
prog_char TRACK_TOOTS[] PROGMEM = "217";
prog_char TRACK_OPERATOR[] PROGMEM = "218";
prog_char TRACK_PENN[] PROGMEM = "219";
prog_char TRACK_808[] PROGMEM = "220";
prog_char TRACK_DEELITE[] PROGMEM = "221";
prog_char TRACK_WHAT_IS_LOVE[] PROGMEM = "222";
prog_char TRACK_STAR_69[] PROGMEM = "223";
prog_char TRACK_GOAL[] PROGMEM = "224";
prog_char TRACK_VADER[] PROGMEM = "225";
prog_char TRACK_CHUCK_BERRY[] PROGMEM = "226";
prog_char TRACK_PYTHON[] PROGMEM = "227";
prog_char TRACK_433[] PROGMEM = "228";
PROGMEM const char *RANDOM_TRACKS[] = {TRACK_ADVENTURE, TRACK_CARELESS, TRACK_ELVISH, TRACK_WHACKEY, TRACK_JENNY, TRACK_GHOST, TRACK_B52, TRACK_777, TRACK_PICKETT, TRACK_MIKE_JONES, TRACK_LUDACRIS, TRACK_BEECHWOOD, TRACK_BIGELOW, TRACK_411, TRACK_SUBLIME, TRACK_TOOTS, TRACK_OPERATOR, TRACK_PENN, TRACK_808, TRACK_WHAT_IS_LOVE, TRACK_GOAL, TRACK_CHUCK_BERRY, TRACK_PYTHON};

char string_buffer[30];

#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0])))
int RANDOM_TRACK_LENGTH = NUMITEMS(RANDOM_TRACKS);

void setup() {
    pinMode(rotaryInput, INPUT);
    Serial.begin(9600);
    MP3player.begin();
    Serial.print("Starting...");
    Serial.println("done");
    MP3player.SetVolume(2, 2);
    randomSeed(analogRead(0)); // make sure the sequence is random
}

int getRandom() {
    return random(int(RANDOM_TRACK_LENGTH + RANDOM_TRACK_LENGTH * 0.3));
}

int stringToNumber(String thisString) {
    int i, value, length;
    length = thisString.length();
    char blah[(length+1)];
    for(i=0; i<length; i++) {
        blah[i] = thisString.charAt(i);
    }
    blah[i]=0;
    value = atoi(blah);
    return value;
}

void setTotalNumberWithMemoryFromBuffer() {
    String number = String(string_buffer);
    totalNumber = number;
    lastTrackPlayed = number;
}

void setTotalNumberWithMemory(String number) {
    totalNumber = number;
    lastTrackPlayed = number;
}

boolean wasLastPlayed(String number) {
    return lastTrackPlayed == number;
}

String checkSpecialNumber(String number) {
    String trackNumber = "0";
    if (number == "8675309") {
        trackNumber = "205";
    } else if (number == "911") {
        trackNumber = "206";
    } else if (number == "18006492568") {
        trackNumber = "207";
    } else if (number == "6060842") {
        trackNumber = "208";
    } else if (number == "7779311") {
        trackNumber = "209";
    } else if (number == "6345789") {
        trackNumber = "210";
    } else if (number == "2813308004") {
        trackNumber = "211";
    } else if (number == "411") {
        trackNumber = "215";
    } else if (number == "4390116") {
        trackNumber = "216";
    } else if (number == "808") {
        trackNumber = "220";
    } else if (number == "707") {
        trackNumber = "220";
    } else if (number == "505") {
        trackNumber = "220";
    } else if (number == "202") {
        trackNumber = "220";
    } else if (number == "666") {
        trackNumber = "225";
    } else if (number == "8314196668") {
        trackNumber = "229";
    } else if (number == "69") {
        int randPick = random(0, 3);
        if (randPick == 0) {
            trackNumber = "221";
        } else if (randPick == 1) {
            trackNumber = "222";
        } else if (randPick == 2) {
            trackNumber = "223";
        } else {
            trackNumber = "221";
        }
    } else {
    }
    return trackNumber;
}

void loadTrackIntoBuffer(int i) {
    strcpy_P(string_buffer, (char*)pgm_read_word(&(RANDOM_TRACKS[i])));
}

void playAdventure() {
    loadTrackIntoBuffer(0);
    setTotalNumberWithMemoryFromBuffer();
}

void loop() {
    int rotaryReading = digitalRead(rotaryInput);
    int resetReading = digitalRead(resetInput);

    if ((millis() - rotaryChangeTime) > stoppedDialingMs) {
        if (totalNumberDialed) {
            Serial.println("Dialing: " + totalNumber);
            MP3player.stopTrack();
            String specialNumber = checkSpecialNumber(totalNumber);
            if (specialNumber != "0") {
                setTotalNumberWithMemory(specialNumber);
            } else if (currentGame == 0) {
                // game not yet started
                // choose from initial menu
                int newRandom = getRandom();
                // check to see if random track should be played
                int i;
                if (totalNumber == "1980") {
                    currentGame = 1;
                    totalNumber = "2";
                } else if (totalNumber.length() < 3 && stringToNumber(totalNumber) < RANDOM_TRACK_LENGTH + 1) {
                    int trackNumber = stringToNumber(totalNumber) - 1;
                    if (trackNumber < 0) {
                        trackNumber = 0;
                    }
                    loadTrackIntoBuffer(trackNumber);
                    setTotalNumberWithMemoryFromBuffer();
                } else if (newRandom < RANDOM_TRACK_LENGTH) {
                    loadTrackIntoBuffer(newRandom);
                    if (wasLastPlayed(string_buffer)) {
                        playAdventure();
                    } else {
                        setTotalNumberWithMemoryFromBuffer();
                    }
                } else {
                    playAdventure();
                }
            } else if (currentGame == 1) {
                if (totalNumber == "0") {
                    totalNumber = "10";
                } else if (totalNumber == "1980") {
                    totalNumber = "2";
                }
            }
            MP3player.playTrack(stringToNumber(totalNumber));
            totalNumber = "";
            totalNumberDialed = false;
        }
    }

    if ((millis() - rotaryChangeTime) > dialHasFinishedRotatingAfterMs) {
        if (singleNumberDialed) {
            Serial.println("Dialed: " + count % 10);
            totalNumber = totalNumber + String(count % 10);
            Serial.println("Total number is: " + totalNumber);
            singleNumberDialed = false;
            totalNumberDialed = true;
            count = 0;
        }
    }

    if (rotaryReading != rotaryLastState) {
        rotaryChangeTime = millis();
    }
    if ((millis() - rotaryChangeTime) > debounceDelay) {
        if (rotaryReading != rotaryLastTrueState) {
            rotaryLastTrueState = rotaryReading;
            if (rotaryLastTrueState == HIGH) {
                count++;
                singleNumberDialed = true;
            }
        }
    }
    rotaryLastState = rotaryReading;

    // reset the phone by hanging up
    if (resetReading != resetLastState) {
        resetChangeTime = millis();
    }
    if ((millis() - resetChangeTime) > debounceDelay) {
        if (resetReading != resetLastStateTrue) {
            resetLastStateTrue = resetReading;
            if (resetLastStateTrue == LOW) {
                Serial.println("Reset!");
                MP3player.stopTrack();
                currentGame = 0;
                MP3player.playTrack(200);
            } else {
                MP3player.stopTrack();
            }
        }
    }
    resetLastState = resetReading;
}



