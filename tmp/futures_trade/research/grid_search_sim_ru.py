import os

#stoploss = [-800, -1000, -1200, -1400, -1600, -1800, -2000]
#takeprofit = [800, 1000, 1200, 1400, 1600, 1800, 2000]
#stoploss = [-100, -100, -100, -100, -100]
#takeprofit = [300, 400, 500, 600, 700]
#stoploss = [-250, -500, -1000, -250, -500, -1000]
#takeprofit = [50, 50, 50, 100, 100, 100]

stoploss = [-99999999, -400, -600, -800, -1000]
takeprofit = [99999999, 400, 600, 800, 1000]

#threshold = [3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5]
threshold = [5.5, 6, 6.5, 7, 7.5]

for sl, tp in zip(stoploss, takeprofit):
    for th in threshold:
        os.system("python vds_sim.py -s %d -t %d -e %f -c SHFE:ru -d RUData -b VLDS_sim" % (sl, tp, th))
