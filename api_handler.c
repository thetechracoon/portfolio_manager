#include "api_handler.h"
#include "file_handler.h" // To load .env
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>

// Global variable to store the key once loaded
char GLOBAL_API_KEY[128] = "";

void initApiHandler() {
    if (!loadApiKey(GLOBAL_API_KEY, sizeof(GLOBAL_API_KEY))) {
        printf("WARNING: Could not load API_KEY from .env file.\n");
        printf("Please create a .env file with API_KEY=your_key\n");
        // Set a fallback or leave empty (will cause API errors)
        strcpy(GLOBAL_API_KEY, "demo"); 
    }
}

// --- Standard libcurl helpers (Same as before) ---
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) { return 0; }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// JSON Helpers
double get_json_double(cJSON *obj, const char *key) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (cJSON_IsString(item) && (item->valuestring != NULL)) return atof(item->valuestring);
    return 0.0;
}

char* get_json_string(cJSON *obj, const char *key) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (cJSON_IsString(item) && (item->valuestring != NULL)) return item->valuestring;
    return "";
}


// --- Main API Function (Secure) ---
int fetchStockData(const char *symbol, StockData *data) {
    char url[256];
    // Use the global key
    snprintf(url, sizeof(url), 
             "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%s&apikey=%s",
             symbol, GLOBAL_API_KEY);

    int ret = -1; 
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl_handle);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *root = cJSON_Parse(chunk.memory);
            if (root) {
                cJSON *quote = cJSON_GetObjectItemCaseSensitive(root, "Global Quote");
                if (quote) {
                    strncpy(data->symbol, get_json_string(quote, "01. symbol"), SYMBOL_LEN - 1);
                    data->symbol[SYMBOL_LEN - 1] = '\0';
                    data->price = get_json_double(quote, "05. price");
                    data->open = get_json_double(quote, "02. open");
                    data->high = get_json_double(quote, "03. high");
                    data->low = get_json_double(quote, "04. low");
                    data->prevClose = get_json_double(quote, "08. previous close");
                    data->lastUpdated = time(NULL);
                    ret = 0;
                }
                cJSON_Delete(root);
            }
        }
        curl_easy_cleanup(curl_handle);
    }
    free(chunk.memory);
    return ret;
}

int fetchStockOverview(const char *symbol, StockData *data) {
    char url[256];
    snprintf(url, sizeof(url), 
             "https://www.alphavantage.co/query?function=OVERVIEW&symbol=%s&apikey=%s",
             symbol, GLOBAL_API_KEY); // Using the secure key

    int ret = -1; 
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl_handle);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *root = cJSON_Parse(chunk.memory);
            if (root) {
                data->fiftyTwoWeekHigh = get_json_double(root, "52WeekHigh");
                data->fiftyTwoWeekLow = get_json_double(root, "52WeekLow");
                ret = 0;
                cJSON_Delete(root);
            }
        }
        curl_easy_cleanup(curl_handle);
    }
    free(chunk.memory);
    return ret;
}
