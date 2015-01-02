import os

# stoploss = [-120, -180, -240, -300, -360]
# takeprofit = [120, 180, 240, 300, 360]
# threshold = [0.1, 0.2, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8]

# stoploss = [-120, -180, -240]
# takeprofit = [120, 180, 240]
# threshold = [0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65]

stoploss = [-360]
takeprofit = [180]
threshold = [0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75]

for sl, tp in zip(stoploss, takeprofit):
    for th in threshold:
        os.system("python vds2_sim.py -s %d -t %d -e %f -c IF" % (sl, tp, th))
