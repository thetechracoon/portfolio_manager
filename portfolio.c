#include "portfolio.h"
#include "api_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>  
#include <ctype.h>

// --- HELPER: CALCULATED HOLDING ---
// Used locally to aggregate transaction data into a current snapshot
typedef struct {
    char symbol[SYMBOL_LEN];
    int totalQuantity;       
    double totalBuyCost;     
    int totalBuyQty;
    double totalShortRevenue;
    int totalShortQty;
} CalcHolding;

// Finds or creates a holding entry in our temporary calculation array
CalcHolding* getOrCreateCalcHolding(CalcHolding* holdings, int* numHoldings, const char* symbol) {
    for (int i = 0; i < *numHoldings; i++) {
        if (strcmp(holdings[i].symbol, symbol) == 0) {
            return &holdings[i];
        }
    }
    // Not found, create new
    CalcHolding* newHolding = &holdings[*numHoldings];
    (*numHoldings)++;
    strcpy(newHolding->symbol, symbol);
    newHolding->totalQuantity = 0;
    newHolding->totalBuyCost = 0.0;
    newHolding->totalBuyQty = 0;
    newHolding->totalShortRevenue = 0.0;
    newHolding->totalShortQty = 0;
    return newHolding;
}

// --- NEW FEATURE: POPULAR STOCKS LIST ---
void displayPopularStocks() {
    printf("\n--- Popular Stocks for Reference ---\n\n");
    const char* stocks[] = {
        "AAPL", "Apple Inc.",
        "MSFT", "Microsoft",
        "GOOGL", "Alphabet (Google)",
        "AMZN", "Amazon",
        "TSLA", "Tesla",
        "NVDA", "NVIDIA",
        "META", "Meta (Facebook)",
        "NFLX", "Netflix",
        "AMD", "Adv. Micro Devices",
        "INTC", "Intel",
        "IBM", "IBM",
        "ORCL", "Oracle",
        "CSCO", "Cisco",
        "ADBE", "Adobe",
        "CRM", "Salesforce",
        "QCOM", "Qualcomm",
        "TXN", "Texas Instruments",
        "AVGO", "Broadcom",
        "PYPL", "PayPal",
        "SQ", "Block (Square)"
    };

    printf("%-10s | %-20s  ||  %-10s | %-20s\n", "Symbol", "Company", "Symbol", "Company");
    printf("-------------------------------------------------------------------------\n");
    
    for (int i = 0; i < 20; i += 2) {
        // Print two columns side by side
        printf("%-10s | %-20s  ||  %-10s | %-20s\n", 
               stocks[i*2], stocks[i*2+1], 
               stocks[(i+1)*2], stocks[(i+1)*2+1]);
    }
    printf("\n(Note: Use these symbols when adding transactions)\n");
}

// --- TRANSACTIONS ---
void addTransaction(Portfolio *p) {
    if (p->numTransactions >= MAX_TRANSACTIONS) {
        printf("Error: Log full.\n"); return;
    }
    Transaction *t = &p->transactions[p->numTransactions];
    char typeChar;
    
    // Updated prompt to use L for Sell and S for Short
    printf("Enter Type (B)uy, se(L)l, (S)hort, (C)over: ");
    scanf(" %c", &typeChar);
    
    if (toupper(typeChar) == 'B') t->type = BUY;
    else if (toupper(typeChar) == 'L') t->type = SELL;
    else if (toupper(typeChar) == 'S') t->type = SHORT;
    else if (toupper(typeChar) == 'C') t->type = COVER;
    else { printf("Invalid type.\n"); return; }

    printf("Enter Stock Symbol: ");
    scanf("%9s", t->symbol);
    
    printf("Enter Quantity: ");
    while (scanf("%d", &t->quantity) != 1 || t->quantity <= 0) {
        printf("Invalid input. Retry: "); while(getchar() != '\n');
    }
    
    printf("Enter Price Per Share: ");
    while (scanf("%lf", &t->pricePerShare) != 1 || t->pricePerShare < 0) {
        printf("Invalid input. Retry: "); while(getchar() != '\n');
    }

    // Auto-charge 0.4% logic
    t->charge = (t->quantity * t->pricePerShare) * 0.004;
    printf("Calculated Charge (0.4%%): $%.2f\n", t->charge);

    // Calculate Realized Profit immediately for closing transactions
    if (t->type == SELL) {
        double totalCost = 0; int totalQuantity = 0;
        // Calculate average cost basis from previous BUYS
        for(int i = 0; i < p->numTransactions; i++) {
            if(strcmp(p->transactions[i].symbol, t->symbol) == 0 && p->transactions[i].type == BUY) {
                totalCost += (p->transactions[i].pricePerShare * p->transactions[i].quantity) + p->transactions[i].charge;
                totalQuantity += p->transactions[i].quantity;
            }
        }
        if (totalQuantity > 0) {
            double avgCost = totalCost / totalQuantity;
            // Profit = (Sell Value - Charge) - Cost Basis
            double profit = ((t->pricePerShare * t->quantity) - t->charge) - (avgCost * t->quantity);
            p->totalRealizedProfit += profit;
            printf("\n--- REALIZED PROFIT: $%.2f ---\n", profit);
        }
    } else if (t->type == COVER) {
        double totalRevenue = 0; int totalQuantity = 0;
        // Calculate average revenue from previous SHORTS
        for(int i = 0; i < p->numTransactions; i++) {
            if(strcmp(p->transactions[i].symbol, t->symbol) == 0 && p->transactions[i].type == SHORT) {
                totalRevenue += (p->transactions[i].pricePerShare * p->transactions[i].quantity) - p->transactions[i].charge;
                totalQuantity += p->transactions[i].quantity;
            }
        }
        if (totalQuantity > 0) {
            double avgRev = totalRevenue / totalQuantity;
            // Profit = Revenue Basis - (Cover Cost + Charge)
            double profit = (avgRev * t->quantity) - ((t->pricePerShare * t->quantity) + t->charge);
            p->totalRealizedProfit += profit;
            printf("\n--- REALIZED PROFIT: $%.2f ---\n", profit);
        }
    }

    (p->numTransactions)++;
    printf("\nTransaction logged.\n");
}

void viewTransactions(const Portfolio *p) {
    printf("%-7s | %-10s | %-5s | %-10s | %-10s\n", "Type", "Symbol", "Qty", "Price", "Charge");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < p->numTransactions; i++) {
        const Transaction *t = &p->transactions[i];
        char typeStr[8];
        switch(t->type) {
            case BUY: strcpy(typeStr, "BUY"); break;
            case SELL: strcpy(typeStr, "SELL"); break;
            case SHORT: strcpy(typeStr, "SHORT"); break;
            case COVER: strcpy(typeStr, "COVER"); break;
        }
        printf("%-7s | %-10s | %-5d | $%-9.2f | $%-9.2f\n", typeStr, t->symbol, t->quantity, t->pricePerShare, t->charge);
    }
}

void displayPortfolioSummary(Portfolio *p) {
    // 1. Aggregate all transactions into current holdings
    CalcHolding calcHoldings[MAX_STOCKS];
    int numHoldings = 0;

    for (int i = 0; i < p->numTransactions; i++) {
        Transaction *t = &p->transactions[i];
        CalcHolding* h = getOrCreateCalcHolding(calcHoldings, &numHoldings, t->symbol);
        switch (t->type) {
            case BUY: 
                h->totalQuantity += t->quantity; 
                h->totalBuyQty += t->quantity;
                h->totalBuyCost += (t->pricePerShare * t->quantity) + t->charge;
                break;
            case SELL: h->totalQuantity -= t->quantity; break;
            case SHORT: 
                h->totalQuantity -= t->quantity; // Short quantity is negative in net total
                h->totalShortQty += t->quantity;
                h->totalShortRevenue += (t->pricePerShare * t->quantity) - t->charge;
                break;
            case COVER: h->totalQuantity += t->quantity; break;
        }
    }

    // 2. Fetch live data and print table
    printf("Fetching live data...\n\n");
    double grandUnrealized = 0;

    printf("%-10s | %-5s | %-10s | %-10s | %-10s | %-10s | %-10s\n",
           "Symbol", "Qty", "Avg Price", "Mkt Price", "P/L", "Basis Val", "Market Val");
    printf("---------------------------------------------------------------------------------------\n");

    for (int i = 0; i < numHoldings; i++) {
        CalcHolding *h = &calcHoldings[i];
        if (h->totalQuantity == 0) continue; // Skip closed positions

        StockData data;
        if (fetchStockData(h->symbol, &data) != 0) {
            printf("Could not fetch %s.\n", h->symbol); continue;
        }

        double avgPrice, basisValue, marketValue, unrealizedProfit;
        
        // Logic for LONG positions
        if (h->totalQuantity > 0) { 
            avgPrice = (h->totalBuyQty > 0) ? h->totalBuyCost / h->totalBuyQty : 0;
            basisValue = avgPrice * h->totalQuantity;
            marketValue = data.price * h->totalQuantity;
            unrealizedProfit = marketValue - basisValue;
        } 
        // Logic for SHORT positions
        else { 
            avgPrice = (h->totalShortQty > 0) ? h->totalShortRevenue / h->totalShortQty : 0;
            basisValue = avgPrice * abs(h->totalQuantity); // Revenue received
            marketValue = data.price * abs(h->totalQuantity); // Cost to buy back
            unrealizedProfit = basisValue - marketValue; // Profit is Revenue - Cost
        }
        
        grandUnrealized += unrealizedProfit;
        printf("%-10s | %-5d | $%-9.2f | $%-9.2f | $%-10.2f | $%-9.2f | $%-9.2f\n",
               h->symbol, h->totalQuantity, avgPrice, data.price, unrealizedProfit, basisValue, marketValue);
    }
    printf("---------------------------------------------------------------------------------------\n");
    printf("Total Unrealized Profit: $%.2f\n", grandUnrealized);
    printf("Total Realized Profit:   $%.2f\n", p->totalRealizedProfit);
}

void displayStockStats(Portfolio *p) {
    char symbol[SYMBOL_LEN];
    printf("Enter Stock Symbol: ");
    scanf("%9s", symbol);
    StockData data;
    printf("Fetching %s...\n", symbol);
    
    if (fetchStockData(symbol, &data) == 0) {
        char timeStr[30];
        strftime(timeStr, 30, "%Y-%m-%d %H:%M:%S", localtime(&data.lastUpdated));
        
        // Try to fetch extended stats (52 week high/low)
        fetchStockOverview(symbol, &data); 

        printf("\n--- Stats for %s ---\n", data.symbol);
        printf("Last Updated:   %s\n", timeStr);
        printf("Current Price:  $%.2f\n", data.price);
        printf("Open:           $%.2f\n", data.open);
        printf("Day High:       $%.2f\n", data.high);
        printf("Day Low:        $%.2f\n", data.low);
        printf("Previous Close: $%.2f\n", data.prevClose);
        printf("52 Week High:   $%.2f\n", data.fiftyTwoWeekHigh);
        printf("52 Week Low:    $%.2f\n", data.fiftyTwoWeekLow);
    } else {
        printf("Could not fetch data for %s.\n", symbol);
    }
}
