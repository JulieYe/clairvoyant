import os

# stoploss = [-999999999, -999999999, -999999999, -999999999, -999999999, -999999999, -120, -180, -240, -300, -360, -120, -180, -240, -300, -360]
# takeprofit = [999999999, 120, 180, 240, 300, 360, 999999999, 999999999, 999999999, 999999999, 999999999, 120, 180, 240, 300, 360]
# threshold = [0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7]

stoploss = [-240, -300, -360, -420, -480]
takeprofit = [240, 300, 360, 420, 480]
threshold_lead = range(5, 11)
threshold_lead_trade_diff = range(10)
nsnapshots = [1]
leadid = [1,2]
binarys = ['LLS_sim'] #, "LLSR_sim"]

for sl, tp in zip(stoploss, takeprofit):
    for th_lead in threshold_lead:
        for th_diff in threshold_lead_trade_diff:
            th_trade = th_lead - th_diff
            if th_trade <= 0:
                continue
            for ns in nsnapshots:
                for li in leadid:
                    for br in binarys:
                        os.system("python lls_sim.py -s %d -t %d -e %d -v %d -c CFFEX:IF -d CFFEX -z 1 -r %d -b %s -o summary_lls_IF.csv -n %d" % (sl, tp, th_trade, th_lead, li, br, ns))
