#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <unistd.h>

#include "portfolio.h"
#include "file_handler.h"
#include "api_handler.h"

// Global variable to hold the current user's save file
char currentUserFile[100];
char currentUserName[50];

void waitForEnter() {
    printf("\n--- Press Enter to return ---");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); 
}

void clearScreen() {
    system("clear"); // Use "cls" on Windows
}

void printBanner() {
    clearScreen();
    printf("\n");
    printf("'##::::'##::::'###::::'##::: ##:'########::::'###:::::'######:::'########:\n");
    printf(" ##:::: ##:::'## ##::: ###:: ##:... ##..::::'## ##:::'##... ##:: ##.....::\n");
    printf(" ##:::: ##::'##:. ##:: ####: ##:::: ##:::::'##:. ##:: ##:::..::: ##:::::::\n");
    printf(" ##:::: ##:'##:::. ##: ## ## ##:::: ##::::'##:::. ##: ##::'####: ######:::\n");
    printf(". ##:: ##:: #########: ##. ####:::: ##:::: #########: ##::: ##:: ##...::::\n");
    printf(":. ## ##::: ##.... ##: ##:. ###:::: ##:::: ##.... ##: ##::: ##:: ##:::::::\n");
    printf("::. ###:::: ##:::: ##: ##::. ##:::: ##:::: ##:::: ##:. ######::: ########:\n");
    printf(":::...:::::..:::::..::..::::..:::::..:::::..:::::..:::......::::........::\n");
    printf("\n                         - TUI Portfolio Manager - \n");
}

// --- THE MAIN APP LOOP (Runs after login) ---
void runUserSession() {
    Portfolio portfolio;
    portfolio.numTransactions = 0;
    portfolio.totalRealizedProfit = 0.0;
    
    // Load THIS user's specific file
    loadPortfolio(&portfolio, currentUserFile);

    char choice;
    do {
        printBanner();
        printf("Logged in as: %s\n", currentUserName);
        printf("-----------------------------------\n");
        printf("[S]ummary (View Holdings)\n");
        printf("[T]ransaction (Log Buy/Sell/Short/Cover)\n");
        printf("[L]ist All Transactions\n");
        printf("[K]Stock Stats (Check Price)\n");
        printf("[P]opular Stocks List\n"); // NEW OPTION
        printf("[Q]uit (Log Out)\n");
        printf("\nTotal Realized Profit: $%.2f\n", portfolio.totalRealizedProfit);
        printf("Enter your choice: ");

        if (scanf(" %c", &choice) != 1) {
            while (getchar() != '\n');
            choice = ' '; 
        }

        switch (toupper(choice)) {
            case 'S':
                clearScreen();
                displayPortfolioSummary(&portfolio);
                waitForEnter();
                break;
            case 'T':
                clearScreen();
                addTransaction(&portfolio);
                savePortfolio(&portfolio, currentUserFile);
                waitForEnter();
                break;
            case 'L':
                clearScreen();
                viewTransactions(&portfolio);
                waitForEnter();
                break;
            case 'K':
                clearScreen();
                displayStockStats(&portfolio);
                waitForEnter();
                break;
            case 'P': // NEW CASE
                clearScreen();
                displayPopularStocks();
                waitForEnter();
                break;
            case 'Q':
                savePortfolio(&portfolio, currentUserFile);
                printf("\nSaving and logging out...\n");
                sleep(1);
                break;
            default:
                printf("\nInvalid choice.\n");
                sleep(1);
                break;
        }

    } while (toupper(choice) != 'Q');
}

// --- AUTHENTICATION MENUS ---
void handleLogin() {
    char user[50], pass[50];
    printf("\n--- Login ---\n");
    printf("Username: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", pass);

    if (authenticateUser(user, pass)) {
        strcpy(currentUserName, user);
        // Create unique filename: portfolio_username.dat
        sprintf(currentUserFile, "portfolio_%s.dat", user);
        printf("\nLogin successful! Loading portfolio...\n");
        sleep(1);
        runUserSession();
    } else {
        printf("\nError: Invalid username or password.\n");
        sleep(2);
    }
}

void handleRegister() {
    char user[50], pass[50];
    printf("\n--- Register New User ---\n");
    printf("Username: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", pass);

    if (registerUser(user, pass)) {
        printf("\nRegistration successful! Please log in.\n");
    } else {
        printf("\nError: User already exists.\n");
    }
    sleep(2);
}

int main() {
    // Initialize API (Load key from .env)
    initApiHandler();
    curl_global_init(CURL_GLOBAL_ALL);

    char choice;
    do {
        printBanner();
        printf("1. Login\n");
        printf("2. Register New User\n");
        printf("3. Exit\n");
        printf("\nSelect option: ");
        
        if (scanf(" %c", &choice) != 1) {
            while (getchar() != '\n');
            choice = ' ';
        }

        switch (choice) {
            case '1': handleLogin(); break;
            case '2': handleRegister(); break;
            case '3': printf("Goodbye!\n"); break;
            default: printf("Invalid option.\n"); sleep(1); break;
        }
    } while (choice != '3');

    curl_global_cleanup();
    return 0;
}
