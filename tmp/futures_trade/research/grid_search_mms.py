import os

stoploss = [-300]
takeprofit = [120]
bandwidth = [180, 240, 300, 360, 420, 480, 540, 600, 720, 900]
volthresh = [0, 0.5, 1, 1.5, 2]
ordersize = [1]
threshold = [0.2]

for orderSize in ordersize:
    for sl, tp in zip(stoploss, takeprofit):
        for vl in volthresh: 
            for bw in bandwidth:
                os.system("python mms_sim.py -s %d -t %d -w %d -c CFFEX:IF -d IFData -z %d -a MMS -b %s -v %f" % (sl, tp, bw, orderSize, 'MMS_sim', vl))
