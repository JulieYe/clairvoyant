contractCode = "CFFEX:IF"
dataDir = ""
binary = "SMS_sim"
alias = "SMS"
configFile = ""

threshold_list = [0.1 + 0.05 * ii for ii in range(20)]

for th in threshold_list:
    cmd = "python master_sim.py -h %(th)f -c %(contractCode)s -d %(dataDir)s -b %(binary)s -a %(alias)s --config %(configFile)s"%locals()
    print cmd
    os.system(cmd)


    
