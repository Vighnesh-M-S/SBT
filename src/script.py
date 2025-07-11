import pandas as pd
import numpy as np

def check_trending_down(prices_series):
    prices = prices_series.to_numpy()
    if len(prices) < 2:
        return 0
    down_moves = sum(prices[i] < prices[i-1] for i in range(1, len(prices)))
    return int(down_moves >= len(prices) // 2)

WINDOW_SIZE = 7  # rolling window size
DROP_THRESHOLD = 0.01  # 1% drop = label 1

# Load your CSV
df = pd.read_csv("/Users/vighneshms/Downloads/SBT/src/Stable Coin_.csv", delimiter=';')
df.columns = df.columns.str.strip()
print("🧾 Columns:", df.columns.tolist())

# Convert close prices to float
df["close"] = pd.to_numeric(df["close"], errors="coerce")
df = df.dropna(subset=["close"]).reset_index(drop=True)

# Set window size (e.g., last 7 days)
WINDOW_SIZE = 7

# Initialize columns
df["avg"] = df["close"].rolling(WINDOW_SIZE).mean()
df["stddev"] = df["close"].rolling(WINDOW_SIZE).std()

# Trending down: if 4 out of last 6 days are decreasing
# def check_trending_down(prices):
#     if len(prices) < 2:
#         return 0
#     down_moves = sum(prices[i] < prices[i-1] for i in range(1, len(prices)))
#     return int(down_moves >= len(prices) // 2)

df["trendingDown"] = df["close"].rolling(WINDOW_SIZE).apply(check_trending_down, raw=False)

# Label: depeg if close drops > 0.5% from previous day
df["label"] = (df["close"].pct_change().fillna(0) < -0.005).astype(int)

# Drop rows with NaNs (from rolling)
df = df.dropna().reset_index(drop=True)

# Save to CSV
df[["name", "avg", "stddev", "trendingDown", "label"]].to_csv("historical_features_labeled.csv", index=False)
print("✅ Feature CSV saved as 'historical_features_labeled.csv'")