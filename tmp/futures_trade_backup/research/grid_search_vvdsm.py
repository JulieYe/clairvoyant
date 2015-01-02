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
stoploss = [-300]
takeprofit = [240]
threshold = [0.51]
threshold_vol = [0.26]
# threshold = map(lambda x : round(x, 2), list(numpy.linspace(0.5, 0.54, 5)))
# threshold_vol = map(lambda x : round(x, 2), list(numpy.linspace(0.24, 0.28, 5)))
ordersize = [1]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for th in threshold:
            for th_vol in threshold_vol:
                os.system("python vds_sim.py -s %d -t %d -e %f -v %f -c CFFEX:IF -d CFFEX -z %d -a VVDS" % (sl, tp, th, th_vol, orderSize))

'''
stoploss = [-180]
takeprofit = [120]
bandwidth = [120]
threshold = [0.5]
threshold_vol = [0.5]
ordersize = [1]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for bw in bandwidth:
            for th in threshold:
                for th_vol in threshold_vol:
                    os.system("python mms_sim.py -s %d -t %d -w %d -e %f -v %f -c CFFEX:IF -d CFFEX -z %d -a MMS -b %s" % (sl, tp, bw, th, th_vol, orderSize, 'MMS_sim'))
'''
