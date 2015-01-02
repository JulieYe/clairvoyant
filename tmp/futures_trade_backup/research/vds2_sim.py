import os
import optparse
from collections import defaultdict
import numpy as np
import glob
import pprint

pp = pprint.PrettyPrinter(indent=4)
commission_IF = 25

def getSimResult(filename):
    ff = open(filename, "r")
    header = None
    for row in ff:
        if header is None:
            newrow = row.replace('\n', '')
            header = newrow.split(',')
        else:
            pass
    newrow = row.replace('\n', '')
    result = newrow.split(',')
    return dict(zip(header, result))

def identifyFiles(folder, contractCode):
    files = glob.glob(os.path.join(folder, "????????.%s????"%(contractCode)))
    return files

def runFile(filename, log_folder, stop_loss, take_profit, threshold):
    date = os.path.basename(filename).split(".")[0]
    date = date[:4]+'-'+date[4:6]+'-'+date[6:]
    contract = os.path.basename(filename).split(".")[1]
    logfile = os.path.join(log_folder, "vds2_%s.log" % date)
    cmd = "/home/fye/Documents/futures_trade/src/VDS_fei %s %s %s %s %f %f %f"%(
        filename, date, contract, log_folder, stop_loss, take_profit, threshold)
    print cmd
    os.system(cmd)
    result = getSimResult(logfile)
    result['date'] = date
    pp.pprint(result)
    return result

def runAll(data_folder, log_folder, contractCode, stop_loss, take_profit, threshold):
    filenames = identifyFiles(data_folder, contractCode)
    filenames.sort()
    result = defaultdict(list)
    for filename in filenames:
        print "processing", filename
        tmp = runFile(filename, log_folder, stop_loss, take_profit, threshold)
        for key in tmp.keys():
            result[key].append(tmp[key])
        result['RealPNL'].append(float(tmp['PNL']) - commission_IF * float(tmp['Volume']))
    del result['Name']
    del result['Time']
    del result['Ts']
    for key in result.keys():
        if key != "date":
            result[key] = [float(item) for item in result[key]]
    result['contract'] = contractCode
    result["stoploss"] = stop_loss
    result["takeprofit"] = take_profit
    result['threshold'] = threshold
    return result

def summarize(result):
    nday = len(result['RealPNL'])
    totalpnl = np.sum(result['RealPNL'])
    totalvolume = np.sum(result['Volume'])
    meanpnl = np.mean(result['RealPNL'])
    ppv = totalpnl / totalvolume
    sharpe = np.mean(result['RealPNL']) / np.std(result['RealPNL'])
    summaryline = {'contract' : result['contract'],
                   'stoploss' : result['stoploss'],
                   'takeprofit' : result['takeprofit'],
                   'threshold': result['threshold'],
                   'nday': nday,
                   'totalpnl' : totalpnl,
                   'totalvolume': totalvolume,
                   'meanpnl': meanpnl,
                   'ppv': ppv,
                   'sharpe': sharpe}
    return summaryline

def writeresult(result, output_folder):
    file1 = os.path.join(output_folder, "s%.2ft%.2fh%.2f.csv"%(result['stoploss'], 
                                                               result['takeprofit'],
                                                               result['threshold']))
    file2 = os.path.join(output_folder, "summary.csv")
    f1 = open(file1, "w")
    f1.write(str(result))
    f1.close()
    summaryline = summarize(result)
    if os.path.exists(file2):
        f2 = open(file2, "a")
        row = [str(summaryline[key]) for key in ['contract', 'stoploss', 'takeprofit',
                                                 'threshold', 'nday', 'totalpnl', 
                                                 'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        f2.close()
    else :
        f2 = open(file2, 'w')
        row = [str(key) for key in ['contract', 'stoploss', 'takeprofit',
                                    'threshold', 'nday', 'totalpnl', 
                                    'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        row = [str(summaryline[key]) for key in ['contract', 'stoploss', 'takeprofit',
                                                 'threshold', 'nday', 'totalpnl', 
                                                 'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        f2.close()
    print "wrote", file1, 'and', file2

if __name__ == "__main__":
    data_folder = "/home/fye/Documents/futures_trade/data/CFFE"
    output_folder = "/home/fye/Documents/futures_trade/data/output"

    parser = optparse.OptionParser()
    parser.add_option("-s", "--stop-loss", dest="stoploss", default=-120,
                      help="stop loss")
    parser.add_option("-t", "--take-profit", dest="takeprofit", default=120,
                      help="take profit")
    parser.add_option("-e", "--threshold", dest="threshold", default=0.28,
                      help="threshold")
    parser.add_option("-c", "--contract-code", dest="contractCode", default="IF",
                      help="contractCode")

    (options, args) = parser.parse_args()
    options.stoploss = float(options.stoploss)
    options.takeprofit = float(options.takeprofit)
    options.threshold = float(options.threshold)

    result = runAll(data_folder, output_folder, options.contractCode, options.stoploss, options.takeprofit, options.threshold)

    pp.pprint(summarize(result))
    writeresult(result, output_folder)
