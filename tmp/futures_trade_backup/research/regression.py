#!/usr/bin/python

import numpy
import scipy
import sys
import csv
import matplotlib.pyplot as plt

def CalcR2(y, yhat, ybar):
    SST = sum(map(lambda x: (x-ybar)**2, y))
    SSE = sum(map(lambda x: (x[0]-x[1])**2, zip(y,yhat)))
    if SST > 0:
        return 1.0 - float(SSE) / float(SST)
    else:
        return None

dates = []
data_X = []
data_Y = []
with open(sys.argv[1], 'r') as fd:
    reader = csv.reader(fd, delimiter=',')
    header = reader.next()
    for row in reader:
        if True:
            dates.append(row[1])
            mid = float(row[2])
            mid_2s = float(row[3])
            mid_5s = float(row[4])
            mid_10s = float(row[5])
            mid_20s = float(row[6])
            mid_30s = float(row[7])
            mid_40s = float(row[8])
            mid_50s = float(row[9])
            mid_60s = float(row[10])
            data_Y.append([(mid_2s-mid)/mid, (mid_5s-mid)/mid, (mid_10s-mid)/mid, (mid_20s-mid)/mid, (mid_30s-mid)/mid, (mid_40s-mid)/mid, (mid_50s-mid)/mid, (mid_60s-mid)/mid])
            data_X.append(map(lambda x: float(x), [row[11], row[12], row[22], row[24], row[39], row[45]]))

    X = numpy.array(data_X)
    Y = numpy.array(data_Y)
    w = numpy.linalg.lstsq(X, Y)
    Yhat = numpy.dot(X, w[0])
    Ybar = numpy.apply_along_axis(numpy.average, 0, Y)
    print 'All-sample (%s):' % len(data_X)
    print numpy.apply_along_axis(numpy.std, 0, Y)
    print w[0]
    r2 = []
    for i in range(Y.shape[1]):
        r2.append(round(CalcR2(Y[numpy.arange(len(data_X)), i].tolist(), Yhat[numpy.arange(len(data_X)), i].tolist(), Ybar[i]), 4))
    print r2

    ss_in = int(len(data_X) * 0.7)
    X_in = numpy.array(data_X[:ss_in])
    Y_in = numpy.array(data_Y[:ss_in])
    w = numpy.linalg.lstsq(X_in, Y_in)
    Yhat_in = numpy.dot(X_in, w[0]) 
    Ybar_in = numpy.apply_along_axis(numpy.average, 0, Y_in)
    print 'In-sample (%s):' % ss_in
    print "sd(Yhat):"
    print numpy.apply_along_axis(numpy.std, 0, Yhat_in)
    print w[0]
    r2 = []
    for i in range(Y_in.shape[1]):
        r2.append(round(CalcR2(Y_in[numpy.arange(ss_in), i].tolist(), Yhat_in[numpy.arange(ss_in), i].tolist(), Ybar_in[i]), 4))
    print r2

    ss_out = len(data_X) - ss_in
    X_out = numpy.array(data_X[ss_in:])
    Y_out = numpy.array(data_Y[ss_in:])
    Yhat_out = numpy.dot(X_out, w[0]) 
    Ybar_out = numpy.apply_along_axis(numpy.average, 0, Y_out)
    print 'Out-of-sample (%s):' % ss_out
    r2 = []
    for i in range(Y_out.shape[1]):
        r2.append(round(CalcR2(Y_out[numpy.arange(ss_out), i].tolist(), Yhat_out[numpy.arange(ss_out), i].tolist(), Ybar_out[i]), 4))
    print r2
   
