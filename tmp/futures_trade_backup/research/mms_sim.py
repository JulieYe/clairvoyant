import os
import optparse
from collections import defaultdict
import numpy as np
import glob
import pprint

pp = pprint.PrettyPrinter(indent=4)
commission_dict = {'IF' : 22,
                   'ru' : 13.8,
                   'rb' : 2.6,
                   'cu' : 17.3}

slippage_dict = {'IF' : 30,
                 'rb' : 0,
                 'ru' : 0,
                 'cu' : 0}

def getSimResult(filename):
    ff = open(filename, "r")
    header = None
    info_row = None
    for row in ff:
        if header is None:
            newrow = row.replace('\n', '')
            header = newrow.split(',')
        else:
            if row.startswith("StrategyStatus") :
                info_row = row
    newrow = info_row.replace('\n', '')
    result = newrow.split(',')
    return dict(zip(header, result))

def identifyFiles(folder, contractCode):
    files = glob.glob(os.path.join(folder, "????????.%s????"%(contractCode)))
    return files

def runFile(contractSpec_file, configSpec_file, filename, log_folder, stop_loss, take_profit, bandwidth, volthreshold, ordersize, exchange, latency_ms, binary_name):
    date = os.path.basename(filename).split(".")[0]
    date = date[:4]+'-'+date[4:6]+'-'+date[6:]
    contract = os.path.basename(filename).split(".")[1]
    logfile = os.path.join(log_folder, "mms_%s_%s.log" % (date, contract))
    cmd = "$ZZTRADERBIN/%s %s %s %s %s %s %s %s %f %f %f %d %d %f %f"%(
        binary_name,
        contractSpec_file, configSpec_file, filename, date, exchange, contract, log_folder, stop_loss, take_profit, 0.0, ordersize, latency_ms, volthreshold, bandwidth)
    print cmd
    os.system(cmd)
    result = getSimResult(logfile)
    result['date'] = date
    pp.pprint(result)
    return result

def runAll(data_folder, log_folder, contractSpec_file, configSpec_file, exchangeCode, contractCode, stop_loss, take_profit, bandwidth, volthreshold, ordersize, latency_ms, binary_name, alias):
    filenames = identifyFiles(data_folder, contractCode)
    filenames.sort()
    result = defaultdict(list)
    for filename in filenames:
        print "processing", filename
        tmp = runFile(contractSpec_file, configSpec_file, filename, log_folder, stop_loss, take_profit, bandwidth, volthreshold, ordersize, exchangeCode, latency_ms, binary_name)
        for key in tmp.keys():
            result[key].append(tmp[key])
        if alias == 'VVDS':
            result['RealPNL'].append(float(tmp['PNL']) - commission_dict[contractCode] * float(tmp['Volume']) - slippage_dict[contractCode] * float(tmp['CancelCount']) * ordersize * 2)
        elif alias == 'VUDS':
            result['RealPNL'].append(float(tmp['PNL']) - (commission_dict[contractCode] + slippage_dict[contractCode]) * float(tmp['Volume']))
        elif alias == 'MMS':
            result['RealPNL'].append(float(tmp['PNL']) - commission_dict[contractCode] * float(tmp['Volume']) - slippage_dict[contractCode] * float(tmp['MarketOrderCount']) * ordersize)
        else:
            result['RealPNL'].append(float(tmp['PNL']) - commission_dict[contractCode] * float(tmp['Volume']))
    del result['Name']
    del result['Time']
    del result['Ts']
    for key in result.keys():
        if key != "date":
            result[key] = [float(item) for item in result[key]]
    result['contract'] = contractCode
    result["stoploss"] = stop_loss
    result["takeprofit"] = take_profit
    result['bandwidth'] = bandwidth
    result['volthreshold'] = volthreshold
    result['latency_ms'] = latency_ms
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
                   'bandwidth' : result['bandwidth'],
                   'volthreshold' : result['volthreshold'],
                   'nday': nday,
                   'totalpnl' : totalpnl,
                   'totalvolume': totalvolume,
                   'meanpnl': meanpnl,
                   'ppv': ppv,
                   'sharpe': sharpe}
    return summaryline

def writeresult(result, output_folder, output_file):
    file1 = os.path.join(output_folder, "s%.2ft%.2fw%.2fv%.2f.csv"%(result['stoploss'], 
                                                                    result['takeprofit'],
                                                                    result['bandwidth'],
                                                                    result['volthreshold']))
    file2 = os.path.join(output_folder, output_file)
    f1 = open(file1, "w")
    f1.write(str(result))
    f1.close()
    summaryline = summarize(result)
    if os.path.exists(file2):
        f2 = open(file2, "a")
        row = [str(summaryline[key]) for key in ['contract', 'stoploss', 'takeprofit', 'bandwidth', 'volthreshold', 
                                                 'nday', 'totalpnl', 
                                                 'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        f2.close()
    else :
        f2 = open(file2, 'w')
        row = [str(key) for key in ['contract', 'stoploss', 'takeprofit', 'bandwidth', 'volthreshold', 
                                    'nday', 'totalpnl', 
                                    'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        row = [str(summaryline[key]) for key in ['contract', 'stoploss', 'takeprofit', 'bandwidth', 'volthreshold', 
                                                 'nday', 'totalpnl', 
                                                 'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        f2.close()
    print "wrote", file1, 'and', file2

if __name__ == "__main__":
    #data_folder = "/home/fye/Documents/futures_trade/data/CFFE"
    #output_folder = "/home/fye/Documents/futures_trade/data/output"
    #contractSpec_file = "/home/fye/Documents/futures_trade/config/contract.specs"

    data_folder = os.getenv("ZZDATAROOT") 
    output_folder = os.getenv("ZZDATAOUT") 
    contractSpec_file = os.path.join(os.getenv("ZZCONFIG"), "contract.specs")
    configSpec_file = os.path.join(os.getenv("ZZCONFIG"), "config.specs")

    parser = optparse.OptionParser()
    parser.add_option("-s", "--stop-loss", dest="stoploss", default=-120,
                      help="stop loss")
    parser.add_option("-t", "--take-profit", dest="takeprofit", default=120,
                      help="take profit")
    parser.add_option("-w", "--bandwidth", dest="bandwidth", default=300,
                      help="bandwidth")
    parser.add_option("-v", "--volthreshold", dest="volthreshold", default=0.2,
                      help="volthreshold")
    parser.add_option("-z", "--ordersize", dest="ordersize", default=1,
                      help="ordersize")
    parser.add_option("-c", "--contract-code", dest="contractCode", default="CFFEX:IF",
                      help="contractCode")
    parser.add_option("-d", "--data-dir", dest="dir", default="CFFEX",
                      help="basename of data dir")
    parser.add_option("-l", "--latency-ms", dest="latency_ms", default=0,
                      help="latency assumption for simulation")
    parser.add_option("-o", "--output-file", dest="output_file", default="summary.csv",
                      help="output summary file")
    parser.add_option("-b", "--binary-name", dest="binary_name", default="MMS_sim",
                      help="binary name")
    parser.add_option("-a", "--alias", dest="alias", default="MMS",
                      help="alias")

    (options, args) = parser.parse_args()
    options.stoploss = float(options.stoploss)
    options.takeprofit = float(options.takeprofit)
    options.bandwidth = float(options.bandwidth)
    options.volthreshold = float(options.volthreshold)
    options.ordersize = int(options.ordersize)
    options.latency_ms = int(options.latency_ms)
    exchange, contract = options.contractCode.split(':')

    data_folder = os.path.join(data_folder, options.dir)

    result = runAll(data_folder, output_folder, contractSpec_file, configSpec_file, exchange, contract, options.stoploss, options.takeprofit, options.bandwidth, options.volthreshold, options.ordersize, options.latency_ms, options.binary_name, options.alias)

    pp.pprint(summarize(result))
    writeresult(result, output_folder, options.output_file)
