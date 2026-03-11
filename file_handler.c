#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USERS_DB "users.dat"

// --- SECURITY: LOAD .ENV ---
int loadApiKey(char *buffer, size_t size) {
    FILE *fp = fopen(".env", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: .env file not found!\n");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // Look for line starting with API_KEY=
        if (strncmp(line, "API_KEY=", 8) == 0) {
            // Remove newline at end if exists
            line[strcspn(line, "\n")] = 0;
            // Copy everything after the '='
            strncpy(buffer, line + 8, size - 1);
            buffer[size - 1] = '\0'; // Ensure null term
            fclose(fp);
            return 1; // Success
        }
    }

    fclose(fp);
    return 0; // Key not found
}

// --- AUTHENTICATION ---
int authenticateUser(const char *username, const char *password) {
    FILE *fp = fopen(USERS_DB, "r");
    if (fp == NULL) return 0; // No users exist yet

    char fileUser[50], filePass[50];
    while (fscanf(fp, "%s %s", fileUser, filePass) == 2) {
        if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
            fclose(fp);
            return 1; // Match found
        }
    }
    fclose(fp);
    return 0;
}

int registerUser(const char *username, const char *password) {
    // Check if user exists first
    FILE *fp = fopen(USERS_DB, "r");
    if (fp != NULL) {
        char fileUser[50], filePass[50];
        while (fscanf(fp, "%s %s", fileUser, filePass) == 2) {
            if (strcmp(username, fileUser) == 0) {
                fclose(fp);
                return 0; // User already exists
            }
        }
        fclose(fp);
    }

    // Append new user
    fp = fopen(USERS_DB, "a"); // 'a' for append
    if (fp == NULL) return 0;

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
    return 1;
}

// --- PORTFOLIO I/O (Unchanged logic, just handles passed filename) ---
void loadPortfolio(Portfolio *p, const char *filename) {
    p->numTransactions = 0;
    p->totalRealizedProfit = 0.0;
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        // New user won't have a file yet, that's fine
        return; 
    }
    
    fscanf(fp, "REALIZED_PROFIT,%lf\n", &p->totalRealizedProfit);

    int type_int;
    while (p->numTransactions < MAX_TRANSACTIONS &&
           fscanf(fp, "%d,%9[^,],%d,%lf,%lf\n",
                  &type_int,
                  p->transactions[p->numTransactions].symbol,
                  &p->transactions[p->numTransactions].quantity,
                  &p->transactions[p->numTransactions].pricePerShare,
                  &p->transactions[p->numTransactions].charge) == 5)
    {
        p->transactions[p->numTransactions].type = (TransactionType)type_int;
        p->transactions[p->numTransactions].symbol[SYMBOL_LEN - 1] = '\0'; 
        (p->numTransactions)++;
    }

    fclose(fp);
}

void savePortfolio(const Portfolio *p, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return;
    
    fprintf(fp, "REALIZED_PROFIT,%.2f\n", p->totalRealizedProfit);

    for (int i = 0; i < p->numTransactions; i++) {
        const Transaction *t = &p->transactions[i];
        fprintf(fp, "%d,%s,%d,%.2f,%.2f\n",
                (int)t->type, t->symbol, t->quantity, t->pricePerShare, t->charge);
    }
    fclose(fp);
}
