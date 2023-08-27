/*
 * DEFCON Online
 * A Telephone System for Replicant Resolve 2022
 *
 * Benjamin Gleitzman (gleitz@replicant.com)
 */

// TODO(gleitz): leet, defcon, replicant

#define __PROG_TYPES_COMPAT__

#include <SPI.h>
#include <FreeStack.h>

#include <avr/pgmspace.h>

//Add the SdFat Libraries
#include <SdFat.h>

//and the MP3 Shield Library
#include <vs1053_SdFat.h>

SdFat sd;
vs1053 MP3player;

byte result;

int count; // The number being dialed
String lastTrackPlayed = ""; // The number of the track last played
String totalNumber = ""; // The running total of numbers dialed

boolean singleNumberDialed = false; // Single number is finished dialing
boolean totalNumberDialed = false; // Total number finished dialing

int rotaryInput = 12; // Input for the rotary device (used to be 12 in previous version)
int resetInput = 5; // Input for the phone reset

int rotaryLastState = LOW; // The previous state of the rotary
int rotaryLastTrueState = LOW; // The previous state of the rotary (stabilized)
int resetLastState = HIGH; // The previous state of the reset
int resetLastStateTrue = HIGH; // The previous state of the reset (stabilized)

long rotaryChangeTime = 0; // Time since last rotary change
long resetChangeTime = 0; // Time since last reset change

int dialHasFinishedRotatingAfterMs = 100; // ms to take dial to stop turning
int stoppedDialingMs = 2000; // ms to take before reading entire number
int debounceDelay = 10; // ms for dial reading to stabilize

int currentGame = 0; // The current game being played

// Tracks (stored in PROGMEM)
// http://arduino.cc/en/Reference/PROGMEM

// Adventure tracks 1-121
const char TRACK_ADVENTURE[] PROGMEM = "1";

const char TRACK_DEFCON_INTRO[] PROGMEM = "122";

const char TRACK_5_7_0_5[] PROGMEM = "123";
const char TRACK_6060842[] PROGMEM = "124";
const char TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER[] PROGMEM = "125";
const char TRACK_853_5937[] PROGMEM = "126";
const char TRACK_BACKSTREET_BOYS_THE_CALL_OFFICIAL_HD_VIDEO[] PROGMEM = "127";
const char TRACK_BECK_CELLPHONES_DEAD[] PROGMEM = "128";
const char TRACK_BLADE_RUNNER_O_MAIN_THEME_O_VANGELIS[] PROGMEM = "129";
const char TRACK_BLONDIE_HANGING_ON_THE_TELEPHONE[] PROGMEM = "130";
const char TRACK_BLONDIE_CALL_ME[] PROGMEM = "131";
const char TRACK_CAKE_NEVER_THERE[] PROGMEM = "132";
const char TRACK_CALL_ME_BACK_AGAIN[] PROGMEM = "133";
const char TRACK_CALL_ME[] PROGMEM = "134";
const char TRACK_CALL_ME_MR_TELEPHONE_CHEYNE_1985_1_USA_DANCE_CHARTS[] PROGMEM = "135";
const char TRACK_CARELESS_WHISPER[] PROGMEM = "136";
const char TRACK_CARLY_RAE_JEPSEN_CALL_ME_MAYBE[] PROGMEM = "137";
const char TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO[] PROGMEM = "138";
const char TRACK_DRAKE_HOTLINE_BLING_LYRICS[] PROGMEM = "139";
const char TRACK_ERIC_PRYDZ_CALL_ON_ME[] PROGMEM = "140";
const char TRACK_GLENN_MILLER_PENNSYLVANIA_6_5000_1940_DIGITALLY_REMASTERED[] PROGMEM = "141";
const char TRACK_HAD_TO_PHONE_YA_REMASTERED_2000[] PROGMEM = "142";
const char TRACK_HOW_COME_U_DONT_CALL_ME_ANYMORE[] PROGMEM = "143";
const char TRACK_I_JUST_CALLED_TO_SAY_I_LOVE_YOU[] PROGMEM = "144";
const char TRACK_JUNIOR_VASQUEZ_IF_MADONNA_CALLS[] PROGMEM = "145";
const char TRACK_JUST_BE_A_MAN_ABOUT_IT_RADIO_EDIT[] PROGMEM = "146";
const char TRACK_KRAFTWERK_THE_TELEPHONE_CALL[] PROGMEM = "147";
const char TRACK_LOVE_ON_THE_TELEPHONE[] PROGMEM = "148";
const char TRACK_MARY_WELLS_OPERATOR[] PROGMEM = "149";
const char TRACK_MABEL_DONT_CALL_ME_UP_OFFICIAL_VIDEO[] PROGMEM = "150";
const char TRACK_NEW_EDITION_MR_TELEPHONE_MAN_OFFICIAL_MUSIC_VIDEO[] PROGMEM = "151";
const char TRACK_OPERATOR_JIM_CROCE[] PROGMEM = "152";
const char TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG[] PROGMEM = "153";
const char TRACK_PRIMITIVE_RADIO_GODS_STANDING_OUTSIDE_A_BROKEN_PHONE_BOOTH_WITH_MONEY_IN_MY_HAND_1996[] PROGMEM = "154";
const char TRACK_RAFFI_BANANAPHONE[] PROGMEM = "155";
const char TRACK_RING_RING_ENGLISH_VERSION[] PROGMEM = "156";
const char TRACK_SMOOTH_OPERATOR_REMASTERED[] PROGMEM = "157";
const char TRACK_SPIDERWEBS[] PROGMEM = "158";
const char TRACK_STEELY_DAN_RIKKI_DONT_LOSE_THAT_NUMBER[] PROGMEM = "159";
const char TRACK_THE_ORLONS_DONT_HANG_UP[] PROGMEM = "160";
const char TRACK_TELEPHONE[] PROGMEM = "161";
const char TRACK_THE_BIG_BOPPER_CHANTILLY_LACE_HQ[] PROGMEM = "162";
const char TRACK_THE_CALL_BACKSTREET_BOYS_FEAT_THE_NEPTUNES_REMIX[] PROGMEM = "163";
const char TRACK_THE_KINKS_PARTY_LINE_HQ[] PROGMEM = "164";
const char TRACK_THE_MARVELETTES_BEECHWOOD_4_5789[] PROGMEM = "165";
const char TRACK_THE_TIME_777_9311_SINGLE_VERSION[] PROGMEM = "166";
const char TRACK_THEME_SONG_KIM_POSSIBLE_DISNEY_CHANNEL[] PROGMEM = "167";
const char TRACK_TODD_RUNDGREN_HELLO_ITS_ME_1972[] PROGMEM = "168";
const char TRACK_TOMMY_TUTONE_867_5309JENNY[] PROGMEM = "169";
const char TRACK_TONY_BOOTH_LONESOME_7_7203[] PROGMEM = "170";
const char TRACK_GAFFYOURSELF[] PROGMEM = "171";
const char TRACK_SKYY_CALL_ME_SINGLE_VERSION[] PROGMEM = "172";
const char* const RANDOM_TRACKS[] PROGMEM = { TRACK_5_7_0_5, TRACK_6060842, TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER, TRACK_853_5937, TRACK_BACKSTREET_BOYS_THE_CALL_OFFICIAL_HD_VIDEO, TRACK_BECK_CELLPHONES_DEAD, TRACK_BLADE_RUNNER_O_MAIN_THEME_O_VANGELIS, TRACK_BLONDIE_HANGING_ON_THE_TELEPHONE, TRACK_BLONDIE_CALL_ME, TRACK_CAKE_NEVER_THERE, TRACK_CALL_ME_BACK_AGAIN, TRACK_CALL_ME, TRACK_CALL_ME_MR_TELEPHONE_CHEYNE_1985_1_USA_DANCE_CHARTS, TRACK_CARELESS_WHISPER, TRACK_CARLY_RAE_JEPSEN_CALL_ME_MAYBE, TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO, TRACK_DRAKE_HOTLINE_BLING_LYRICS, TRACK_ERIC_PRYDZ_CALL_ON_ME, TRACK_GLENN_MILLER_PENNSYLVANIA_6_5000_1940_DIGITALLY_REMASTERED, TRACK_HAD_TO_PHONE_YA_REMASTERED_2000, TRACK_HOW_COME_U_DONT_CALL_ME_ANYMORE, TRACK_I_JUST_CALLED_TO_SAY_I_LOVE_YOU, TRACK_JUNIOR_VASQUEZ_IF_MADONNA_CALLS, TRACK_JUST_BE_A_MAN_ABOUT_IT_RADIO_EDIT, TRACK_KRAFTWERK_THE_TELEPHONE_CALL, TRACK_LOVE_ON_THE_TELEPHONE, TRACK_MARY_WELLS_OPERATOR, TRACK_MABEL_DONT_CALL_ME_UP_OFFICIAL_VIDEO, TRACK_NEW_EDITION_MR_TELEPHONE_MAN_OFFICIAL_MUSIC_VIDEO, TRACK_OPERATOR_JIM_CROCE, TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG, TRACK_PRIMITIVE_RADIO_GODS_STANDING_OUTSIDE_A_BROKEN_PHONE_BOOTH_WITH_MONEY_IN_MY_HAND_1996, TRACK_RAFFI_BANANAPHONE, TRACK_RING_RING_ENGLISH_VERSION, TRACK_SMOOTH_OPERATOR_REMASTERED, TRACK_SPIDERWEBS, TRACK_STEELY_DAN_RIKKI_DONT_LOSE_THAT_NUMBER, TRACK_THE_ORLONS_DONT_HANG_UP, TRACK_TELEPHONE, TRACK_THE_BIG_BOPPER_CHANTILLY_LACE_HQ, TRACK_THE_CALL_BACKSTREET_BOYS_FEAT_THE_NEPTUNES_REMIX, TRACK_THE_KINKS_PARTY_LINE_HQ, TRACK_THE_MARVELETTES_BEECHWOOD_4_5789, TRACK_THE_TIME_777_9311_SINGLE_VERSION, TRACK_THEME_SONG_KIM_POSSIBLE_DISNEY_CHANNEL, TRACK_TODD_RUNDGREN_HELLO_ITS_ME_1972, TRACK_TOMMY_TUTONE_867_5309JENNY, TRACK_TONY_BOOTH_LONESOME_7_7203, TRACK_GAFFYOURSELF, TRACK_SKYY_CALL_ME_SINGLE_VERSION };

char string_buffer[30];

#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0])))
int RANDOM_TRACK_LENGTH = NUMITEMS(RANDOM_TRACKS);

void setup() {
    pinMode(rotaryInput, INPUT);
    pinMode(resetInput, INPUT);
    Serial.begin(115200);

    Serial.print("Starting...");

    //Initialize the SdCard.
    if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
    // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
    if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

    //Initialize the MP3 Player Shield
    result = MP3player.begin();
    //check result, see readme for e rror codes.
    if(result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to start MP3 player"));
      if( result == 6 ) {
        Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
        Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
      }
    }

    Serial.println("done");
    MP3player.setVolume(0xA, 0xA); // 0x14, 0x14
    // TODO: do i need this?
    MP3player.playTrack(stringToNumber(getProgmem(TRACK_DEFCON_INTRO)));
    randomSeed(analogRead(0)); // make sure the sequence is random
}

void(* resetFunc) (void) = 0;

int getRandom() {
    return random(int(RANDOM_TRACK_LENGTH + RANDOM_TRACK_LENGTH * 0.2));
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

String getProgmem(const char* value) {
    return (const __FlashStringHelper *)value;
}

String checkSpecialNumber(String number) {
    String trackNumber = "0";
    if (number == "8675309") {
        trackNumber = getProgmem(TRACK_TOMMY_TUTONE_867_5309JENNY);
    } else if (number == "911" || number == "666") {
        trackNumber = getProgmem(TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG);
    } else if (number == "6060842") {
        trackNumber = getProgmem(TRACK_6060842);
    } else if (number == "7779311") {
        trackNumber = getProgmem(TRACK_THE_TIME_777_9311_SINGLE_VERSION);
    } else if (number == "6345789") {
        trackNumber = getProgmem(TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER);
    } else if (number == "2222222") {
        trackNumber = getProgmem(TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO);
    } else if (number == "411") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "5108160458‬") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "0") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "69") {
        trackNumber = getProgmem(TRACK_CARELESS_WHISPER);
    } else if (number == "420") {
        trackNumber = getProgmem(TRACK_CARELESS_WHISPER);
    } else if (number == "737542268") {
        trackNumber = getProgmem(TRACK_BLADE_RUNNER_O_MAIN_THEME_O_VANGELIS);
    } else {
    }
    return trackNumber;
}

void loadTrackIntoBuffer(int i) {
    strcpy_P(string_buffer, (char*)pgm_read_word(&(RANDOM_TRACKS[i])));
}


void playAdventure() {
    loadTrackIntoBuffer(stringToNumber(getProgmem(TRACK_ADVENTURE)));
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
                Serial.println("Special number");
                setTotalNumberWithMemory(specialNumber);
            } else if (currentGame == 0) {
                // game not yet started
                // choose from initial menu
                int newRandom = getRandom();
                // check to see if random track should be played
                int i;
                if (totalNumber == "1980") {
                    Serial.println("1980 - start game");
                    currentGame = 1;
                    totalNumber = "2";
                } else if (totalNumber == "1995" || totalNumber == "4775619" || totalNumber == "2134775619" || totalNumber == "7370583" || totalNumber == "7376583" || totalNumber == "776659" || totalNumber == "770059" || totalNumber == "200" || totalNumber == "266") { // SPOOKY, RES0LVE, B00, RESOLVE, BOO
                    Serial.println("1995 - start game at top");
                    currentGame = 1;
                    totalNumber = "1";
                } else if (totalNumber.length() < 3 && stringToNumber(totalNumber) < RANDOM_TRACK_LENGTH + 1) {
                    int trackNumber = stringToNumber(totalNumber) - 1;
                    if (trackNumber < 0) {
                        trackNumber = 0;
                    }
                    Serial.println("Dialed number less than number of random tracks");
                    loadTrackIntoBuffer(trackNumber);
                    setTotalNumberWithMemoryFromBuffer();
                } else if (newRandom < RANDOM_TRACK_LENGTH) {
                    Serial.println("Playing random track " + newRandom);
                    loadTrackIntoBuffer(newRandom);
                    if (wasLastPlayed(string_buffer)) {
                        Serial.println("Was already played, starting adventure");
                        playAdventure();
                        currentGame = 1;
                    } else {
                        setTotalNumberWithMemoryFromBuffer();
                    }
                } else {
                    Serial.println("Fall through, start adventure");
                    playAdventure();
                }
            } else if (currentGame == 1) {
                if (totalNumber == "0") {
                    totalNumber = "10";
                } else if (totalNumber == "1980") {
                    totalNumber = "2";
                }
            }
            Serial.println("Playing " + totalNumber);
            Serial.println("Playing " + stringToNumber(totalNumber));
            MP3player.playTrack(stringToNumber(totalNumber));
            totalNumber = "";
            totalNumberDialed = false;
        }
    }

    if ((millis() - rotaryChangeTime) > dialHasFinishedRotatingAfterMs) {
        if (singleNumberDialed) {
            Serial.println("Dialed: " + count % 10);
            MP3player.stopTrack();
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
                MP3player.playTrack(stringToNumber(getProgmem(TRACK_DEFCON_INTRO)));
            } else {
                MP3player.stopTrack();
            }
        }
    }
    resetLastState = resetReading;
}
