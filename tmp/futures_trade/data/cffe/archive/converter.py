#!/usr/bin/python

import sys
import os

def doWork():
    rawfiles = filter(lambda x: x.endswith('csv'), os.listdir('.'))
    for rawfile in rawfiles:
        with open(rawfile, 'r') as fd_in:
            data = fd_in.readlines()
            asOfDate = rawfile.split('_')[-1].split('.')[0]
            asOfDate = asOfDate[:4] + '-' + asOfDate[4:6] + '-' + asOfDate[6:]
            filename_output = rawfile.split('_')[-1].split('.')[0] + '.IFXXXX'
            with open(filename_output, 'w') as fd_out:
                fd_out.write("date_time,last_px,total_volume,bid1_px,bid1_qty,bid2_px,bid2_qty,bid3_px,bid3_qty,bid4_px,bid4_qty,bid5_px,bid5_qty,ask1_px,ask1_qty,ask2_px,ask2_qty,ask3_px,ask3_qty,ask4_px,ask4_qty,ask5_px,ask5_qty\n")
                tradeVolume = 0
                for i, eachline in enumerate(data):
                    fields = eachline.strip().split(',')
                    if i == 0:
                        next_fields = data[i+1].strip().split(',')
                        if next_fields[0] != fields[0]:
                            datetime = "%sT%s.250" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                        else:
                            datetime = "%sT%s.000" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                    elif i == (len(data)-1):
                        prev_fields = data[i-1].strip().split(',')
                        if prev_fields[0] != fields[0]:
                            datetime = "%sT%s.250" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                        else:
                            datetime = "%sT%s.500" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                    else:
                        prev_fields = data[i-1].strip().split(',')
                        next_fields = data[i+1].strip().split(',')
                        if prev_fields[0] == fields[0]:
                            datetime = "%sT%s.500" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                        elif next_fields[0] == fields[0]:
                            datetime = "%sT%s.000" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                        else:
                            datetime = "%sT%s.250" % (asOfDate, '0' + fields[0] if len(fields[0]) == 7 else fields[0])
                    tradeVolume += int(fields[4])
                    fd_out.write("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" % (datetime, fields[1], tradeVolume, fields[2], fields[5], 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, fields[3], fields[6], 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0))

if __name__ == "__main__":
    doWork()
