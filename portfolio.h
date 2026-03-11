#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <time.h> // For time_t

// --- DATA STRUCTS ---

#define MAX_TRANSACTIONS 500
#define MAX_STOCKS 100
#define SYMBOL_LEN 10

// Represents a single stock data point
typedef struct {
    char symbol[SYMBOL_LEN];
    double price;
    double open;
    double high;
    double low;
    double prevClose;
    time_t lastUpdated;
    double fiftyTwoWeekHigh;
    double fiftyTwoWeekLow;
} StockData;

// Transaction Types
typedef enum { 
    BUY,    // Buy to open a long position
    SELL,   // Sell to close a long position
    SHORT,  // Short-sell to open a short position
    COVER   // Buy to cover a short position
} TransactionType;

typedef struct {
    TransactionType type;
    char symbol[SYMBOL_LEN];
    int quantity;
    double pricePerShare;
    double charge;
} Transaction;

// Calculated Holding
typedef struct {
    char symbol[SYMBOL_LEN];
    int totalQuantity;
    double avgPrice;
    double purchaseValue;
    double marketPrice;
    double marketValue;
    double unrealizedProfit;
} Holding;

// Portfolio (Main User Data)
typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    int numTransactions;
    double totalRealizedProfit;
} Portfolio;


// --- FUNCTION PROTOTYPES ---

void addTransaction(Portfolio *p);
void viewTransactions(const Portfolio *p);
void displayPortfolioSummary(Portfolio *p);
void displayStockStats(Portfolio *p);

// --- NEW FUNCTION ---
void displayPopularStocks();

#endif // PORTFOLIO_H
