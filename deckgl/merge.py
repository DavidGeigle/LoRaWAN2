import pandas as pd

a = pd.read_csv("location.csv")
b = pd.read_csv("rssi.csv")
merged = a.merge(b, on='time')
merged.to_csv("output.csv", index=False)
