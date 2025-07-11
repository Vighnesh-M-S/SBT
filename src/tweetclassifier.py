import pandas as pd
import numpy as np
import re

from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.pipeline import Pipeline
from sklearn.model_selection import train_test_split

import joblib  

with open("/Users/vighneshms/Downloads/SBT/build/usdc_tweets.txt", "r") as f:
    tweets = f.readlines()

tweets = [t.strip() for t in tweets if t.strip()]


training_data = [
    {"text": "Bridges get hacked. Wrapped tokens depeg. Custodians rug.", "label": 1},
    {"text": "Even the strongest stablecoins depeg from time to time.", "label": 1},
    {"text": "Stablecoins hold steady amidst volatility.", "label": 0},
    {"text": "FTX, LUNA, UST collapse shows the risks.", "label": 1},
    {"text": "USDC remains strong and backed by reserves.", "label": 0},
]

df_train = pd.DataFrame(training_data)


def clean_text(text):
    text = re.sub(r"http\S+", "", text)      # Remove URLs
    text = re.sub(r"@\w+", "", text)         # Remove mentions
    text = re.sub(r"[^a-zA-Z\s$]", "", text) # Remove special chars except $
    return text.lower()

df_train["text"] = df_train["text"].apply(clean_text)
tweets_cleaned = [clean_text(t) for t in tweets]


model = Pipeline([
    ("tfidf", TfidfVectorizer(ngram_range=(1, 2), max_features=1000)),
    ("clf", LogisticRegression())
])


X_train, X_test, y_train, y_test = train_test_split(df_train["text"], df_train["label"], test_size=0.2, random_state=42)
model.fit(X_train, y_train)

# Optional: Save the model
joblib.dump(model, "tweet_risk_model.pkl")


risk_scores = model.predict_proba(tweets_cleaned)[:, 1]


output_file = "usdc_tweet_risk_scores.txt"
with open(output_file, "w") as f:
    for tweet, score in zip(tweets, risk_scores):
        f.write(f"{score:.2f}\t{tweet.strip()}\n")

print(f"✅ Risk scores saved to {output_file}")
