import speech_recognition as sr
import pyttsx3
import pywhatkit
import datetime
import wikipedia
import pyjokes
# from cv2 import *
import os


# Ears
listener = sr.Recognizer()

# Mouth
engine = pyttsx3.init()
engine.setProperty("rate", 180)
# voices = engine.getProperty('voices')
# engine.setProperty('voice', voices[1].id)


def talk(text_to_speach):
    engine.say(text_to_speach)
    engine.runAndWait()


def take_command():
    try:
        with sr.Microphone() as source:
            print('listening...')
            voice = listener.listen(source)
            command = listener.recognize_google(voice)
            command = command.lower()
            if 'raptor' in command:
                command = command.replace('raptor', '')
                print(command)
    except:
        pass
    return command


def run_raptor():
    command = take_command()
    if 'play' in command:
        song = command.replace('play', '')
        talk('Now playing' + song)
        pywhatkit.playonyt(song, True)
    elif 'time' in command:
        time = datetime.datetime.now().strftime('%I:%M %p')
        talk('The current time is' + time)
    elif 'google' in command:
        noun = command.replace('google', '')
        info = wikipedia.summary(noun, 1)
        talk('Let me google that for you, here is what I found')
        talk(info)
    elif 'take my picture' in command:
        talk('Say lie po batteries')
#        dtg = datetime.datetime.now().strftime('%Y%d%H%M')
#        cam = VideoCapture(0)
#        result, image = cam.read()
#        if result:
#            imwrite("~/Pictures/" + dtg + ".png", image)
#            pywhatkit.image_to_ascii_art("~/Pictures/" + dtg + ".png", "~/Pictures/" + dtg + ".txt")
#            talk('Picture stored')
#        else:
        talk('Sorry, issues taking picture')
        talk('I\'m sure it was me, you could not have broken tha camera')
    elif 'joke' in command:
        talk(pyjokes.get_joke())
    else:
        talk('I\'m sorry Dave, I can\'t do that')
        talk('Hal nine thousand is one of my hero\'s')


while True:
    run_raptor()
