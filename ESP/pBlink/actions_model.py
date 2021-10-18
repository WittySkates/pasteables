# Processing imports

import re
import csv
import os

from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import StandardScaler  
from sklearn.metrics import precision_score
from sklearn.metrics import recall_score



# Functions

# Train new model
def train_new_model_new(path, model):
    
    all_csv_files = []
    path = r'C:\Users\conno\Desktop\Pastebles\data'

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


    # Changes target from string to numeric
    le = LabelEncoder().fit(y_data)
    y = le.transform(y_data)

    # Sets the X data
    X = np.array(X_data)
    
    # Splits data into training and testing
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.20) # random_state=42
    
    # Pipeline from data pre-processing to model training
    pipe_model = make_pipeline(StandardScaler(), model)
    pipe_model.fit(X_train, y_train)
    
    # Model predictions
    y_pred = pipe_model.predict(X_test)

    accuracy = pipe_model.score(X_test, y_test)
    recall = recall_score(y_test, y_pred, average='macro')
    precision = precision_score(y_test, y_pred, average='macro')
    
    # Save model
    #joblib.dump(pipe_model, 'actions_model.pkl')

    print("Model:", model, "\nAccuracy:", accuracy, "\nRecall:", recall, "\nPrecision:", precision)

train_new_model_new(r'C:\Users\conno\Desktop\Pastebles\data', RandomForestClassifier())
