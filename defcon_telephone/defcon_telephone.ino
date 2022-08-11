/*
 * DEFCON Online
 * A Telephone System for DEFCON 30
 *
 * Benjamin Gleitzman (gleitz@mit.edu)
 */

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

int rotaryInput = 12; // Input for the rotary device
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

//const char TRACK_CARELESS[] PROGMEM = "202";
//const char TRACK_ELVISH[] PROGMEM = "203";
//const char TRACK_WHACKEY[] PROGMEM = "204";
//const char TRACK_JENNY[] PROGMEM = "205";
//const char TRACK_GHOST[] PROGMEM = "206";
//const char TRACK_BABY_BACK[] PROGMEM = "207";
//const char TRACK_B52[] PROGMEM = "208";
//const char TRACK_777[] PROGMEM = "209";
//const char TRACK_PICKETT[] PROGMEM = "210";
//const char TRACK_MIKE_JONES[] PROGMEM = "211";
//const char TRACK_LUDACRIS[] PROGMEM = "212";
//const char TRACK_BEECHWOOD[] PROGMEM = "213";
//const char TRACK_BIGELOW[] PROGMEM = "214";
//const char TRACK_411[] PROGMEM = "215";
//const char TRACK_SUBLIME[] PROGMEM = "216";
//const char TRACK_TOOTS[] PROGMEM = "217";
//const char TRACK_OPERATOR[] PROGMEM = "218";
//const char TRACK_PENN[] PROGMEM = "219";
//const char TRACK_808[] PROGMEM = "220";
//const char TRACK_DEELITE[] PROGMEM = "221";
//const char TRACK_WHAT_IS_LOVE[] PROGMEM = "222";
//const char TRACK_STAR_69[] PROGMEM = "223";
//const char TRACK_GOAL[] PROGMEM = "224";
//const char TRACK_VADER[] PROGMEM = "225";
//const char TRACK_CHUCK_BERRY[] PROGMEM = "226";
//const char TRACK_PYTHON[] PROGMEM = "227";
//const char TRACK_433[] PROGMEM = "228";
//const char TRACK_BOYZ[] PROGMEM = "229";
//const char TRACK_DUTCHIE[] PROGMEM = "230";
//
//const char* const RANDOM_TRACKS[] PROGMEM = {TRACK_ADVENTURE, TRACK_CARELESS, TRACK_ELVISH, TRACK_WHACKEY, TRACK_JENNY, TRACK_GHOST, TRACK_B52, TRACK_777, TRACK_PICKETT, TRACK_MIKE_JONES, TRACK_LUDACRIS, TRACK_BEECHWOOD, TRACK_BIGELOW, TRACK_411, TRACK_SUBLIME, TRACK_TOOTS, TRACK_OPERATOR, TRACK_PENN, TRACK_808, TRACK_WHAT_IS_LOVE, TRACK_GOAL, TRACK_CHUCK_BERRY, TRACK_PYTHON};

const char TRACK_2_LIVE_CREW_ME_SO_HORNY[] PROGMEM = "123";
const char TRACK_5_7_0_5[] PROGMEM = "124";
const char TRACK_6060842[] PROGMEM = "125";
const char TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER[] PROGMEM = "126";
const char TRACK_853_5937[] PROGMEM = "127";
const char TRACK_AREA_CODES_FEAT_NATE_DOGG_BY_LUDACRIS_CRATE_CLASSICS_RAP[] PROGMEM = "128";
const char TRACK_BABY_GOT_BACK[] PROGMEM = "129";
const char TRACK_BACKSTREET_BOYS_THE_CALL_OFFICIAL_HD_VIDEO[] PROGMEM = "130";
const char TRACK_BECK_CELLPHONES_DEAD[] PROGMEM = "131";
const char TRACK_BLONDIE_HANGING_ON_THE_TELEPHONE[] PROGMEM = "132";
const char TRACK_BLONDIE_CALL_ME[] PROGMEM = "133";
const char TRACK_CAKE_NEVER_THERE[] PROGMEM = "134";
const char TRACK_CALL_ME_BACK_AGAIN[] PROGMEM = "135";
const char TRACK_CALL_ME[] PROGMEM = "136";
const char TRACK_CALL_ME_MR_TELEPHONE_CHEYNE_1985_1_USA_DANCE_CHARTS[] PROGMEM = "137";
const char TRACK_CARELESS_WHISPER[] PROGMEM = "138";
const char TRACK_CARLY_RAE_JEPSEN_CALL_ME_MAYBE[] PROGMEM = "139";
const char TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO[] PROGMEM = "140";
const char TRACK_DRAKE_HOTLINE_BLING_LYRICS[] PROGMEM = "141";
const char TRACK_ERIC_PRYDZ_CALL_ON_ME[] PROGMEM = "142";
const char TRACK_GLENN_MILLER_PENNSYLVANIA_6_5000_1940_DIGITALLY_REMASTERED[] PROGMEM = "143";
const char TRACK_HAD_TO_PHONE_YA_REMASTERED_2000[] PROGMEM = "144";
const char TRACK_HITS_FROM_THE_BONG[] PROGMEM = "145";
const char TRACK_HOW_COME_U_DONT_CALL_ME_ANYMORE[] PROGMEM = "146";
const char TRACK_I_JUST_CALLED_TO_SAY_I_LOVE_YOU[] PROGMEM = "147";
const char TRACK_JUNIOR_VASQUEZ_IF_MADONNA_CALLS[] PROGMEM = "148";
const char TRACK_JUST_BE_A_MAN_ABOUT_IT_RADIO_EDIT[] PROGMEM = "149";
const char TRACK_KEHA_STEPHEN[] PROGMEM = "150";
const char TRACK_KRAFTWERK_THE_TELEPHONE_CALL[] PROGMEM = "151";
const char TRACK_LITTLE_MIX_HOW_YA_DOIN_FT_MISSY_ELLIOTT[] PROGMEM = "152";
const char TRACK_LONG_DISTANCE_RUNAROUND_2008_REMASTER[] PROGMEM = "153";
const char TRACK_LOVE_ON_THE_TELEPHONE[] PROGMEM = "154";
const char TRACK_MARY_WELLS_OPERATOR[] PROGMEM = "155";
const char TRACK_MABEL_DONT_CALL_ME_UP_OFFICIAL_VIDEO[] PROGMEM = "156";
const char TRACK_MIKE_JONES_BACK_THEN[] PROGMEM = "157";
const char TRACK_NEW_EDITION_MR_TELEPHONE_MAN_OFFICIAL_MUSIC_VIDEO[] PROGMEM = "158";
const char TRACK_OPERATOR_JIM_CROCE[] PROGMEM = "159";
const char TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG[] PROGMEM = "160";
const char TRACK_PICK_UP_THE_PHONE[] PROGMEM = "161";
const char TRACK_POPPY_MOSHI_MOSHI_OFFICIAL_VIDEO[] PROGMEM = "162";
const char TRACK_PRIMITIVE_RADIO_GODS_STANDING_OUTSIDE_A_BROKEN_PHONE_BOOTH_WITH_MONEY_IN_MY_HAND_1996[] PROGMEM = "163";
const char TRACK_R_E_M_STAR_69_MONSTER_REMASTERED[] PROGMEM = "164";
const char TRACK_RAFFI_BANANAPHONE[] PROGMEM = "165";
const char TRACK_RESISTOR_CORDLESS_PHONE[] PROGMEM = "166";
const char TRACK_RING_RING_ENGLISH_VERSION[] PROGMEM = "167";
const char TRACK_SMOOTH_OPERATOR_REMASTERED[] PROGMEM = "168";
const char TRACK_SOULJA_BOY_TELLEM_FT_SAMMIE_KISS_ME_THRU_THE_PHONE_OFFICIAL_VIDEO[] PROGMEM = "169";
const char TRACK_SPIDERWEBS[] PROGMEM = "170";
const char TRACK_STEELY_DAN_RIKKI_DONT_LOSE_THAT_NUMBER[] PROGMEM = "171";
const char TRACK_SUBLIME_DONT_PUSH[] PROGMEM = "172";
const char TRACK_THE_ORLONS_DONT_HANG_UP[] PROGMEM = "173";
const char TRACK_TELEPHONE[] PROGMEM = "174";
const char TRACK_THE_BIG_BOPPER_CHANTILLY_LACE_HQ[] PROGMEM = "175";
const char TRACK_THE_CALL_BACKSTREET_BOYS_FEAT_THE_NEPTUNES_REMIX[] PROGMEM = "176";
const char TRACK_THE_KINKS_PARTY_LINE_HQ[] PROGMEM = "177";
const char TRACK_THE_MARVELETTES_BEECHWOOD_4_5789[] PROGMEM = "178";
const char TRACK_THE_TIME_777_9311_SINGLE_VERSION[] PROGMEM = "179";
const char TRACK_THEME_SONG[] PROGMEM = "180";
const char TRACK_TODD_RUNDGREN_HELLO_ITS_ME_1972[] PROGMEM = "181";
const char TRACK_TOMMY_TUTONE_867_5309JENNY[] PROGMEM = "182";
const char TRACK_TONY_BOOTH_LONESOME_7_7203[] PROGMEM = "183";
const char TRACK_VIDEO_PHONE[] PROGMEM = "184";
const char TRACK_VILLAGE_PEOPLE_SEX_OVER_THE_PHONE_OFFICIAL_MUSIC_VIDEO_1985[] PROGMEM = "185";
const char TRACK_YOUNG_LUST[] PROGMEM = "186";
const char TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT[] PROGMEM = "187";
const char TRACK_SKYY_CALL_ME_SINGLE_VERSION[] PROGMEM = "188";
const char* const RANDOM_TRACKS[] PROGMEM = { TRACK_2_LIVE_CREW_ME_SO_HORNY, TRACK_5_7_0_5, TRACK_6060842, TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER, TRACK_853_5937, TRACK_AREA_CODES_FEAT_NATE_DOGG_BY_LUDACRIS_CRATE_CLASSICS_RAP, TRACK_BABY_GOT_BACK, TRACK_BACKSTREET_BOYS_THE_CALL_OFFICIAL_HD_VIDEO, TRACK_BECK_CELLPHONES_DEAD, TRACK_BLONDIE_HANGING_ON_THE_TELEPHONE, TRACK_BLONDIE_CALL_ME, TRACK_CAKE_NEVER_THERE, TRACK_CALL_ME_BACK_AGAIN, TRACK_CALL_ME, TRACK_CALL_ME_MR_TELEPHONE_CHEYNE_1985_1_USA_DANCE_CHARTS, TRACK_CARELESS_WHISPER, TRACK_CARLY_RAE_JEPSEN_CALL_ME_MAYBE, TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO, TRACK_DRAKE_HOTLINE_BLING_LYRICS, TRACK_ERIC_PRYDZ_CALL_ON_ME, TRACK_GLENN_MILLER_PENNSYLVANIA_6_5000_1940_DIGITALLY_REMASTERED, TRACK_HAD_TO_PHONE_YA_REMASTERED_2000, TRACK_HITS_FROM_THE_BONG, TRACK_HOW_COME_U_DONT_CALL_ME_ANYMORE, TRACK_I_JUST_CALLED_TO_SAY_I_LOVE_YOU, TRACK_JUNIOR_VASQUEZ_IF_MADONNA_CALLS, TRACK_JUST_BE_A_MAN_ABOUT_IT_RADIO_EDIT, TRACK_KEHA_STEPHEN, TRACK_KRAFTWERK_THE_TELEPHONE_CALL, TRACK_LITTLE_MIX_HOW_YA_DOIN_FT_MISSY_ELLIOTT, TRACK_LONG_DISTANCE_RUNAROUND_2008_REMASTER, TRACK_LOVE_ON_THE_TELEPHONE, TRACK_MARY_WELLS_OPERATOR, TRACK_MABEL_DONT_CALL_ME_UP_OFFICIAL_VIDEO, TRACK_MIKE_JONES_BACK_THEN, TRACK_NEW_EDITION_MR_TELEPHONE_MAN_OFFICIAL_MUSIC_VIDEO, TRACK_OPERATOR_JIM_CROCE, TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG, TRACK_PICK_UP_THE_PHONE, TRACK_POPPY_MOSHI_MOSHI_OFFICIAL_VIDEO, TRACK_PRIMITIVE_RADIO_GODS_STANDING_OUTSIDE_A_BROKEN_PHONE_BOOTH_WITH_MONEY_IN_MY_HAND_1996, TRACK_R_E_M_STAR_69_MONSTER_REMASTERED, TRACK_RAFFI_BANANAPHONE, TRACK_RESISTOR_CORDLESS_PHONE, TRACK_RING_RING_ENGLISH_VERSION, TRACK_SMOOTH_OPERATOR_REMASTERED, TRACK_SOULJA_BOY_TELLEM_FT_SAMMIE_KISS_ME_THRU_THE_PHONE_OFFICIAL_VIDEO, TRACK_SPIDERWEBS, TRACK_STEELY_DAN_RIKKI_DONT_LOSE_THAT_NUMBER, TRACK_SUBLIME_DONT_PUSH, TRACK_THE_ORLONS_DONT_HANG_UP, TRACK_TELEPHONE, TRACK_THE_BIG_BOPPER_CHANTILLY_LACE_HQ, TRACK_THE_CALL_BACKSTREET_BOYS_FEAT_THE_NEPTUNES_REMIX, TRACK_THE_KINKS_PARTY_LINE_HQ, TRACK_THE_MARVELETTES_BEECHWOOD_4_5789, TRACK_THE_TIME_777_9311_SINGLE_VERSION, TRACK_THEME_SONG, TRACK_TODD_RUNDGREN_HELLO_ITS_ME_1972, TRACK_TOMMY_TUTONE_867_5309JENNY, TRACK_TONY_BOOTH_LONESOME_7_7203, TRACK_VIDEO_PHONE, TRACK_VILLAGE_PEOPLE_SEX_OVER_THE_PHONE_OFFICIAL_MUSIC_VIDEO_1985, TRACK_YOUNG_LUST, TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT, TRACK_SKYY_CALL_ME_SINGLE_VERSION };


char string_buffer[30];

#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0])))
int RANDOM_TRACK_LENGTH = NUMITEMS(RANDOM_TRACKS);

void setup() {
    pinMode(rotaryInput, INPUT);
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
    MP3player.setVolume(2, 2);
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
        trackNumber = getProgmem(TRACK_TONY_BOOTH_LONESOME_7_7203);
    } else if (number == "911" || number == "666") {
        trackNumber = getProgmem(TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG);
    } else if (number == "18006492568" || number == "19006492568") {
        trackNumber = getProgmem(TRACK_BABY_GOT_BACK);
    } else if (number == "6060842") {
        trackNumber = getProgmem(TRACK_6060842);
    } else if (number == "7779311") {
        trackNumber = getProgmem(TRACK_THE_TIME_777_9311_SINGLE_VERSION);
    } else if (number == "6345789") {
        trackNumber = getProgmem(TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER);
    } else if (number == "2813308004") {
        trackNumber = getProgmem(TRACK_MIKE_JONES_BACK_THEN);
    } else if (number == "411") {
        trackNumber = getProgmem(TRACK_CARELESS_WHISPER);
    } else if (number == "4390116") {
        trackNumber = getProgmem(TRACK_SUBLIME_DONT_PUSH);
    } else if (number == "420") {
        trackNumber = getProgmem(TRACK_HITS_FROM_THE_BONG);
    } else if (number == "808") {
        trackNumber = getProgmem(TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT);
    } else if (number == "707") {
        trackNumber = getProgmem(TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT);
    } else if (number == "505") {
        trackNumber = getProgmem(TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT);
    } else if (number == "202") {
        trackNumber = getProgmem(TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT);
    } else if (number == "5108160458‬") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "0") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "69") {
        int randPick = random(0, 5);
        if (randPick == 0) {
            trackNumber = getProgmem(TRACK_2_LIVE_CREW_ME_SO_HORNY);
        } else if (randPick == 1) {
            trackNumber = getProgmem(TRACK_AREA_CODES_FEAT_NATE_DOGG_BY_LUDACRIS_CRATE_CLASSICS_RAP);
        } else if (randPick == 2) {
            trackNumber = getProgmem(TRACK_CARELESS_WHISPER);
        } else if (randPick == 3) {
            trackNumber = getProgmem(TRACK_VILLAGE_PEOPLE_SEX_OVER_THE_PHONE_OFFICIAL_MUSIC_VIDEO_1985);
        } else if (randPick == 4) {
            trackNumber = getProgmem(TRACK_BABY_GOT_BACK);
        } else {
            trackNumber = getProgmem(TRACK_ERIC_PRYDZ_CALL_ON_ME);
        }
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
                } else if (totalNumber == "1995" || totalNumber == "4775619" || totalNumber == "2134775619") {
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
