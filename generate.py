#!/usr/bin/env python

##################################################
#
# yacyoa - yet another choose your own adventure
# written by Benjamin Gleitzman (gleitz@mit.edu)
#
##################################################

import time
import shlex
import os
import subprocess
import re

PAGE_RE = re.compile('Page (\d+)')

# For voices using Mac OSX's say command
SAY_CMD = "say -v Serena -o /tmp/sub-{number}.aiff -f /tmp/{number}.txt -r 160 && lame /tmp/sub-{number}.aiff --tg Speech -b 160 -m m {output_path}"

# For voices using festival_client
# FESTIVAL_CLIENT_CMD = "festival_client --ttw --aucommand 'lame $FILE --tg Speech --preset mw-us {output_path}' /tmp/{number}.txt"
# FESTIVAL_CLIENT_CMD = "festival_client --ttw --aucommand 'sox $FILE /tmp/{number}.wav tempo 1.05; lame /tmp/{number}.wav --tg Speech --preset mw-us {output_path}' /tmp/{number}.txt"

# For voices using text2wave
# SPEAK_CMD = 'text2wave -o /tmp/output.wav /tmp/speak.txt -eval "(voice_cstr_uk_nick_8hour_hts)"'

TEMPO_CMD = 'sox /tmp/output.wav /tmp/output2.wav tempo 1.1'
LAME_CMD = 'lame /tmp/output2.wav --tg Speech --preset mw-us {0}'
AFPLAY_CMD = 'afplay {0}'

SOUNDS_DIR = os.path.join(os.path.dirname(__file__), 'sounds')

def parse_pages():
    pages = {}
    with open('chimney-rock.txt', 'r') as f:
        current_lines = ''
        current_page = False
        for line in f.readlines():
            if line.startswith('-'):
                if current_page:
                    pages[current_page] = current_lines
                    current_lines = ''
                current_page = int(PAGE_RE.findall(line)[0]) - 7
            else:
                current_lines += line


    return pages

def speak_pages(pages):
    for page_num, page in pages.items():
        if not page.strip():
            continue
        number = str(page_num).zfill(3)
        output_data = {'number': number,
                       'output_path': SOUNDS_DIR + '/track' + number + '.mp3'}

        with open('/tmp/{number}.txt'.format(**output_data), 'w') as f:
            f.write('[[volm 1.0]] ' + page)
        command = SAY_CMD.format(**output_data)
        print(command)
        subprocess.Popen(['/bin/bash', '-c', command])

def play_file(number):
    number = str(number).zfill(3)
    os.system(AFPLAY_CMD.format('sounds/track' + number + '.mp3'))

def navigate_story():
    play_file(2)
    while True:
        selection = input('Enter a page: ')
        play_file(selection)


if __name__ == '__main__':
    pages = parse_pages()
    speak_pages(pages)
