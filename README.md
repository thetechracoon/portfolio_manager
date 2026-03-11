# Vantage TUI
### Terminal Stock Portfolio Manager
*Written in C · EGC 111P Team Project*

---

Vantage TUI is a terminal-based stock portfolio manager built entirely in C. It supports multiple users, secure API key management via a `.env` configuration file, and live market data powered by the Alpha Vantage API. Users can log and track BUY, SELL, SHORT, and COVER transactions, monitor unrealized profit/loss, and review real-time stock statistics — all from the command line.

---

## Features

| Feature | Description |
|---|---|
| **Multi-User System** | Login and Registration system — each user maintains their own independent portfolio save file. |
| **Secure Configuration** | Uses a `.env` file to store API keys, keeping credentials out of source code. |
| **Transaction Ledger** | Logs all BUY, SELL, SHORT, and COVER transactions with automatic 0.4% transaction charges. |
| **Realized P&L Tracking** | Automatically calculates profit/loss when closing positions. |
| **Live Holdings Summary** | Fetches real-time data to display Market Value and Unrealized Profit/Loss. |
| **Live Stock Stats** | Displays current Price, Day High/Low, and 52-Week range statistics. |
| **Popular Stocks List** | Built-in quick-reference list of common ticker symbols. |

---

## Prerequisites

Before compiling, install the required development libraries for `libcurl` and `cJSON`.

**On Ubuntu / Debian / Pop!_OS:**
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev libcjson-dev
```

---

## How to Compile

### Step 1 — Set up your API key

1. Get a free API key from [Alpha Vantage](https://www.alphavantage.co).
2. Create a file named `.env` in the project root directory.
3. Add your key in the following format:

```
API_KEY=YOUR_ACTUAL_KEY_HERE
```

### Step 2 — Run Make

Open a terminal in the project directory and run:

```bash
make
```

This compiles all source files and produces an executable named `portfolio_manager`.

---

## How to Run

After a successful build, launch the program with:

```bash
./portfolio_manager
```

---

## Usage Guide

- **First Run — Register:** Select option `2` from the main menu to create a new user account.
- **Login:** Enter your registered credentials to access your personal portfolio.
- **Manage Portfolio:** Use the in-app menu to record transactions (BUY / SELL / SHORT / COVER) or view your live holdings summary.

---

## Data Persistence

All data is written to disk automatically. The following files are created in the project directory:

| File | Contents |
|---|---|
| `users.dat` | All registered user credentials |
| `portfolio_USERNAME.dat` | Per-user transaction and holdings data |

Keep these files in the same directory as the executable to preserve your data across sessions.

---

## Tech Stack

- **Language:** C (C99)
- **HTTP Client:** libcurl
- **JSON Parsing:** cJSON
- **Market Data:** Alpha Vantage REST API
- **Build System:** GNU Make

---

*EGC 111P Team Project · Vantage TUI*
