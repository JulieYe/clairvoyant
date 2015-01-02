#!/usr/bin/python

import sys

input_filename = sys.argv[1]
output_filename = sys.argv[2]

with open(input_filename, 'r') as fd:
    data = fd.readlines()
    data = data[1:]
    data.reverse()

with open(output_filename, 'w') as fd:
    fd.write("date_time,last_px,total_volume,bid1_px,bid1_qty,bid2_px,bid2_qty,bid3_px,bid3_qty,bid4_px,bid4_qty,bid5_px,bid5_qty,ask1_px,ask1_qty,ask2_px,ask2_qty,ask3_px,ask3_qty,ask4_px,ask4_qty,ask5_px,ask5_qty\n")
    for i, eachline in enumerate(data):
        fields = eachline.split(',')
        if i == 0:
            next_fields = data[i+1].split(',')
            if next_fields[2] != fields[2]:
                fields[2] = "%s.250" % fields[2].replace(' ','T')
            else:
                fields[2] = "%s.000" % fields[2].replace(' ','T')
        elif i == (len(data)-1):
            prev_fields = data[i-1].split(',')
            if prev_fields[2] != fields[2]:
                fields[2] = "%s.250" % fields[2].replace(' ','T')
            else:
                fields[2] = "%s.500" % fields[2].replace(' ','T')
        else:
            prev_fields = data[i-1].split(',')
            next_fields = data[i+1].split(',')
            if prev_fields[2] == fields[2]:
                fields[2] = "%s.500" % fields[2].replace(' ','T')
            elif next_fields[2] == fields[2]:
                fields[2] = "%s.000" % fields[2].replace(' ','T')
            else:
                fields[2] = "%s.250" % fields[2].replace(' ','T')
        line = fields[-21:-1]
        line.reverse()
        line.append(int(float(fields[7])))
        line.append(fields[3])
        line.append(fields[2])
        line.reverse()
        fd.write("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" % tuple(line))
