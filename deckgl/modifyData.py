import csv
import os
import random
import math

if os.path.exists("data_out.csv"):
    os.remove("data_out.csv")

with open("output.csv", newline='') as data:
    reader = csv.DictReader(data, delimiter=',')

    with open('data_out.csv', mode='w', newline='') as out:
        fieldnames = ['time', 'latitude', 'longitude', 'rssi']
        writer = csv.DictWriter(out, fieldnames=fieldnames)
        writer.writeheader()

        for line in reader:
            rssi = int(math.log10(127 - int(line['rssi'][0:line['rssi'].find('.')])))
            for x in range(rssi):
                lat = float(line['latitude']) + random.uniform(0.000001, 0.00001)
                lng = float(line['longitude']) + random.uniform(0.000001, 0.00001)
                writer.writerow({'time': line['time'], 'latitude': lat, 'longitude': lng, 'rssi': line['rssi']})
