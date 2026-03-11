#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "portfolio.h"

// --- EXISTING FUNCTIONS ---
void loadPortfolio(Portfolio *p, const char *filename);
void savePortfolio(const Portfolio *p, const char *filename);

// --- NEW FUNCTIONS FOR AUTH & SECURITY ---
// Loads the API key from the .env file into a buffer
int loadApiKey(char *buffer, size_t size);

// Checks if username/password combo exists in users.dat
// Returns 1 if valid, 0 if invalid
int authenticateUser(const char *username, const char *password);

// Adds a new user to users.dat
// Returns 1 if successful, 0 if username taken
int registerUser(const char *username, const char *password);

#endif // FILE_HANDLER_H
