import pandas as pd

a = pd.read_csv("location.csv")
b = pd.read_csv("rssi.csv")
merged = a.merge(b, on='time')
merged = merged.drop("name_y",axis=1)
merged = merged.drop("name_x",axis=1)
merged.to_csv("output.csv", index=False)
