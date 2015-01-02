import os

stoploss = [-120, -180, -240, -300, -360, -420, -480]
takeprofit = [120, 180, 240, 300, 360, 420, 480]
#stoploss = [-120, -180]
#takeprofit=[120, 180]
threshold = [0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6]

for sl, tp in zip(stoploss, takeprofit):
    for th in threshold:
        os.system("python vds1_sim.py -s %d -t %d -e %f -c IF"%(sl, tp, th))


