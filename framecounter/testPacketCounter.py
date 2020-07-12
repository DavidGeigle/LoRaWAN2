#20.6 21Uhr - 5.7.
#14eb752bd35b0ccc

import csv
import pandas

with open("framecounter.csv", newline='') as data:
    reader = csv.DictReader(data, delimiter=',')

    v = 1

    for line in reader:
        if line['dev_eui'] == '14eb752bd35b0ccc':
            #time = pandas.to_datetime(line['time'][0:-9], unit='s')
            if int(line['value']) == 1:
                print("Start bei 1:")
                v = 1
            if int(line['value']) != v:
                print("Paket: " + str(v) + " fehlt")
                v += 1
            print("Paket: " + line['value'])
            v += 1

