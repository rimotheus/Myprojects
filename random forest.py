import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn import metrics

# Read CSV file into a DataFrame
df = pd.read_csv(r"C:\Users\cheon\Desktop\hr_params.csv")
# Handle missing values, if necessary
# df = df.dropna()

# Prepare X and Y data
Y = df['likert']
X = df.drop(labels=['heart rate', 'Amplitude', 'SCR Amplitude', 'AccelX_RMS', 'AccelY_RMS', 'AccelZ_RMS', 'GyroX_RMS', 'GyroY_RMS', 'GyroZ_RMS'], axis=1)

# Split data into train and test sets
X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.4, random_state=20)

# Create and fit the Random Forest Classifier model
model = RandomForestClassifier(n_estimators=10, random_state=30)
model.fit(X_train, Y_train)
# Make predictions on test data
prediction_test = model.predict(X_test)

# Calculate accuracy
accuracy = metrics.accuracy_score(Y_test, prediction_test)
print('Accuracy =', accuracy)

# Get feature importances
feature_list = list(X.columns)
feature_imp = pd.Series(model.feature_importances_, index=feature_list).sort_values(ascending=False)
print(feature_imp)

# Read the new CSV file into a DataFrame
#df = pd.read_csv(r"path/to/new_data.csv")
#Prepare the input data for prediction by selecting the appropriate features (columns) from the DataFrame, and making sure they match the features used during training. In your case, it seems like you are dropping the 'Amplitude' column from the input data to predict the 'Tilt' column.
#python
#Copy code
#X = df.drop(labels=['Amplitude'], axis=1)
#Use the model.predict() method to make predictions on the new data.

#predictions = model.predict(X)










#Read the new CSV file into a DataFrame using pd.read_csv() function, just like you did with the training data.
#python

#import pandas as pd
#from sklearn.ensemble import RandomForestClassifier
#from sklearn import metrics

