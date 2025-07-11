from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
import joblib  
import pandas as pd

# Load your CSV
df = pd.read_csv("historical_features_labeled.csv")

# Features and labels
X = df[["avg", "stddev", "trendingDown"]]
y = df["label"]

# Train/test split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train model
clf = RandomForestClassifier()
clf.fit(X_train, y_train)

# Evaluate
y_pred = clf.predict(X_test)
joblib.dump(clf, "trend_classifier.pkl")
print(classification_report(y_test, y_pred))