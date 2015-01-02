import os
import optparse
from collections import defaultdict
import numpy as np
import glob
import pprint
from ConfigParser import ConfigParser

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

def runVVDSM2(baseConfigFile, filename, paramDict):
    date = os.path.basename(filename).split(".")[0]
    date = date[:4]+'-'+date[4:6]+'-'+date[6:]
    contract = os.path.basename(filename).split(".")[1]
    config = ConfigParser()
    config.read(baseConfigFile)
    config.set("Main", "Date", date)
    config.set("CFFEXAlt_IF1", "Contract", contract)
    config.set("CFFEXAlt_IF1", "DataFile", filename)
    config.set("VVDSM2_IF", "Contract", contract)
    config.set("VVDSM2_IF", "StopLoss", paramDict["StopLoss"])
    config.set("VVDSM2_IF", "TakeProfit", paramDict["TakeProfit"])
    config.set("VVDSM2_IF", "Threshold", paramDict['Threshold'])
    config.set("VVDSM2_IF", "OrderSize", paramDict["OrderSize"])
    config.set("VVDSM2_IF", "ThresholdVol", paramDict["ThresholdVol"])
    config.set("VVDSM2_IF", "EntryEdge", paramDict['EntryEdge'])
    config.set("VVDSM2_IF", "ExitEdge", paramDict['ExitEdge'])
    cfgFile = os.path.join(config.get("VVDSM2_IF", "LoggerOutputFolder"), date + "." + contract + ".cfg")
    config.write(cfgFile)
    cmd = "$ZZTRADEBIN/SimMain %s"%cfgFile
    print cmd
    os.system(cmd)
    logfile = os.path.join(config.get("VVDSM2_IF", "LoggerOutputFolder"), "sim_"+date+"_VVDSM2.log")
    result = getSimResult(logfile)
    result['date'] = date
    pp.pprint(result)
    return result

paramKey_dict = {
    #'MMS': ['take_profit', 'stop_loss', 'bandwidth', 'volthreshold', 'order_size'],
    #'VVDSM': ['take_profit', 'stop_loss', 'threshold', 'volthreshold', 'order_size'],
    'VVDSM2': ['TakeProfit', 'StopLoss', 'Threshold', 'ThresholdVol', 'OrderSize', 'EntryEdge', 'ExitEdge']
    #'VUDS': ['take_profit', 'stop_loss', 'threshold', 'volthreshold', 'order_size'],
}

runFile_dict = {
    'VVDSM2': runVVDSM2
    #'MMS': runFileMMS,
    #'VVDSM': runFileVDS,
    #'VUDSM': runFileVDS
}




def runAll(data_folder, baseConfig, paramDict, alias):
    filenames = identifyFiles(data_folder, contractCode)
    filenames.sort()
    result = defaultdict(list)
    for filename in filenames:
        print "processing", filename
        tmp = runFile_dict[alias](baseConfig, filename, paramDict)
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
    result['param'] = {}
    for key in paramKey_dict[alias]:
        result['param'][key] = params[key]
    return result

def summarize(result):
    nday = len(result['RealPNL'])
    totalpnl = np.sum(result['RealPNL'])
    totalvolume = np.sum(result['Volume'])
    meanpnl = np.mean(result['RealPNL'])
    ppv = totalpnl / totalvolume
    sharpe = np.mean(result['RealPNL']) / np.std(result['RealPNL'])
    summaryline = {'contract' : result['contract'],
                   'nday': nday,
                   'totalpnl' : totalpnl,
                   'totalvolume': totalvolume,
                   'meanpnl': meanpnl,
                   'ppv': ppv,
                   'sharpe': sharpe}
    for key in result['param'].keys():
        summaryline[key] = result['param'][key]
    return summaryline

def writeresult(result, output_folder, output_file):
    file2 = os.path.join(output_folder, output_file)
    summaryline = summarize(result)
    param_key = result['param'].keys()
    param_key = sort(param_key)
    summary_key = ['contract'] + param_key + ['nday', 'totalpnl', 'totalvolume', 'meanpnl', 'ppv', 'sharpe']
    if os.path.exists(file2):
        f2 = open(file2, "a")
        row = [str(summaryline[key]) for key in summary_key]
        f2.write(",".join(row)+"\n")
        f2.close()
    else :
        f2 = open(file2, 'w')
        row = [str(key) for key in summary_key]
        f2.write(",".join(row)+"\n")
        row = [str(summaryline[key]) for key in summary_key]
        f2.write(",".join(row)+"\n")
        f2.close()
    print "wrote", file2

if __name__ == "__main__":
    data_folder = os.getenv("ZZDATAROOT") 
    output_folder = os.getenv("ZZDATAOUT") 

    parser = optparse.OptionParser()
    parser.add_option("-d", "--data-dir", dest="dir", 
                      help="basename of data dir")
    parser.add_option("-o", "--output-file", dest="output_file", 
                      help="output summary file")
    parser.add_option("-a", "--alias", dest="alias", default="VVDSM2",
                      help="alias")
    parser.add_option("--config", dest="config", 
                      help="base config file")
    parser.add_option("--param-list", dest="param_list",
                      help="param Key:value,Key2:value")

    (options, args) = parser.parse_args()

    data_folder = os.path.join(data_folder, options.dir)
    
    paramDict = dict([tuple(item.split(":")) for item in options.param_list.split(",")])
    
    result = runAll(data_folder, options.config, paramDict, options.alias)
    pp.pprint(summarize(result))
    writeresult(result, output_folder, options.output_file)
