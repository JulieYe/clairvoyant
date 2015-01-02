import csv
import optparse
import numpy

def run(filename, side):
    
    with open(filename, 'r') as fd:
        reader = csv.reader(fd)
        idx = reader.next().index('bid1_qty' if side in ['b', 'bid'] else 'ask1_qty')
        x = []
        for row in reader:
            x.append(float(row[idx]))
        print numpy.median(x)

if __name__ == '__main__':

    parser = optparse.OptionParser()
    parser.add_option('-f', '--file', dest='file', help='file name')
    parser.add_option('-s', '--side', dest='side', help='side')

    (options, args) = parser.parse_args()

    run(options.file, options.side)
