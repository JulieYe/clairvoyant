#!/usr/bin/python

import datetime
import os
import sys
import numpy as np

def process(asOfDate, contractSpec_file, exchange, ticker_head, dir_input, dir_output, splitter):
    dataFile_input = os.path.join(dir_input, '%s.data' % (asOfDate))
    if not os.path.exists(dataFile_input):
        return
    os.system("egrep '\[%s' %s | cut -c15-20 | sort | uniq > /tmp/tickers" % (ticker_head, dataFile_input))
    tickers = map(lambda x: x.strip('\n'), open('/tmp/tickers','r').readlines())
    volumes = []
    for ticker in tickers:
        os.system("grep '%s' %s | tail -n 1 | cut -d, -f5 | sed 's/ Volume=\[//' | sed 's/\]//' > /tmp/volume" % (ticker, dataFile_input))
        volumes.append(int(open('/tmp/volume','r').readlines()[0].strip('\n')))
    if len(volumes) == 0:
        return
    volume_idx = np.argsort(volumes)
    ticker_1 = tickers[volume_idx[-1]]
    ticker_2 = tickers[volume_idx[-2]]
    print "Working %s on %s" % (ticker_1, asOfDate)
    print "python %s %s %s %s %s %s" % (splitter, dataFile_input, dir_output, asOfDate, ticker_1, ticker_2)
    os.system("python %s %s %s %s %s %s" % (splitter, dataFile_input, dir_output, asOfDate, ticker_1, ticker_2))
    os.system("mv %s.%s %s.1.%s" %(os.path.join(dir_output, asOfDate), ticker_1,
                                   os.path.join(dir_output, asOfDate), ticker_1))
    os.system("mv %s.%s %s.2.%s" %(os.path.join(dir_output, asOfDate), ticker_2,
                                   os.path.join(dir_output, asOfDate), ticker_2))

    
if __name__ == '__main__':
    import optparse
    parser = optparse.OptionParser();
    parser.add_option('-x', "--exchange", dest="exchange", 
                      help="exchange code")
    parser.add_option('-t', "--ticker-head", dest="ticker_head", 
                      help="ticker head")
    parser.add_option('-b', "--begin-date", dest="begin_date",
                      help="begin date YYYY-MM-DD")
    parser.add_option('-e', "--end-date", dest="end_date",
                      help="end date YYYY-MM-DD")
    parser.add_option('-s', "--splitter", dest="splitter",
                      help="path to splitter (absolute path)")
    parser.add_option('-i', "--input-folder", dest="input_folder",
                      help="path to original data (absolute path)")
    parser.add_option('-o', "--output-folder", dest="output_folder",
                      help="path to output feature data (absolute path)")

    options, args = parser.parse_args()

    exchange = options.exchange
    ticker_head = options.ticker_head
    start_date = options.begin_date
    stop_date = options.end_date
    dir_input = options.input_folder
    dir_output = options.output_folder
    splitter = options.splitter

    start_date_datetime = datetime.datetime(year = int(start_date.split('-')[0]),
                                            month = int(start_date.split('-')[1]),
                                            day = int(start_date.split('-')[2]))
    stop_date_datetime = datetime.datetime(year = int(stop_date.split('-')[0]),
                                           month = int(stop_date.split('-')[1]),
                                           day = int(stop_date.split('-')[2]))
    curr_date_datetime = start_date_datetime

    contractSpec_file = os.path.join(os.getenv("ZZCONFIG"), "contract.specs")

    while curr_date_datetime <= stop_date_datetime:
        process(curr_date_datetime.strftime('%Y%m%d'), contractSpec_file, exchange, ticker_head, dir_input, dir_output, splitter)
        curr_date_datetime += datetime.timedelta(days = 1)
        
