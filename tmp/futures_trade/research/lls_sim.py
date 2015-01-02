import os
import optparse
from collections import defaultdict
import numpy as np
import glob
import pprint

pp = pprint.PrettyPrinter(indent=4)
#commission_IF = 25
commission_dict = {'IF' : 25,
                   'ru' : 13.8,
                   'rb' : 2.6,
                   'cu' : 17.3}

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

def identifyTradeFiles(folder, contractCode, tradeid = 2):
    files = glob.glob(os.path.join(folder, "????????.%d.%s????"%(tradeid, contractCode)))
    return files

def findLeadFile(folder, adate, contractCode, leadid = 1):
    files = glob.glob(os.path.join(folder, "%s.%d.%s????"%(adate, leadid, contractCode)))
    return files[0]
                      

def runFile(contractSpec_file, configSpec_file, filename_trade, filename_lead, log_folder, stop_loss, take_profit, threshold_trade, threshold_lead, ordersize, exchange, binary_name="LLS_sim", nsnapshots=1):
    date = os.path.basename(filename_trade).split(".")[0]
    date = date[:4]+'-'+date[4:6]+'-'+date[6:]
    contract_trade = os.path.basename(filename_trade).split(".")[2]
    contract_lead = os.path.basename(filename_lead).split(".")[2]
    logfile = os.path.join(log_folder, "lls_%s_%s.log" % (date, contract_trade))
    cmd = "$ZZTRADERBIN/%s %s %s %s %s %s %s %s %s %s %s %f %f %d %d %d %d"%(
        binary_name,
        contractSpec_file, configSpec_file, filename_trade, exchange, contract_trade, filename_lead, exchange, contract_lead, date, log_folder, stop_loss, take_profit, threshold_lead, threshold_trade, ordersize, nsnapshots)
    print cmd
    os.system(cmd)
    result = getSimResult(logfile)
    result['date'] = date
    pp.pprint(result)
    return result

def runAll(data_folder, log_folder, contractSpec_file, configSpec_file, exchangeCode, contractCode, stop_loss, take_profit, threshold_trade, threshold_lead, ordersize, binary_name="LLS_sim", lead_id = 1, nsnapshots=1):
    if lead_id == 1:
        trade_id = 2
    else:
        trade_id = 1
    filenames = identifyTradeFiles(data_folder, contractCode, tradeid=trade_id)
    filenames.sort()
    result = defaultdict(list)
    for filename in filenames:
        filename_lead = findLeadFile(data_folder, os.path.basename(filename).split(".")[0], contractCode, leadid=lead_id)
        print "processing", filename, filename_lead
        tmp = runFile(contractSpec_file, configSpec_file, filename, filename_lead, log_folder, stop_loss, take_profit, threshold_trade, threshold_lead, ordersize, exchangeCode, binary_name, nsnapshots)
        for key in tmp.keys():
            result[key].append(tmp[key])
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
    result['threshold_trade'] = threshold_trade
    result['threshold_lead'] = threshold_lead
    result['nsnapshots'] = nsnapshots
    result['lead_id'] = lead_id
    result['binary'] = binary_name
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
                   'threshold_trade': result['threshold_trade'],
                   'threshold_lead': result['threshold_lead'],
                   'lead_id': result['lead_id'],
                   'binary': result['binary'],
                   'nsnapshots': result['nsnapshots'],
                   'nday': nday,
                   'totalpnl' : totalpnl,
                   'totalvolume': totalvolume,
                   'meanpnl': meanpnl,
                   'ppv': ppv,
                   'sharpe': sharpe}
    return summaryline

def writeresult(result, output_folder, output_file):
    file1 = os.path.join(output_folder, "s%s.%.2ft%.2fh%.2fh%.2f.%d.%d.%s.csv"%(result['contract'],
                                                                                result['stoploss'], 
                                                                                result['takeprofit'],
                                                                                result['threshold_trade'],
                                                                                result['threshold_lead'],
                                                                                result['lead_id'],
                                                                                result['nsnapshots'],
                                                                                result['binary']))
    file2 = os.path.join(output_folder, output_file)
    f1 = open(file1, "w")
    f1.write(str(result))
    f1.close()
    summaryline = summarize(result)
    if os.path.exists(file2):
        f2 = open(file2, "a")
        row = [str(summaryline[key]) for key in ['contract', 'stoploss', 'takeprofit',
                                                 'threshold_trade', 'threshold_lead', 'lead_id', 'nsnapshots', 'binary', 'nday', 'totalpnl', 
                                                 'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        f2.close()
    else :
        f2 = open(file2, 'w')
        row = [str(key) for key in ['contract', 'stoploss', 'takeprofit',
                                    'threshold_trade', 'threshold_lead', 'lead_id', 'nsnapshots', 'binary', 'nday', 'totalpnl', 
                                    'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        row = [str(summaryline[key]) for key in ['contract', 'stoploss', 'takeprofit',
                                                 'threshold_trade', 'threshold_lead', 'lead_id', 'nsnapshots', 'binary', 'nday', 'totalpnl', 
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
    parser.add_option("-e", "--threshold-trade", dest="threshold_trade", default=10,
                      help="threshold trade")
    parser.add_option("-v", "--threshold-lead", dest="threshold_lead", default=10,
                      help="threshold lead")
    parser.add_option("-z", "--ordersize", dest="ordersize", default=1,
                      help="ordersize")
    parser.add_option("-n", "--nsnapshots", dest="nsnapshots", default=1,
                      help="nsnapshots")
    parser.add_option("-c", "--contract-code", dest="contractCode", default="CFFEX:IF",
                      help="contractCode")
    parser.add_option("-d", "--data-dir", dest="dir", default="CFFEX",
                      help="basename of data dir")
    parser.add_option("-o", "--output-file", dest="output_file", default="summary.csv",
                      help="output summary file")
    parser.add_option("-b", "--binary-name", dest="binary_name", default="LLS_sim",
                      help="binary name")
    parser.add_option("-r", "--lead-id", dest="lead_id", default=1,
                      help="lead id")


    (options, args) = parser.parse_args()
    options.stoploss = float(options.stoploss)
    options.takeprofit = float(options.takeprofit)
    options.threshold_trade = int(options.threshold_trade)
    options.threshold_lead = int(options.threshold_lead)
    options.ordersize = int(options.ordersize)
    options.lead_id = int(options.lead_id)
    options.nsnapshots = int(options.nsnapshots)
    exchange, contract = options.contractCode.split(':')

    data_folder = os.path.join(data_folder, options.dir)

    result = runAll(data_folder, output_folder, contractSpec_file, configSpec_file, exchange, contract, options.stoploss, options.takeprofit, options.threshold_trade, options.threshold_lead, options.ordersize, options.binary_name,
                    options.lead_id, options.nsnapshots)

    pp.pprint(summarize(result))
    writeresult(result, output_folder, options.output_file)
