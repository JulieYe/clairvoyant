import os

stoploss = [-180, -190, -210, -220, -230, -240, -250]
takeprofit = [180, 190, 210, 220, 230, 240, 250]
threshold = [0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9]

for sl, tp in zip(stoploss, takeprofit):
    for th in threshold:
        os.system("python vds_sim.py -s %d -t %d -e %f -c SHFE:rb -d SHFE" % (sl, tp, th))
