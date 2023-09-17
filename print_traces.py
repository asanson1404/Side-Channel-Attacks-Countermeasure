#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
import matplotlib.pyplot as plt

path_dir = "/home/asanson/capture_ref/"

#files = os.listdir(path_dir) # to run all the files
files = ['Refskinny_2023-02-08T15:53:57_pt=00000000000000000000000000000000_ct=2e92d2ebc097db91bac1188acddbe779_key=df889548cfc7ea52d296339301797449ab588a34a47f1ab2dfe9c8293fbea9a5ab1afac2611012cd8cef952618c3ebe8.txt',
         'Refskinny_2023-02-08T15:55:01_pt=ffffffffffffffffffffffffffffffff_ct=ddb920dfbacad1ed65ffbe8d572f2d64_key=df889548cfc7ea52d296339301797449ab588a34a47f1ab2dfe9c8293fbea9a5ab1afac2611012cd8cef952618c3ebe8.txt',
         'Refskinny_2023-02-08T16:00:55_pt=0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f_ct=3fe71bf2046ce6cb705cfb0e183ae14f_key=df889548cfc7ea52d296339301797449ab588a34a47f1ab2dfe9c8293fbea9a5ab1afac2611012cd8cef952618c3ebe8.txt'
        ]

traces = []
print (len(files))

for fname in files:
    f = open(path_dir+fname, "r")
    traces.append(np.frombuffer(f.read(), dtype=np.int8))
    f.close()

print(len(traces[0]))

absc = range(1, len(traces[2])+1)
plt.plot(absc, traces[2], label='pt=0f')
plt.title('Trace avec plaintext a 0f')

plt.plot(absc, traces[0], label='pt=0')
plt.title('Trace avec plaintext set a 0')
#plt.show()

plt.plot(absc, traces[1], label='pt=1')
plt.title('Trace avec plaintext set a 1')
#plt.show()



#plt.plot(absc, traces[2] - traces[3], label='diff')
#plt.title('Graphique de diff')
plt.legend()
plt.show()