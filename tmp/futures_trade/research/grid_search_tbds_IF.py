import os
import numpy

threshold = [0.53]
threshold_vol = [0.2]
ordersize = [1]

for orderSize in ordersize:
    for th in threshold:
        for th_vol in threshold_vol:
            os.system("python tbds_sim.py -e %f -v %f -c CFFEX:IF -d CFFEX -z %d -a TBDS" % (th, th_vol, orderSize))
