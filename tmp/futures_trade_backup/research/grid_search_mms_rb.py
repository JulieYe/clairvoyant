import os

stoploss = [-100]
takeprofit = [50]
bandwidth = [200]
ordersize = [1]
volthreshold = [0.5]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for bw in bandwidth:
            for vol in volthreshold:
                os.system("python mms_sim.py -s %d -t %d -w %d -v %f -c SHFE:rb -d SHFE -z %d -a MMS -b %s" % (sl, tp, bw, vol, orderSize, 'MMS_sim'))
