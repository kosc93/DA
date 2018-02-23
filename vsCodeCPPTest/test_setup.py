'''
Created on Feb 21, 2018

@author: kosc93
'''
from __future__ import division

from amsldm import __version__
from amsldm import MeasurementSetup, Sweep
from amsldm import FieldFox, PNA, ZVA24
from time import sleep,time
from dummy import Dummy
import os

os.system('guvcview -j test.avi  -o mp3 -u h264 -y 30 -e &')
m = MeasurementSetup("test_setup")
m.add_instrument(ZVA24('192.168.2.159'))
m.add_sweep(Sweep(Dummy('A'),0.,2.,5))


m.run()
