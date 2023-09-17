#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
import sys
from time import sleep
from datetime import datetime 
import secrets
import pyanalyzer
import devices.oscilloscopes.infiniium as infiniium
import logging

# Open serial port
con = serial.Serial("/dev/ttyACM0", 115200)

#logging.basicConfig(level=logging.DEBUG)

# Create the instrument
inst = infiniium.infiniium('gigascope', timeout=30) # Create an Infiniium object using its DNS name
# Load a sedata = Ptup
inst.setup('STM32L475_Skinny128-384_ref.set')
#preamble = inst.get_preamble()

# we assume that the other end encodes the data using ascii as well
def read(amount):
    counter = 0
    s = bytearray()
    c = ''

    # for the trailing '\n' which signals the end of the frame
    while counter != (amount + 1):
        c = con.read()
        if c != '':
            s += c
            counter += 1
    if '\n'.encode('ascii') != bytes([s[-1]]):
        print('did not get the expected end of frame character')
        return

    return s[:-1]

def write(data):
    for c in data:
        con.write(c.encode('ascii'))



## Skinny -128 -384+ 
##Key : d f 8 8 9 5 4 8 c f c 7 e a 5 2 d 2 9 6 3 3 9 3 0 1 7 9 7 4 4 9
##      a b 5 8 8 a 3 4 a 4 7 f 1 a b 2 d f e 9 c 8 2 9 3 f b e a 9 a 5
##      a b 1 a f a c 2 6 1 1 0 1 2 c d 8 c e f 9 5 2 6 1 8 c 3 e b e 8
##Plaintext :  0xa3 0x99 0x4b 0x66 0xad 0x85 0xa3 0x45 0x9f 0x44 0xe9 0x2b 0x08 0xf5 0x50 0xcb
##Ciphertext : 0xff 0x38 0xd1 0xd2 0x4c 0x86 0x4c 0x43 0x52 0xa8 0x53 0x69 0x0f 0xe3 0x6e 0x5e


SKINNY_MODE_REF = 'r' # for the reference version
SKINNY_MODE_SEC = 's' # for the secure version

# Key will never change
KEY = 'df889548cfc7ea52d296339301797449ab588a34a47f1ab2dfe9c8293fbea9a5ab1afac2611012cd8cef952618c3ebe8'

#PLAINTEXT = 'a3994b66ad85a3459f44e92b08f550cb' #if you want the same plaintext every time

i = 0
while(True):

    # Generate random plaintext
    PLAINTEXT = secrets.token_hex(16)
    data = PLAINTEXT + KEY + SKINNY_MODE_REF

    fname = 'Refskinny_'
    fname += datetime.now().isoformat(timespec='seconds')
    #fname += datetime.now(). strftime('%Y-%m-%dT%H:%M:%S') #PYTHON2
    fname += '_pt=' + PLAINTEXT

    # Digitize
    inst.digit()
    
    # send pt and key to the board, don't forget to specify the Skinny version
    write(data)
    sleep(0.1)
    # read the cyphertext from the key
    CYPHERTEXT = read(32)
    fname += '_ct=' + CYPHERTEXT.decode()
    fname += '_key=' + KEY
    fname += '.txt'

    print("{}\t{}".format(i, CYPHERTEXT))

    # Get the trace from the oscilloscope
    wave = inst.get_wave('CHAN2')

    # If you want to debug the trace
    '''
    print "DEBUG len =", len(wave), type(wave), type
    for c in wave:
        s = ord(c)
        if s >= 128: s -= 128
        print s
    '''

    # write the cyphertext to captures/fname file
    f = open("captures/" + fname, "w") #"w" command will overwrite any existing file found with the same file name.
    f.write(str(CYPHERTEXT))
    f.close()
    i += 1
    sys.exit(0)


