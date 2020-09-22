# -*- coding: utf-8 -*-
"""
Created on Fri Jul  3 08:48:17 2020

@author: bergholz
"""

import matplotlib.pyplot as plt

emg1 = []
bi = []

import csv
with open('example.txt', newline='') as csvfile:
  plots = csv.reader(csvfile, delimiter=',')
  for row in plots:
      bi.append(float(row[0]))
      emg1.append(float(row[1]))
      
plt.plot(emg1)
#plt.ylim(0, 0.004)