Yet Another Choose Your Own Adventure
========================

Your digital host guides you through a deathly Choose Your Own Adventure game pirated from the Library of Congress.


Usage
-----

*  `python generate.py`
*  `generate.navigate_story()`

To Update Phones
----------------

First you want to replace the main file, which is `track122.mp3`
Do this by generating audio and then splicing it into the editing track122. Save the audio as 160kbps mono.
`python google_tts.py --voice en-GB-News-H --rate 1.1 --text "Thank you for calling Replicant Online. All representatives are currently assisting other callers. If you know your department's extension, please try dialing it at any time."`
`python synthesize.py --voice Arthur --text "Thank you for calling DEFCON Online. All representatives are currently assisting other callers. If you know your department's extension, please try dialing it at any time."`
> also Sassy Aerisita
> you might want to use the model "Eleven Turbo v2" if it's for English

Next you want to make sure the opening line of the game says the right thing, which is contained in `track002.mp3`
Edit `chimney-rock-edit.txt` and then run `python generate.py`
Export will be in the variable `SOUNDS_DIR`

The Black phone is an Arduino Duemilanove
The Red phone is an Arduino Uno

Make sure the libraries/ folder is copied to ~/Documents/Arduino/libraries/ (then restart Arduino IDE)

Author
------

*  Benjamin Gleitzman ([@gleitz](http://twitter.com/gleitz))

Extra notes
-----------

*  Can be used with `telephone.ino` to upload the game to an Arduino
*  Many thanks to Edward Packard, Paul Granger, and Bantam Books for use of [The Mystery of Chimney Rock](http://www.amazon.com/Mystery-Chimney-Rock-Choose-Adventure/dp/0553128183). I urge you to make your classic works available digitally for children of all ages to enjoy.
