import os

stoploss = [-100]
takeprofit = [120]
threshold = [0.3]
volthreshold = [0.11]
bandwidth = [60]
ordersize = [1]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for bw in bandwidth:
            for th in threshold:
                for voth in volthreshold:
                    os.system("python mms_sim.py -s %d -t %d -w %d -c CFFEX:IF -d CFFEX -z %d -a MMS -b %s -e %s -v %s" % (sl, tp, bw, orderSize, 'MMS_sim', th, voth))
