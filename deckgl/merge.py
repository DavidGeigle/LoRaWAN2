import pandas

loc = pandas.read_csv("location.csv")
rssi = pandas.read_csv("rssi.csv")

merged = loc.merge(rssi, on='time')
merged = merged.drop("name_y", axis=1)
merged = merged.drop("name_x", axis=1)

merged['rssi'] = 100-(merged['rssi']*-1)*(100/127)

merged = merged.drop_duplicates(['latitude', 'longitude'])

merged.to_csv("output.csv", index=False)
