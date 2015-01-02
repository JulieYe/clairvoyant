#!/usr/bin/python

'''
python splitter.py input_file output_dir 20121228
python splitter.py input_file output_dir 20121228 IF1301 IF1302 ...
'''

import os, sys, re

file_name = sys.argv[1]
output_dir = sys.argv[2]
contracts = []
for i in range(4, len(sys.argv)):
    contracts.append(sys.argv[i])

# Load raw data file.
with open(file_name,'r') as fd:
    content = fd.readlines()

# Load contract symbols from raw data file only if no contract symbols.
if len(contracts) == 0:
    for line in content:
        m = re.match("InstrumentID=\[(.*)\], UpdateTime=(.*)", line)
        if not m.group(1) in contracts:
            contracts.append(m.group(1))

# Split into data files by contract symbols
fds = []
for contract in contracts:
    fds.append(open(os.path.join(output_dir, "%s.%s" % (sys.argv[3], contract)), 'w'))
    fds[-1].write("date_time,last_px,total_volume,bid1_px,bid1_qty,bid2_px,bid2_qty,bid3_px,bid3_qty,bid4_px,bid4_qty,bid5_px,bid5_qty,ask1_px,ask1_qty,ask2_px,ask2_qty,ask3_px,ask3_qty,ask4_px,ask4_qty,ask5_px,ask5_qty\n")

for line in content:
    # Assume file name starts with date YYYYMMDD and is a format of YYYYMMDD.data
    if int(file_name.split('.')[0]) <= 20131030:
        m = re.match("InstrumentID=\[(.*)\], UpdateTime=\[(.*)\], UpdateMillisec=\[(.*)\], LastPrice=\[(.*)\], Volume=\[(.*)\], BidPrice1=\[(.*)\], BidVolume1=\[(.*)\], AskPrice1=\[(.*)\], AskVolume1=\[(.*)\]\n", line)
    else:
        m = re.match("InstrumentID=\[(.*)\], UpdateTime=\[(.*)\], UpdateMillisec=\[(.*)\], LastPrice=\[(.*)\], Volume=\[(.*)\], BidPrice1=\[(.*)\], BidVolume1=\[(.*)\], AskPrice1=\[(.*)\], AskVolume1=\[(.*)\], Epoch=\[(.*)\](.*)\n", line)
    try:
        idx = contracts.index(m.group(1))
        fds[idx].write("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" % (sys.argv[3][:4]+'-'+sys.argv[3][4:6]+'-'+sys.argv[3][6:]+'T'+m.group(2)+'.'+m.group(3), m.group(4), m.group(5), m.group(6), m.group(7), 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, m.group(8), m.group(9), 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0))
    except ValueError:
        pass
    
for fd in fds:
    fd.close()
