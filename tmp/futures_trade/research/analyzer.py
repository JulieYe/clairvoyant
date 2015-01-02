#!/usr/bin/python

import csv
import os
import optparse

def process(directory, version):
    total_pnl = 0.0
    print 'date, volume, min, max, max_drawdown, last'
    filenames = os.listdir(directory)
    filenames = filter(lambda x: x.startswith('vds_') and x.endswith('log'), filenames)
    filenames.sort()
    for filename in filenames:
        pnl, qty = [], []
        max_drawdown = 0
        path_drawdown =  0
        with open(os.path.join(directory, filename), 'r') as fd:
            csv_reader = csv.reader(fd, delimiter=',')
            csv_reader.next()
            for row in csv_reader:
                if len(row) == 14 and int(row[6]) > 0 and int(row[7]) == 0:
                    qty = int(row[6])
                    qty_mkt = int(row[-2])
                    qty_cancel = int(row[-1])
                    if version == 'VVDS':
                        pnl.append(float(row[3]) - 0.1 * 300 * 2 * qty_cancel - 24 * qty)
                    elif version == 'VUDS':
                        pnl.append(float(row[3]) - 0.1 * 300 * qty_mkt  - 24 * qty)
                    if len(pnl) > 1:
                        if pnl[-1] >= pnl[-2]:
                            max_drawdown = min(max_drawdown, path_drawdown)
                            path_drawdown = 0
                        else:
                            path_drawdown += (pnl[-1]-pnl[-2])
        if len(pnl) > 0:
            total_pnl += pnl[-1]
            print '%s, %s, %s, %s, %s, %s' % (filename.split('_')[1], qty, min(pnl), max(pnl), max_drawdown, pnl[-1])
    print 'total pnl: %s' % total_pnl

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-d", "--directory", dest='directory', default='.', help='data directory')
    parser.add_option("-v", "--version", dest='version', default='.', help='version of strategy')
    (options, args) = parser.parse_args()
    process(options.directory, options.version)
