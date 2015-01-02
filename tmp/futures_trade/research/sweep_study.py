#!/usr/bin/python

import os
import csv
import numpy as np
import matplotlib.pyplot as plt
import sys

bid_sweep = []
ask_sweep = []

#bid_colname = 'SweepStrong<Bid 1/1>'
#ask_colname = 'SweepStrong<Ask 1/1>'
bid_colname = 'SweepStrong<Bid 1/5>'
ask_colname = 'SweepStrong<Ask 1/5>'
mid_colname = 'MidPoint'
mid_2s_colname = 'MidPoint_2000ms'

#filenames = os.listdir('/home/fye/Documents/futures_trade/data/SHFE/')
filenames = os.listdir('/home/fye/Documents/futures_trade/data/CFFEX/')

#filenames = map(lambda t : os.path.join('/home/fye/Documents/futures_trade/data/SHFE', t), filter(lambda t : t.startswith('features'), filenames))
filenames = map(lambda t : os.path.join('/home/fye/Documents/futures_trade/data/CFFEX', t), filter(lambda t : t.startswith('features'), filenames))

for eachfile in filenames:
    with open(eachfile, 'r') as fd:
        csvreader = csv.reader(fd)
        header = csvreader.next()
        bid_idx = header.index(bid_colname)
        ask_idx = header.index(ask_colname)
        mid_idx = header.index(mid_colname)
        mid_2s_idx = header.index(mid_2s_colname)
        for row in csvreader:
            bid_sweep.append([int(row[bid_idx]), float(row[mid_2s_idx])-float(row[mid_idx])])
            ask_sweep.append([int(row[ask_idx]), float(row[mid_2s_idx])-float(row[mid_idx])])

bid_sweep = filter(lambda t : t[0] >= int(sys.argv[1]), bid_sweep)
ask_sweep = filter(lambda t : t[0] >= int(sys.argv[1]), ask_sweep)

print 'Total trading days: %s' % len(filenames)
print len(bid_sweep), len(bid_sweep) / len(filenames), np.mean(map(lambda x:x[0], bid_sweep)), np.std(map(lambda x:x[0], bid_sweep)), np.mean(map(lambda x:x[1], bid_sweep)), np.std(map(lambda x:x[1], bid_sweep)) 
print len(ask_sweep), len(ask_sweep) / len(filenames), np.mean(map(lambda x:x[0], ask_sweep)), np.std(map(lambda x:x[0], ask_sweep)), np.mean(map(lambda x:x[1], ask_sweep)), np.std(map(lambda x:x[1], ask_sweep))
