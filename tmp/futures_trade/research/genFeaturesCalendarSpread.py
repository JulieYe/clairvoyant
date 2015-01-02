#!/usr/bin/python

import optparse
import datetime
import os
import sys
import copy

def process(asOfDate, contractSpec_file, exchange, ticker_head, dir_input, dir_output, splitter, feature_generator):
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
    ticker = tickers[volumes.index(max(volumes))]
    volumes_next_contract = copy.deepcopy(volumes)
    volumes_next_contract.remove(max(volumes))
    ticker_next_contract = tickers[volumes.index(max(volumes_next_contract))]
    print "Working calendar spread %s-%s on %s" % (ticker_next_contract, ticker, asOfDate)
    if os.path.exists('/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker)):
        print "%s.%s already exits, skipping..." % (asOfDate, ticker)
    else:
        print "python %s %s %s %s %s" % (splitter, dataFile_input, dir_output, asOfDate, ticker)
        os.system("python %s %s %s %s %s" % (splitter, dataFile_input, dir_output, asOfDate, ticker))
    if os.path.exists('/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker_next_contract)):
        print "%s.%s already exits, skipping..." % (asOfDate, ticker_next_contract)
    else:
        print "python %s %s %s %s %s" % (splitter, dataFile_input, dir_output, asOfDate, ticker_next_contract)
        os.system("python %s %s %s %s %s" % (splitter, dataFile_input, dir_output, asOfDate, ticker_next_contract))
    print "%s %s %s %s %s %s %s %s %s /%s/features.%s.%s.%s.csv" % (feature_generator, 
                                                                    '/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker), 
                                                                    '/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker_next_contract),
                                                                    contractSpec_file, 
                                                                    exchange, exchange, 
                                                                    ticker, ticker_next_contract, 
                                                                    asOfDate, 
                                                                    dir_output.strip('/'), asOfDate, ticker, ticker_next_contract)
    os.system("%s %s %s %s %s %s %s %s %s /%s/features.%s.%s.%s.csv" % (feature_generator, 
                                                                        '/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker), 
                                                                        '/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker_next_contract),
                                                                        contractSpec_file, 
                                                                        exchange, exchange, 
                                                                        ticker, ticker_next_contract, 
                                                                        asOfDate, 
                                                                        dir_output.strip('/'), asOfDate, ticker, ticker_next_contract))

if __name__ == '__main__':

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
    parser.add_option('-f', "--feature-generator", dest="feature_generator",
                      help="path to feature_generator (absolute path)")
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
    feature_generator = options.feature_generator

    start_date_datetime = datetime.datetime(year = int(start_date.split('-')[0]),
                                            month = int(start_date.split('-')[1]),
                                            day = int(start_date.split('-')[2]))
    stop_date_datetime = datetime.datetime(year = int(stop_date.split('-')[0]),
                                           month = int(stop_date.split('-')[1]),
                                           day = int(stop_date.split('-')[2]))
    curr_date_datetime = start_date_datetime

    contractSpec_file = os.path.join(os.getenv("ZZCONFIG"), "contract.specs")

    while curr_date_datetime <= stop_date_datetime:
        process(curr_date_datetime.strftime('%Y%m%d'), contractSpec_file, exchange, ticker_head, dir_input, dir_output, splitter, feature_generator)
        curr_date_datetime += datetime.timedelta(days = 1)
