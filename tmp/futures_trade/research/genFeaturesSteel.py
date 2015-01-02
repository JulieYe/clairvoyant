#!/usr/bin/python

import datetime
import os
import sys
import copy

def process(asOfDate, contractSpec_file, exchange, ticker_head, dir_input, dir_output, splitter, feature_generator):
    filenames = os.listdir(dir_input)
    filenames = filter(lambda x: x. startswith(asOfDate), filenames)
    volume_max, ticker = 0, None
    for filename in filenames:
        os.system("tail -n 1 %s | cut -d, -f3 > /tmp/volume" % os.path.join(dir_input, filename))
        #print open('/tmp/volume','r').readlines()
        volume = int(open('/tmp/volume','r').readlines()[0].strip('\n'))
        print filename, volume
        if volume > volume_max:
            volume_max = volume        
            ticker = filename.split('.')[1]
    if ticker is None:
        return
    print "Working %s on %s" % (ticker, asOfDate)
    print "%s %s %s %s %s %s /%s/features.%s.%s.csv" % (feature_generator, 
                                                        '/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker), 
                                                        contractSpec_file, exchange, ticker, asOfDate, 
                                                        dir_output.strip('/'), asOfDate, ticker)
    os.system("%s %s %s %s %s %s /%s/features.%s.%s.csv" % (feature_generator, 
                                                            '/%s/%s.%s' % (dir_output.strip('/'), asOfDate, ticker), 
                                                            contractSpec_file, exchange, ticker, asOfDate, 
                                                            dir_output.strip('/'), asOfDate, ticker))
    
if __name__ == '__main__':
    #if len(sys.argv) != 8:
    #    print "Incorrect number of arguments..."
    #    exit()

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
