import os
import numpy

'''
stoploss = [-999999999]
takeprofit = [999999999]
threshold = map(lambda x : round(x, 2), list(numpy.linspace(0.28, 0.32, 5)))
threshold_vol = map(lambda x : round(x, 2), list(numpy.linspace(0.18, 0.21, 4)))
ordersize = [1]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for th in threshold:
            for th_vol in threshold_vol:
                os.system("python vds_sim.py -s %d -t %d -e %f -v %f -c CFFEX:IF -d CFFEX -z %d -a VUDS" % (sl, tp, th, th_vol, orderSize))

'''
stoploss = [-240]
takeprofit = [180]
#threshold = map(lambda x : round(x, 2), list(numpy.linspace(0.45, 0.53, 5)))
threshold = [0.45]
#threshold_vol = map(lambda x : round(x, 2), list(numpy.linspace(0.22, 0.3, 5)))
threshold_vol = [0.19]
ordersize = [1]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for th in threshold:
            for th_vol in threshold_vol:
                os.system("python vds_sim.py -s %d -t %d -e %f -v %f -c CFFEX:IF -d CFFEX -z %d -a VVDSM -b VVDSM_sim" % (sl, tp, th, th_vol, orderSize))
