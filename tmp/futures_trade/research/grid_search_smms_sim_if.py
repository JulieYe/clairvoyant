import os

edge = [0.4]
buffers = [0.4]
takeprofit = [[120, 120]]
stoploss = [[-120, -120]]
maxspreadposition = [2]
maxcancelqty = [99999999999]
binarys = ['SMMS_sim']

for eg in edge:
    for bf in buffers:
        for sl, tp in zip(stoploss, takeprofit):
            for msp in maxspreadposition:
                for mcq in maxcancelqty:
                    for br in binarys:
                        os.system("python smms_sim.py -e %f -f %f -t %d -T %s -s %d -S %d -p %s -q %s -z 1 -c CFFEX:IF -d CFFEX -b %s -o summary_smms_IF.csv" % (eg, bf, tp[0], tp[1], sl[0], sl[1], msp, mcq, br))
