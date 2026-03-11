#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "portfolio.h" 

// Initialize the API handler (loads key)
void initApiHandler();

int fetchStockData(const char *symbol, StockData *data);
int fetchStockOverview(const char *symbol, StockData *data);

#endif // API_HANDLER_H
