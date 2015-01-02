import os

# stoploss = [-999999999, -999999999, -999999999, -999999999, -999999999, -999999999, -120, -180, -240, -300, -360, -120, -180, -240, -300, -360]
# takeprofit = [999999999, 120, 180, 240, 300, 360, 999999999, 999999999, 999999999, 999999999, 999999999, 120, 180, 240, 300, 360]
# threshold = [0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7]

stoploss = [-150, -250, -500, -800, -1000]
takeprofit = [150, 250, 500, 800, 1000]
threshold = [3, 5, 8, 10, 15, 20]
nsnapshots = [1,2,3,4,5]
leadid = [1,2]
binarys = ['LLS_sim'] #, "LLSR_sim"]

for sl, tp in zip(stoploss, takeprofit):
    for th in threshold:
        for ns in nsnapshots:
            for li in leadid:
                for br in binarys:
                    os.system("python lls_sim.py -s %d -t %d -e %d -v %d -c SHFE:ru -d RULLS -z 1 -r %d -b %s -o ru_summary.csv -n %d" % (sl, tp, th, th, li, br, ns))
