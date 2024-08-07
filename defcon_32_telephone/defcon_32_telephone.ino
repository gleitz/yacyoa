/*
 * DEFCON Online
 * A Telephone System for DEFCON 30
 *
 * Benjamin Gleitzman (gleitz@mit.edu)
 *
 * Board is Arduino Duemilanove
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

// int tmpCounter = 0; // Used for knowing when to print periodic values

int count; // The number being dialed
int resetCount; // The reset number being dialed
String lastTrackPlayed = ""; // The number of the track last played
String totalNumber = ""; // The running total of numbers dialed

boolean singleNumberDialed = false; // Single number is finished dialing
boolean singleNumberResetDialed = false; // Single number is finished dialing
boolean totalNumberResetDialed = false; // Total number finished dialing
boolean totalNumberDialed = false; // Total number finished dialing

boolean activated = false; // if the phone is working

int rotaryInput = 12; // Input for the rotary device
int resetInput = 5; // Input for the phone reset

int rotaryLastState = LOW; // The previous state of the rotary
int rotaryLastTrueState = LOW; // The previous state of the rotary (stabilized)
int resetLastState = HIGH; // The previous state of the reset
int resetLastTrueState = HIGH; // The previous state of the reset (stabilized)

long rotaryChangeTime = 0; // Time since last rotary change
long resetChangeTime = 0; // Time since last reset change

int dialHasFinishedRotatingAfterMs = 100; // ms to take dial to stop turning
int resetAfterMs = 700; // ms to wait before we count a reset
int stoppedDialingMs = 2000; // ms to take before reading entire number
int debounceDelay = 10; // ms for dial reading to stabilize
int resetDebounceDelay = 10; // ms for dial reading to stabilize

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

// START OF GENERATED SECTION
const char TRACK_10_10_220_CHRISTOPHER_LLOYD_TAXI_COMMERCIAL_1999[] PROGMEM = "123";
const char TRACK_2_LIVE_CREW_ME_SO_HORNY[] PROGMEM = "124";
const char TRACK_5_7_0_5[] PROGMEM = "125";
const char TRACK_6060842[] PROGMEM = "126";
const char TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER[] PROGMEM = "127";
const char TRACK_853_5937[] PROGMEM = "128";
const char TRACK_AREA_CODES_FEAT_NATE_DOGG_BY_LUDACRIS_CRATE_CLASSICS_RAP[] PROGMEM = "129";
const char TRACK_BABY_GOT_BACK[] PROGMEM = "130";
const char TRACK_BACKSTREET_BOYS_THE_CALL_OFFICIAL_HD_VIDEO[] PROGMEM = "131";
const char TRACK_BALLAD_OF_THE_20TH_MAINE[] PROGMEM = "132";
const char TRACK_BECK_CELLPHONES_DEAD[] PROGMEM = "133";
const char TRACK_BLONDIE_HANGING_ON_THE_TELEPHONE[] PROGMEM = "134";
const char TRACK_BLONDIE_CALL_ME[] PROGMEM = "135";
const char TRACK_CAKE_NEVER_THERE[] PROGMEM = "136";
const char TRACK_CALL_ME_BACK_AGAIN[] PROGMEM = "137";
const char TRACK_CALL_ME[] PROGMEM = "138";
const char TRACK_CALL_ME_MR_TELEPHONE_CHEYNE_1985_1_USA_DANCE_CHARTS[] PROGMEM = "139";
const char TRACK_CARELESS_WHISPER[] PROGMEM = "140";
const char TRACK_CARLY_RAE_JEPSEN_CALL_ME_MAYBE[] PROGMEM = "141";
const char TRACK_CISCO_DEFAULT_HOLD_MUSIC_HQ_MONO_AUDIO_OPUS_NUMBER_1[] PROGMEM = "142";
const char TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO[] PROGMEM = "143";
const char TRACK_DRAKE_HOTLINE_BLING_LYRICS[] PROGMEM = "144";
const char TRACK_ERIC_PRYDZ_CALL_ON_ME[] PROGMEM = "145";
const char TRACK_GLENN_MILLER_PENNSYLVANIA_6_5000_1940_DIGITALLY_REMASTERED[] PROGMEM = "146";
const char TRACK_GO_DOWN_TOGETHER[] PROGMEM = "147";
const char TRACK_GOLDENEYE_64_WATCH_PAUSE_MUSIC_UNCOMPRESSED_REMAKE_777PROJEKT[] PROGMEM = "148";
const char TRACK_HACKER_MUSIC[] PROGMEM = "149";
const char TRACK_HAD_TO_PHONE_YA_REMASTERED_2000[] PROGMEM = "150";
const char TRACK_HARD_TO_EXPLAIN[] PROGMEM = "151";
const char TRACK_HITS_FROM_THE_BONG[] PROGMEM = "152";
const char TRACK_HOW_COME_U_DONT_CALL_ME_ANYMORE[] PROGMEM = "153";
const char TRACK_I_JUST_CALLED_TO_SAY_I_LOVE_YOU[] PROGMEM = "154";
const char TRACK_ITS_THE_MUSIC[] PROGMEM = "155";
const char TRACK_JUNIOR_VASQUEZ_IF_MADONNA_CALLS[] PROGMEM = "156";
const char TRACK_JUST_BE_A_MAN_ABOUT_IT_RADIO_EDIT[] PROGMEM = "157";
const char TRACK_KEHA_STEPHEN[] PROGMEM = "158";
const char TRACK_KRAFTWERK_THE_TELEPHONE_CALL[] PROGMEM = "159";
const char TRACK_LITTLE_MIX_HOW_YA_DOIN_FT_MISSY_ELLIOTT[] PROGMEM = "160";
const char TRACK_LONG_DISTANCE_RUNAROUND_2008_REMASTER[] PROGMEM = "161";
const char TRACK_LOVE_ON_THE_TELEPHONE[] PROGMEM = "162";
const char TRACK_MARY_WELLS_OPERATOR[] PROGMEM = "163";
const char TRACK_MABEL_DONT_CALL_ME_UP_OFFICIAL_VIDEO[] PROGMEM = "164";
const char TRACK_MIKE_JONES_BACK_THEN[] PROGMEM = "165";
const char TRACK_NEW_EDITION_MR_TELEPHONE_MAN_OFFICIAL_MUSIC_VIDEO[] PROGMEM = "166";
const char TRACK_OPERATOR_JIM_CROCE[] PROGMEM = "167";
const char TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG[] PROGMEM = "168";
const char TRACK_PICK_UP_THE_PHONE[] PROGMEM = "169";
const char TRACK_POPPY_MOSHI_MOSHI_OFFICIAL_VIDEO[] PROGMEM = "170";
const char TRACK_PRIMITIVE_RADIO_GODS_STANDING_OUTSIDE_A_BROKEN_PHONE_BOOTH_WITH_MONEY_IN_MY_HAND_1996[] PROGMEM = "171";
const char TRACK_R_E_M_STAR_69_MONSTER_REMASTERED[] PROGMEM = "172";
const char TRACK_RAFFI_BANANAPHONE[] PROGMEM = "173";
const char TRACK_RAPP_SNITCH_KNISHES[] PROGMEM = "174";
const char TRACK_RESISTOR_CORDLESS_PHONE[] PROGMEM = "175";
const char TRACK_RING_RING_ENGLISH_VERSION[] PROGMEM = "176";
const char TRACK_SALT_SHAKER[] PROGMEM = "177";
const char TRACK_SMOOTH_OPERATOR_REMASTERED[] PROGMEM = "178";
const char TRACK_SOULJA_BOY_TELLEM_FT_SAMMIE_KISS_ME_THRU_THE_PHONE_OFFICIAL_VIDEO[] PROGMEM = "179";
const char TRACK_SPIDERWEBS[] PROGMEM = "180";
const char TRACK_STEELY_DAN_RIKKI_DONT_LOSE_THAT_NUMBER[] PROGMEM = "181";
const char TRACK_SUBLIME_DONT_PUSH[] PROGMEM = "182";
const char TRACK_THE_ORLONS_DONT_HANG_UP[] PROGMEM = "183";
const char TRACK_TELEPHONE[] PROGMEM = "184";
const char TRACK_THE_BIG_BOPPER_CHANTILLY_LACE_HQ[] PROGMEM = "185";
const char TRACK_THE_CALL_BACKSTREET_BOYS_FEAT_THE_NEPTUNES_REMIX[] PROGMEM = "186";
const char TRACK_THE_DEVIL_WENT_DOWN_TO_GEORGIA[] PROGMEM = "187";
const char TRACK_THE_KINKS_PARTY_LINE_HQ[] PROGMEM = "188";
const char TRACK_THE_MARVELETTES_BEECHWOOD_4_5789[] PROGMEM = "189";
const char TRACK_THE_TIME_777_9311_SINGLE_VERSION[] PROGMEM = "190";
const char TRACK_THEME_SONG_KIM_POSSIBLE_DISNEY_CHANNEL[] PROGMEM = "191";
const char TRACK_THIZZLE_DANCE[] PROGMEM = "192";
const char TRACK_TIME_5551212[] PROGMEM = "193";
const char TRACK_TODD_RUNDGREN_HELLO_ITS_ME_1972[] PROGMEM = "194";
const char TRACK_TOMMY_TUTONE_867_5309JENNY[] PROGMEM = "195";
const char TRACK_TONY_BOOTH_LONESOME_7_7203[] PROGMEM = "196";
const char TRACK_VIDEO_PHONE[] PROGMEM = "197";
const char TRACK_VILLAGE_PEOPLE_SEX_OVER_THE_PHONE_OFFICIAL_MUSIC_VIDEO_1985[] PROGMEM = "198";
const char TRACK_WOO_WOO[] PROGMEM = "199";
const char TRACK_WEIRD_AL_YANKOVIC_WHITE_NERDY_OFFICIAL_4K_VIDEO[] PROGMEM = "200";
const char TRACK_YOUNG_LUST[] PROGMEM = "201";
const char TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT[] PROGMEM = "202";
const char TRACK_SKYY_CALL_ME_SINGLE_VERSION[] PROGMEM = "203";
// END OF GENERATED SECTION

const char* const RANDOM_TRACKS[] PROGMEM = {
  TRACK_DEFCON_INTRO, // 1
	TRACK_CARLY_RAE_JEPSEN_CALL_ME_MAYBE,
	TRACK_RAFFI_BANANAPHONE,
	TRACK_CARELESS_WHISPER,
	TRACK_5_7_0_5,
	TRACK_6060842,
	TRACK_THE_TIME_777_9311_SINGLE_VERSION,
	TRACK_853_5937,
	TRACK_SPIDERWEBS, // 9
	TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER, // 0

	TRACK_GO_DOWN_TOGETHER,
	TRACK_CAKE_NEVER_THERE,
	TRACK_BLONDIE_CALL_ME,
	TRACK_BACKSTREET_BOYS_THE_CALL_OFFICIAL_HD_VIDEO,
	TRACK_HARD_TO_EXPLAIN,
	TRACK_KRAFTWERK_THE_TELEPHONE_CALL,
	TRACK_PICK_UP_THE_PHONE,
	TRACK_HOW_COME_U_DONT_CALL_ME_ANYMORE,
	TRACK_NEW_EDITION_MR_TELEPHONE_MAN_OFFICIAL_MUSIC_VIDEO,
	TRACK_RING_RING_ENGLISH_VERSION,
	TRACK_PRIMITIVE_RADIO_GODS_STANDING_OUTSIDE_A_BROKEN_PHONE_BOOTH_WITH_MONEY_IN_MY_HAND_1996,
	TRACK_KEHA_STEPHEN,
	TRACK_TELEPHONE,
	TRACK_HACKER_MUSIC,
	TRACK_CALL_ME_BACK_AGAIN,
	TRACK_THE_BIG_BOPPER_CHANTILLY_LACE_HQ,
	TRACK_MARY_WELLS_OPERATOR,
	TRACK_YOUNG_LUST,
	TRACK_CALL_ME,
	TRACK_DE_LA_SOUL_RING_RING_RING_HA_HA_HEY_OFFICIAL_MUSIC_VIDEO,
	TRACK_THE_CALL_BACKSTREET_BOYS_FEAT_THE_NEPTUNES_REMIX,
	TRACK_SKYY_CALL_ME_SINGLE_VERSION,
	TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG,
	TRACK_OPERATOR_JIM_CROCE,
	TRACK_RAPP_SNITCH_KNISHES,
	TRACK_SMOOTH_OPERATOR_REMASTERED,
	TRACK_LITTLE_MIX_HOW_YA_DOIN_FT_MISSY_ELLIOTT,
	TRACK_POPPY_MOSHI_MOSHI_OFFICIAL_VIDEO,
	TRACK_VIDEO_PHONE,
	TRACK_HAD_TO_PHONE_YA_REMASTERED_2000,
	TRACK_THE_MARVELETTES_BEECHWOOD_4_5789,
	TRACK_BLONDIE_HANGING_ON_THE_TELEPHONE,
	TRACK_R_E_M_STAR_69_MONSTER_REMASTERED,
	TRACK_GLENN_MILLER_PENNSYLVANIA_6_5000_1940_DIGITALLY_REMASTERED,
	TRACK_LOVE_ON_THE_TELEPHONE,
	TRACK_HITS_FROM_THE_BONG,
	TRACK_MABEL_DONT_CALL_ME_UP_OFFICIAL_VIDEO,
	TRACK_ITS_THE_MUSIC,
	TRACK_I_JUST_CALLED_TO_SAY_I_LOVE_YOU,
	TRACK_THE_KINKS_PARTY_LINE_HQ,
	TRACK_LONG_DISTANCE_RUNAROUND_2008_REMASTER,
	TRACK_THEME_SONG_KIM_POSSIBLE_DISNEY_CHANNEL,
	TRACK_SUBLIME_DONT_PUSH,
	TRACK_SOULJA_BOY_TELLEM_FT_SAMMIE_KISS_ME_THRU_THE_PHONE_OFFICIAL_VIDEO,
	TRACK_RESISTOR_CORDLESS_PHONE,
	TRACK_MIKE_JONES_BACK_THEN,
	TRACK_JUST_BE_A_MAN_ABOUT_IT_RADIO_EDIT,
	TRACK_JUNIOR_VASQUEZ_IF_MADONNA_CALLS,
	TRACK_DRAKE_HOTLINE_BLING_LYRICS,
	TRACK_CALL_ME_MR_TELEPHONE_CHEYNE_1985_1_USA_DANCE_CHARTS,
	TRACK_THIZZLE_DANCE,
	TRACK_BECK_CELLPHONES_DEAD,
	TRACK_R0T0RR00T3R_BLOW_THE_WHISTLE_BAD_CUT,
	TRACK_STEELY_DAN_RIKKI_DONT_LOSE_THAT_NUMBER,
	TRACK_THE_ORLONS_DONT_HANG_UP,
	TRACK_WOO_WOO,
	TRACK_TOMMY_TUTONE_867_5309JENNY,
	TRACK_TONY_BOOTH_LONESOME_7_7203,
	TRACK_TODD_RUNDGREN_HELLO_ITS_ME_1972,
  TRACK_GOLDENEYE_64_WATCH_PAUSE_MUSIC_UNCOMPRESSED_REMAKE_777PROJEKT,
  TRACK_CISCO_DEFAULT_HOLD_MUSIC_HQ_MONO_AUDIO_OPUS_NUMBER_1,
  TRACK_THE_DEVIL_WENT_DOWN_TO_GEORGIA,
  TRACK_WEIRD_AL_YANKOVIC_WHITE_NERDY_OFFICIAL_4K_VIDEO,
  TRACK_TIME_5551212,
  TRACK_10_10_220_CHRISTOPHER_LLOYD_TAXI_COMMERCIAL_1999

};


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

/* struct Mapping { */
/*   const char* numbers[20]; */
/*   const char* tracks[10]; */
/* }; */

/* String checkSpecialNumber(String number) { */
/*   Mapping mappings[] = { */
/*     {{"8675309", NULL}, {TRACK_TOMMY_TUTONE_867_5309JENNY, NULL}}, */
/*     {{"911", NULL}, {TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG, NULL}}, */
/*     {{"666", NULL}, {TRACK_THE_DEVIL_WENT_DOWN_TO_GEORGIA, NULL}}, */
/*     {{"18006492568", "19006492568", NULL}, {TRACK_BABY_GOT_BACK, NULL}}, */
/*     {{"6060842", NULL}, {TRACK_6060842, NULL}}, */
/*     {{"285538", "6224", "62242583", NULL}, {TRACK_HACKER_MUSIC, NULL}}, */
/*     {{"7258", NULL}, {TRACK_SALT_SHAKER, NULL}}, */
/*     {{"33582", NULL}, {TRACK_ITS_THE_MUSIC, NULL}}, */
/*     {{"629", NULL}, {TRACK_GO_DOWN_TOGETHER, NULL}}, */
/*     {{"687", "3265", "3666", NULL}, {TRACK_RAPP_SNITCH_KNISHES, NULL}}, */
/*     {{"6424637", NULL}, {TRACK_HARD_TO_EXPLAIN, NULL}}, */
/*     {{"2455624", NULL}, {TRACK_THIZZLE_DANCE, NULL}}, */
/*     {{"32510", "510", NULL}, {TRACK_WOO_WOO, NULL}}, */
/*     {{"32207", "207", NULL}, {TRACK_BALLAD_OF_THE_20TH_MAINE, NULL}}, */
/*     {{"5551212", NULL}, {TRACK_TIME_5551212, NULL}}, */
/*     {{"737542268", NULL}, {TRACK_DRAKE_HOTLINE_BLING_LYRICS, NULL}}, */
/*     {{"5338", "6373", "1337", "31337", NULL}, {TRACK_WEIRD_AL_YANKOVIC_WHITE_NERDY_OFFICIAL_4K_VIDEO, NULL}}, */
/*     {{"633874", "349433", "834543", NULL}, {TRACK_CISCO_DEFAULT_HOLD_MUSIC_HQ_MONO_AUDIO_OPUS_NUMBER_1, NULL}}, */
/*     {{"333266", "33326631", NULL}, {TRACK_HACKER_MUSIC, NULL}}, */
/*     {{"333266", "1010220", NULL}, {TRACK_10_10_220_CHRISTOPHER_LLOYD_TAXI_COMMERCIAL_1999, NULL}}, */
/*     {{"7779311", NULL}, {TRACK_THE_TIME_777_9311_SINGLE_VERSION, NULL}}, */
/*     {{"6345789", NULL}, {TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER, NULL}}, */
/*     {{"2813308004", NULL}, {TRACK_MIKE_JONES_BACK_THEN, NULL}}, */
/*     {{"411", "311", NULL}, {TRACK_GOLDENEYE_64_WATCH_PAUSE_MUSIC_UNCOMPRESSED_REMAKE_777PROJEKT, NULL}}, */
/*     {{"4390116", NULL}, {TRACK_SUBLIME_DONT_PUSH, NULL}}, */
/*     {{"420", "808", "707", "505", "202", NULL}, {TRACK_HITS_FROM_THE_BONG, NULL}}, */
/*     {{"5108160458", "0", NULL}, {TRACK_SMOOTH_OPERATOR_REMASTERED, NULL}}, */
/*     {{"69", NULL}, {TRACK_2_LIVE_CREW_ME_SO_HORNY, TRACK_AREA_CODES_FEAT_NATE_DOGG_BY_LUDACRIS_CRATE_CLASSICS_RAP, TRACK_CARELESS_WHISPER, TRACK_VILLAGE_PEOPLE_SEX_OVER_THE_PHONE_OFFICIAL_MUSIC_VIDEO_1985, TRACK_BABY_GOT_BACK, TRACK_SALT_SHAKER, TRACK_ERIC_PRYDZ_CALL_ON_ME, NULL}}, */
/*   }; */

/*   String trackNumber = "0"; */

/*   for (int i = 0; i < sizeof(mappings) / sizeof(mappings[0]); ++i) { */
/*     for (int j = 0; mappings[i].numbers[j] != NULL; ++j) { */
/*       if (number == mappings[i].numbers[j]) { */
/*         int numTracks = 0; */
/*         while (mappings[i].tracks[numTracks] != NULL) numTracks++; */
/*         int randPick = random(0, numTracks); */
/*         trackNumber = getProgmem(mappings[i].tracks[randPick]); */
/*         return trackNumber; */
/*       } */
/*     } */
/*   } */

/*   return trackNumber; */
/* } */


String checkSpecialNumber(String number) {
    String trackNumber = "0";
    if (number == "8675309") {
        trackNumber = getProgmem(TRACK_TOMMY_TUTONE_867_5309JENNY);
    } else if (number == "77203") {
        trackNumber = getProgmem(TRACK_TONY_BOOTH_LONESOME_7_7203);
    } else if (number == "911") {
        trackNumber = getProgmem(TRACK_ORIGINAL_GHOSTBUSTERS_THEME_SONG);
    } else if (number == "666") {
        trackNumber = getProgmem(TRACK_THE_DEVIL_WENT_DOWN_TO_GEORGIA);
    } else if (number == "18006492568" || number == "19006492568") {
        trackNumber = getProgmem(TRACK_BABY_GOT_BACK);
    } else if (number == "6060842") {
        trackNumber = getProgmem(TRACK_6060842);
    } else if (number == "285538" || number == "6224" || number == "62242583") { // MACH
        trackNumber = getProgmem(TRACK_HACKER_MUSIC);
    } else if (number == "7258") { // SALT
        trackNumber = getProgmem(TRACK_SALT_SHAKER);
    } else if (number == "33582") { // DELTA
        trackNumber = getProgmem(TRACK_ITS_THE_MUSIC);
    } else if (number == "629") { // MAY
        trackNumber = getProgmem(TRACK_GO_DOWN_TOGETHER);
    } else if (number == "687" || number == "3265" || number == "3666") { // MVR DANK DOOM
        trackNumber = getProgmem(TRACK_RAPP_SNITCH_KNISHES);
    } else if (number == "6424637") { // MHAIMES
        trackNumber = getProgmem(TRACK_HARD_TO_EXPLAIN);
    } else if (number == "2455624") { // BILLMAG
        trackNumber = getProgmem(TRACK_THIZZLE_DANCE);
    } else if (number == "32510" || number == "510") { // DC510
        trackNumber = getProgmem(TRACK_WOO_WOO);
    } else if (number == "32207" || number == "207") { // DC207
        trackNumber = getProgmem(TRACK_BALLAD_OF_THE_20TH_MAINE);
    } else if (number == "5551212") { // 5551212
        trackNumber = getProgmem(TRACK_TIME_5551212);
    } else if (number == "737542268") { // REPLICANT
        trackNumber = getProgmem(TRACK_DRAKE_HOTLINE_BLING_LYRICS);
    } else if (number == "5338" || number == "6373" || number == "1337" || number == "31337") { // LEET NERD
        trackNumber = getProgmem(TRACK_WEIRD_AL_YANKOVIC_WHITE_NERDY_OFFICIAL_4K_VIDEO);
    } else if (number == "633874" || number == "349433" || number == "834543") { // MEDUS4 OR DIXIE3 OR VEILID
        trackNumber = getProgmem(TRACK_CISCO_DEFAULT_HOLD_MUSIC_HQ_MONO_AUDIO_OPUS_NUMBER_1);
    } else if (number == "333266" || number == "33326631") { // DEFCON or DEFCON31
        trackNumber = getProgmem(TRACK_HACKER_MUSIC);
    } else if (number == "333266" || number == "1010220") { // 1010220
        trackNumber = getProgmem(TRACK_10_10_220_CHRISTOPHER_LLOYD_TAXI_COMMERCIAL_1999);
    } else if (number == "7779311") {
        trackNumber = getProgmem(TRACK_THE_TIME_777_9311_SINGLE_VERSION);
    } else if (number == "6345789") {
        trackNumber = getProgmem(TRACK_634_5789_SOULSVILLE_U_S_A_2007_REMASTER);
    } else if (number == "2813308004") {
        trackNumber = getProgmem(TRACK_MIKE_JONES_BACK_THEN);
    } else if (number == "411" || number == "311") {
        trackNumber = getProgmem(TRACK_GOLDENEYE_64_WATCH_PAUSE_MUSIC_UNCOMPRESSED_REMAKE_777PROJEKT);
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
    } else if (number == "5108160458") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "0") {
        trackNumber = getProgmem(TRACK_SMOOTH_OPERATOR_REMASTERED);
    } else if (number == "69") {
        int randPick = random(0, 6);
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
        } else if (randPick == 5) {
            trackNumber = getProgmem(TRACK_SALT_SHAKER);
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
    //if (tmpCounter == 1) {
        //uncomment to monitor
        //Serial.println(String(resetReading) + " " + String(resetLastState) + " " + String(resetLastTrueState));
    //}
    //tmpCounter++;
    //tmpCounter = tmpCounter % 10000;
    if ((millis() - rotaryChangeTime) > stoppedDialingMs) {
        if (totalNumberDialed) {
            Serial.println("Dialing: " + totalNumber);
            MP3player.stopTrack();
            if (currentGame == 0) {
                String specialNumber = checkSpecialNumber(totalNumber);
                if (specialNumber != "0") {
                    Serial.println("Special number");
                    setTotalNumberWithMemory(specialNumber);
                } else {
                    // game not yet started
                    // choose from initial menu
                    int newRandom = getRandom();
                    // check to see if random track should be played
                    int i;
                    if (totalNumber == "1980") {
                        Serial.println("1980 - start game");
                        currentGame = 1;
                        totalNumber = "2";
                    } else if (totalNumber == "1995" || totalNumber == "4775619" || totalNumber == "2134775619" || totalNumber == "200" || totalNumber == "266") {
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
                            //playAdventure();
                            currentGame = 1;
                            totalNumber = "1";
                        } else {
                            setTotalNumberWithMemoryFromBuffer();
                        }
                    } else {
                        Serial.println("Fall through, start adventure");
                        //playAdventure();
                        currentGame = 1;
                        totalNumber = "1";
                    }
                }
            } else if (currentGame == 1) {
                if (totalNumber == "0") {
                    totalNumber = "10";
                } else if (totalNumber == "1980") {
                    totalNumber = "2";
                }
            }
            Serial.println("Playing " + totalNumber);
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

    // reset if the phone has been on the hook for a while
    if (resetLastTrueState == HIGH && (millis() - resetChangeTime) > stoppedDialingMs) {
        if (resetReading != resetLastTrueState) {
            Serial.println("Eating it!");
            resetLastTrueState = resetReading;
            if (resetLastTrueState == LOW) {
                Serial.println("We should start!");
                Serial.println("Reset!");
                activated = true;
                MP3player.stopTrack();
                currentGame = 0;
                totalNumber = "";
                MP3player.playTrack(stringToNumber(getProgmem(TRACK_DEFCON_INTRO)));
            } else {

            }
        }
    }

    if ((millis() - resetChangeTime) > resetAfterMs) {
        if (singleNumberResetDialed) {
            Serial.println("Dialed reset: " + resetCount % 10);
            totalNumber = totalNumber + String(resetCount % 10);
            Serial.println("Total number reset is: " + totalNumber);
            singleNumberResetDialed = false;
            totalNumberResetDialed = true;
            //totalNumberDialed = true;
            resetCount = 0;
        }
    }

    if ((millis() - resetChangeTime) > stoppedDialingMs) {
        if (totalNumberResetDialed) {
          Serial.println("Total number reset2 is: " + totalNumber);
          singleNumberResetDialed = false;
          totalNumberDialed = true;
          totalNumberResetDialed = false;
          count = 0;
          resetCount = 0;
        }
    }

    if (resetReading != resetLastState) {
        resetChangeTime = millis();
    }

    if ((millis() - resetChangeTime) > resetDebounceDelay) {
        if (resetReading != resetLastTrueState) {
            resetLastTrueState = resetReading;
            if (resetLastTrueState == HIGH) {
                resetCount++;
                Serial.println("Incrementing count reset!");
                singleNumberResetDialed = true;
            }
        }
    }

    resetLastState = resetReading;
}
