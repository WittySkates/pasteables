# main.py

import machine
import time
import re
#import csv
import os


pin = machine.Pin(2, machine.Pin.OUT)
path = r'\Person_1'
while(1):
    print("0")
    
    all_csv_files = []
    path = r'\Person_1'

    # Will grab any subfolders from path and their csv files
    for root,dirs,files in os.walk(path):
        for filename in files:
            if filename.endswith(".csv"):
                all_csv_files.append(root+'\\'+filename)

    X_data = []
    y_data = []
    for filename in all_csv_files:
        match  = re.findall('[a-zA-Z]+(?=_[0-9]+_data)', str(filename))
        y = (''.join(match))
        # Reads individual csv files
        with open(filename, newline='') as csvfile:
            spamreader = csv.reader(csvfile, delimiter=',')
            for index, row in enumerate(spamreader):
                if(index != 0):
                    y_data.append(y)
                    # Remove arrival time (column 0)
                    add = row[1:8]
                    X_data.append(add)

        pin.value(1)
        time.sleep(0.5)
        pin.value(0)
        time.sleep(0.5)
        print(X_data)