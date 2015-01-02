import os
import optparse
from collections import defaultdict
import numpy as np
import glob
import pprint

pp = pprint.PrettyPrinter(indent=4)
commission_dict = {'IF' : 20,
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

def identifyPairs(folder, contractCode):
    files = glob.glob(os.path.join(folder, "????????.%s????"%(contractCode)))
    ret = []
    for each in files:
        asOfDate = os.path.basename(each).split(".")[0]
        pairs = filter(lambda x : os.path.basename(x).split('.')[0] == asOfDate, files)
        volumes = []
        for eachfile in pairs:
            os.system("tail -n 1 %s | cut -d, -f3 > /tmp/volume" % eachfile)
            # print "tail -n 1 %s | cut -d, -f3 > /tmp/volume" % eachfile
            volumes.append(int(open('/tmp/volume','r').readlines()[0].strip('\n')))
        if volumes[0] < volumes[1]:
            pairs.reverse()
        if not pairs in ret:
            ret.append(pairs)
    return ret

def runFile(contractSpec_file, config_folder, filename, log_folder, edge, bufferr, take_profit, take_profit_spread, stop_loss, stop_loss_spread, max_spread_position, max_cancel_qty, ordersize, exchange, binary_name="SMMS_sim"):
    date = os.path.basename(filename[0]).split(".")[0]
    date = date[:4]+'-'+date[4:6]+'-'+date[6:]
    contract_front = os.path.basename(filename[0]).split(".")[1]
    contract_back = os.path.basename(filename[1]).split(".")[1]
    logfile = os.path.join(log_folder, "smms_%s_%s-%s.log" % (date, contract_back, contract_front))
    cmd = "$ZZTRADERBIN/%s %s %s %s %s %s %s %s %s %s %s %f %f %f %f %f %f %d %d %d" % (binary_name, config_folder, contractSpec_file, filename[0], exchange, contract_front, filename[1], exchange, contract_back, date, log_folder, edge, bufferr, take_profit, take_profit_spread, stop_loss, stop_loss_spread, max_spread_position, max_cancel_qty, ordersize)
    print cmd
    os.system(cmd)
    result = getSimResult(logfile)
    result['date'] = date
    pp.pprint(result)
    return result

def runAll(data_folder, log_folder, config_folder, contractSpec_file, exchangeCode, contractCode, edge, bufferr, take_profit, take_profit_spread, stop_loss, stop_loss_spread, max_spread_position, max_cancel_qty, ordersize, binary_name="SMMS_sim"):
    filenames = identifyPairs(data_folder, contractCode)
    filenames.sort()
    result = defaultdict(list)
    for filename in filenames:
        print "processing", map(lambda f : os.path.basename(f), filename)
        tmp = runFile(contractSpec_file, config_folder, filename, log_folder, edge, bufferr, take_profit, take_profit_spread, stop_loss, stop_loss_spread, max_spread_position, max_cancel_qty , ordersize, exchangeCode, binary_name)
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
    result['edge'] = edge
    result['buffer'] = bufferr
    result["takeprofit"] = take_profit
    result["takeprofitspread"] = take_profit_spread
    result["stoploss"] = stop_loss
    result["stoplossspread"] = stop_loss_spread
    result['max_spread_position'] = max_spread_position
    result['max_cancel_qty'] = max_cancel_qty
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
                   'edge' : result['edge'], 
                   'buffer' : result['buffer'], 
                   'takeprofit' : result['takeprofit'],
                   'takeprofitspread' : result['takeprofitspread'],
                   'stoploss' : result['stoploss'],
                   'stoplossspread' : result['stoplossspread'],
                   'max_spread_position': result['max_spread_position'],
                   'max_cancel_qty': result['max_cancel_qty'],
                   'binary': result['binary'],
                   'nday': nday,
                   'totalpnl' : totalpnl,
                   'totalvolume': totalvolume,
                   'meanpnl': meanpnl,
                   'ppv': ppv,
                   'sharpe': sharpe}
    return summaryline

def writeresult(result, output_folder, output_file):
    file1 = os.path.join(output_folder, "e%.2fb%.2ft%.2fT%.2fs%.2fS%.2fp%dq%d.csv" % (result['edge'], 
                                                                                      result['buffer'], 
                                                                                      result['takeprofit'],
                                                                                      result['takeprofitspread'],
                                                                                      result['stoploss'], 
                                                                                      result['stoplossspread'], 
                                                                                      result['max_spread_position'],
                                                                                      result['max_cancel_qty']))
    file2 = os.path.join(output_folder, output_file)
    f1 = open(file1, "w")
    f1.write(str(result))
    f1.close()
    summaryline = summarize(result)
    if os.path.exists(file2):
        f2 = open(file2, "a")
        row = [str(summaryline[key]) for key in ['contract', 'edge', 'buffer', 'takeprofit', 'takeprofitspread', 'stoploss', 'stoplossspread', 
                                                 'max_spread_position', 'max_cancel_qty', 'binary', 'nday', 'totalpnl', 
                                                 'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        f2.close()
    else :
        f2 = open(file2, 'w')
        row = [str(key) for key in ['contract', 'edge', 'buffer', 'takeprofit', 'takeprofitspread', 'stoploss', 'stoplossspread', 
                                    'max_spread_position', 'max_cancel_qty', 'binary', 'nday', 'totalpnl', 
                                    'totalvolume', 'meanpnl', 'ppv', 'sharpe']]
        f2.write(",".join(row)+"\n")
        row = [str(summaryline[key]) for key in ['contract', 'edge', 'buffer', 'takeprofit', 'takeprofitspread', 'stoploss', 'stoplossspread', 
                                                 'max_spread_position', 'max_cancel_qty', 'binary', 'nday', 'totalpnl', 
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
    config_folder = os.getenv("ZZCONFIG")
    contractSpec_file = os.path.join(config_folder, "contract.specs")
    #configSpec_file = os.path.join(os.getenv("ZZCONFIG"), "config.specs")

    parser = optparse.OptionParser()
    parser.add_option("-e", "--edge", dest="edge", default=0.4,
                      help="edge")
    parser.add_option("-f", "--buffer", dest="buffer", default=0.4,
                      help="buffer")
    parser.add_option("-t", "--take-profit", dest="takeprofit", default=120,
                      help="take profit")
    parser.add_option("-T", "--take-profit-spread", dest="takeprofitspread", default=120,
                      help="take profit spread")
    parser.add_option("-s", "--stop-loss", dest="stoploss", default=-240,
                      help="stop loss")
    parser.add_option("-S", "--stop-loss-spread", dest="stoplossspread", default=-240,
                      help="stop loss spread")
    parser.add_option("-p", "--max-spread-position", dest="maxspreadposition", default=5,
                      help="max spread position")
    parser.add_option("-q", "--max-cancel-qty", dest="maxcancelqty", default=9999999999,
                      help="max cancel qty")
    parser.add_option("-z", "--ordersize", dest="ordersize", default=1,
                      help="ordersize")
    parser.add_option("-c", "--contract-code", dest="contractCode", default="CFFEX:IF",
                      help="contractCode")
    parser.add_option("-d", "--data-dir", dest="dir", default="CFFEX",
                      help="basename of data dir")
    parser.add_option("-o", "--output-file", dest="output_file", default="summary.csv",
                      help="output summary file")
    parser.add_option("-b", "--binary-name", dest="binary_name", default="LLS_sim",
                      help="binary name")

    (options, args) = parser.parse_args()
    options.edge = float(options.edge)
    options.buffer = float(options.buffer)
    options.takeprofit = float(options.takeprofit)
    options.takeprofitspread = float(options.takeprofitspread)
    options.stoploss = float(options.stoploss)
    options.stoplossspread = float(options.stoplossspread)
    options.maxspreadposition = int(options.maxspreadposition)
    options.maxcancelqty = int(options.maxcancelqty)
    options.ordersize = int(options.ordersize)
    exchange, contract = options.contractCode.split(':')

    data_folder = os.path.join(data_folder, options.dir)

    result = runAll(data_folder, output_folder, config_folder, contractSpec_file, exchange, contract, options.edge, options.buffer, options.takeprofit, options.takeprofitspread, options.stoploss, options.stoplossspread, options.maxspreadposition, options.maxcancelqty, options.ordersize, options.binary_name)

    pp.pprint(summarize(result))
    writeresult(result, output_folder, options.output_file)
