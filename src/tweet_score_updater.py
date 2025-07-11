import pandas as pd
import numpy as np
import re
import joblib
import os
from datetime import datetime
import time

TWEET_FILE = "usdc_tweets.txt"
MODEL_PATH = "tweet_risk_model.pkl"
CSV_PATH = "model_scores.csv"

def clean_text(text):
    text = re.sub(r"http\S+", "", text)
    text = re.sub(r"@\w+", "", text)
    text = re.sub(r"[^a-zA-Z\s$]", "", text)
    return text.lower()

# 1. Load tweets
def update_tweet_score():
    if not os.path.exists(TWEET_FILE):
        print("⚠️ Tweet file not found.")
        exit()

    with open(TWEET_FILE, "r") as f:
        tweets = [line.strip() for line in f if line.strip()]

    if not tweets:
        print("ℹ️ No tweets to analyze.")
        exit()

    # 2. Load model
    model = joblib.load(MODEL_PATH)

    # 3. Clean tweets
    tweets_cleaned = [clean_text(t) for t in tweets]

    # 4. Predict
    risk_scores = model.predict_proba(tweets_cleaned)[:, 1]
    avg_score = np.mean(risk_scores)

    # 5. Append to CSV
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # Create CSV if doesn't exist
    if not os.path.exists(CSV_PATH):
        with open(CSV_PATH, "w") as f:
            f.write("timestamp,priceTrendRisk,tweetScore,liquidityRisk,redemptionRisk,bridgeRisk\n")

    # Read old lines
    df = pd.read_csv(CSV_PATH)

    if not df.empty:
        df.at[len(df)-1, "tweetScore"] = round(avg_score, 4)
    else:
        df = pd.DataFrame([{
            "timestamp": timestamp,
            "priceTrendRisk": 0.0,
            "tweetScore": round(avg_score, 4),
            "liquidityRisk": 0.0,
            "redemptionRisk": 0.0,
            "bridgeRisk": 0.0,
        }])

    # Save back
    df.to_csv(CSV_PATH, index=False)
    print(f"✅ Tweet risk score ({avg_score:.4f}) added to {CSV_PATH}")

if __name__ == "__main__":
    while True:
        update_tweet_score()
        time.sleep(60)