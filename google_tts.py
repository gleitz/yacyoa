import os
import argparse
import html
import re
from datetime import datetime

from google.cloud import texttospeech

# All voices including multi-language
#SUPPORTED_VOICES = ["en-US-Wavenet-A", "en-US-Wavenet-B", "en-US-Wavenet-C", "en-US-Wavenet-D", "en-US-Wavenet-E", "en-US-Wavenet-F", "en-US-Wavenet-G", "en-US-Wavenet-H", "en-US-Wavenet-I", "en-US-Wavenet-J", "en-US-Neural2-A", "en-US-Neural2-C", "en-US-Neural2-D", "en-US-Neural2-E", "en-US-Neural2-F", "en-US-Neural2-G", "en-US-Neural2-H", "en-US-Neural2-I", "en-US-Neural2-J", "es-US-Neural2-A", "es-US-Neural2-B", "es-US-Neural2-C", "fr-CA-Neural2-A", "fr-CA-Neural2-B", "fr-CA-Neural2-C", "fr-CA-Neural2-D", "en-US-Studio-M", "en-US-Studio-O", "en-US-News-K", "en-US-News-L", "en-US-News-M", "en-US-News-N"]
# Voices in the 'Curated' set (https://www.notion.so/replicant/Thinking-Machine-Voices-27484593161641c0a0079c717163c66a?pvs=4)
#SUPPORTED_VOICES = ["en-US-Neural2-C", "en-US-Neural2-D", "en-US-Neural2-E", "en-US-Neural2-F", "en-US-Neural2-G", "en-US-Neural2-H", "en-US-Neural2-I", "en-US-Neural2-J", "en-US-News-M", "en-US-News-N", "en-US-Studio-M", "en-US-Studio-O"]
# All English voices
SUPPORTED_VOICES = ["en-US-Wavenet-A", "en-US-Wavenet-B", "en-US-Wavenet-C", "en-US-Wavenet-D", "en-US-Wavenet-E", "en-US-Wavenet-F", "en-US-Wavenet-G", "en-US-Wavenet-H", "en-US-Wavenet-I", "en-US-Wavenet-J", "en-US-Neural2-A", "en-US-Neural2-C", "en-US-Neural2-D", "en-US-Neural2-E", "en-US-Neural2-F", "en-US-Neural2-G", "en-US-Neural2-H", "en-US-Neural2-I", "en-US-Neural2-J", "en-US-Studio-M", "en-US-Studio-O", "en-US-News-K", "en-US-News-L", "en-US-News-M", "en-US-News-N", "en-GB-News-H"]


def translate_text(target, text):
    """Translates text into the target language.
    Target must be an ISO 639-1 language code.
    See https://g.co/cloud/translate/v2/translate-reference#supported_languages
    """
    import six
    from google.cloud import translate_v2 as translate

    translate_client = translate.Client()

    if isinstance(text, six.binary_type):
        text = text.decode("utf-8")

    # Text can also be a sequence of strings, in which case this method
    # will return a sequence of results for each text.
    result = translate_client.translate(text, target_language=target)

    print(u"Text: {}".format(result["input"]))
    print(u"Translation: {}".format(html.unescape(result["translatedText"])))
    print(u"Detected source language: {}".format(result["detectedSourceLanguage"]))

    return html.unescape(result["translatedText"])


def synthesize(text, voice, speakingRate=1.0, isSsml=False, returnAudio=False, volume_gain_db=0.0):
    """Synthesizes speech from the input string of text."""
    client = texttospeech.TextToSpeechClient()

    language = voice[:5]
    if language not in ["en-US", "en-GB"]:
        text = translate_text(language, text)

    if isSsml == False:
        input_text = texttospeech.SynthesisInput(text=text)
    else:
        input_text = texttospeech.SynthesisInput(ssml=text)

    voice_arg = texttospeech.VoiceSelectionParams(
        language_code = language,
        name = voice
    )

    # audio_config = texttospeech.AudioConfig(
    #     audio_encoding = texttospeech.AudioEncoding.MP3,
    #     sample_rate_hertz = 8000
    # )

    audio_config = texttospeech.AudioConfig(
        audio_encoding = texttospeech.AudioEncoding.MP3,
        speaking_rate = float(speakingRate),
        volume_gain_db = volume_gain_db
    )

    response = client.synthesize_speech(
        request = {"input": input_text, "voice": voice_arg, "audio_config": audio_config}
    )

    if returnAudio:
        return response.audio_content

    output_path = os.path.join("output", "google", f"{voice}")
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    file_name = datetime.now().strftime("%Y%m%d-%H%M") + " " + re.sub(r'<[^>]+>', '', text)[:100]
    #file_name = voice

    output_path = os.path.join(output_path, f"{file_name}.mp3")
    with open(output_path, "wb") as file:
        file.write(response.audio_content)
        print(f'Audio content written to file "{output_path}"')


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )

    group = parser.add_mutually_exclusive_group(required = True)
    group.add_argument("--text", help="The text from which to synthesize speech.")
    group.add_argument("--file", help="Path to the text file from which to synthesize speech. This will be interpreted as plaintext, one line per audio clip.")

    parser.add_argument("--voice", help="The voice for the synthesized speech. Outputs all supported voices if none is specified.", choices=SUPPORTED_VOICES)

    parser.add_argument("--ssml", help="Denotes that the text/file should be interpreted as SSML.", action='store_true')
    parser.add_argument("--rate", help="Speaking rate for the synthesized speech, between 0.25 and 4.0. Defaults to 1.0.", default=1.0, type=float)

    args = parser.parse_args()

    if args.file:
        try:
            with open(args.file, 'r') as file:
                text_list = [line.strip() for line in file.readlines()]
        except FileNotFoundError:
            print("File not found.")
        except IOError:
            print("Error reading file.")

    # Use the specified voice; otherwise, output all the supported voices
    voice_list = [args.voice] if args.voice else SUPPORTED_VOICES

    for voice in voice_list:
        print(f"Generating voiceover as {voice}")
        if args.text:
            synthesize(args.text, voice, args.rate, args.ssml)
        elif args.file:
            for text in text_list:
                synthesize(text, voice, args.rate, args.ssml)
        else:
            print("Must specify text or ssml to synthesize.")
