// system library headers
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// external library headers
#include "curl/curl.h"
#include "json-c/json.h"

// MaxCompiler headers
#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int errorCount = 0; // counter of connection errors
int acceptCount = 0; // counter of accepted shares
int shareCount = 0; // counter of all sent shares

// data structure - HTTP headers, as part of HTTP requests
struct curl_slist *headers = NULL;
// data structure - contents of HTTP responses
struct responseStruct {
	char *content;
	size_t size;
};

// function - write contents of HTTP responses into memory;
// adapted from "https://curl.haxx.se/libcurl/c/getinmemory.html"
static size_t getInMemory(void *contents, size_t size, size_t nmemb, void *userp) {

	size_t realsize = size * nmemb;
	struct responseStruct *response = (struct responseStruct *) userp;
	response->content = realloc(response->content, response->size + realsize + 1);

	if (response->content == NULL) {
		fprintf(stderr, "ERROR: Not enough memory\n");
		return 0;
	}

	memcpy(&(response->content[response->size]), contents, realsize);
	response->size += realsize;
	response->content[response->size] = 0;
	return realsize;
}

// function - swap byte order (endianness) of 32-bit words;
// e.g. 0x12345678 into 0x78563412
uint swap(uint x) {
	uint x0 = (x >> 0) & 0xff;
	uint x1 = (x >> 8) & 0xff;
	uint x2 = (x >> 16) & 0xff;
	uint x3 = (x >> 24) & 0xff;
	return x0 << 24 | x1 << 16 | x2 << 8 | x3 << 0;
}

// improvised global semaphore
bool semaphore = true;

// data structure - thread data
struct threadStruct {
	char *url;
	char *userpwd;
	char *string;
	char *nonce;
};

// thread function - create and send shares with matching nonces
void *sendShare(void *arg) {

	// copy strings into memory
	struct threadStruct *threadData = (struct threadStruct *) arg;
	char *string = strdup(threadData->string);
	char *nonce = strdup(threadData->nonce);

	// once the strings are copied into memory, let the main thread continue
	semaphore = true;

	// initialize CURL handle and CURL status code
	CURL *CURL;
	CURLcode CURLcode;
	CURL = curl_easy_init();

	// if CURL handle initialization succeeds, continue
	if (CURL) {

		// write a matching nonce into a string
		for (int i = 0; i < 8; i++) {
			string[153 + i] = nonce[i];
		}

		// expand string into a share
		char share[303];
		char *share1 = "{\"method\": \"getwork\", \"params\": [";
		char *share2 = "], \"id\": 1}";

		strcpy(share, share1);
		strcat(share, string);
		strcat(share, share2);

		// allocate memory for the content of HTTP response
		struct responseStruct response;
		response.content = malloc(1);
		response.size = 0;

		// create a HTTP request to send share
		curl_easy_setopt(CURL, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(CURL, CURLOPT_URL, threadData->url);
		curl_easy_setopt(CURL, CURLOPT_USERPWD, threadData->userpwd);
		curl_easy_setopt(CURL, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(CURL, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(CURL, CURLOPT_TIMEOUT, 3);
		curl_easy_setopt(CURL, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(CURL, CURLOPT_POSTFIELDS, share);
		curl_easy_setopt(CURL, CURLOPT_WRITEFUNCTION, getInMemory);
		curl_easy_setopt(CURL, CURLOPT_WRITEDATA, (void *) &response);

		// get the starting time snapshot of submitting share
		struct timeval start, stop, diff;
		gettimeofday(&start, NULL);

		// send share to the mining node
		CURLcode = curl_easy_perform(CURL);
		curl_easy_cleanup(CURL);

		// if sending share succeeds, continue
		if (CURLcode == CURLE_OK) {

			// parse the content of HTTP response
			struct json_object *json, *jsonResult;
			json = json_tokener_parse(response.content);
			json_object_object_get_ex(json, "result", &jsonResult);

			// check if share was accepted ...
			if (strcmp(json_object_to_json_string(jsonResult), "true") == 0) {
				acceptCount++;
				shareCount++;
				printf("share ACCEPTED! ... ");
			}

			// ... or rejected
			else if (strcmp(json_object_to_json_string(jsonResult), "false") == 0) {
				shareCount++;
				printf("share REJECTED! ... ");
			}

			// print out share statistics
			printf("%d / %d (%.0Lf%%) ... ", acceptCount, shareCount,
				(long double) acceptCount / (long double) shareCount * 100);

			// calculate and print out a delay between submitting share and receiving response
			gettimeofday(&stop, NULL);
			timersub(&stop, &start, &diff);
			long double delay = (long double) diff.tv_sec + (long double) diff.tv_usec / 1000000;
			printf("delay %.0Lf ms\n", delay * 1000);

			// free json objects to avoid memory leaks
			json_object_put(json);
		}

		// if sending share fails, print out error
		else fprintf(stderr, "Share not sent\n");

		// free memory to avoid memory leaks
		free(response.content);
	}

	// if CURL handle initialization fails, print out error
	else {
		fprintf(stderr, "ERROR: Trouble with CURL library\n");
	}

	// free memory to prevent memory leaks
	free(string);
	free(nonce);

	// end of thread function
	return NULL;
}

// function - display instructions to run program
void use() {
	puts("Required parameters:");
	puts("   -o: a bitcoin mining node's URL address");
	puts("   -u: a bitcoin miner's username");
	puts("   -p: a bitcoin miner's password");
	puts("Optional parameters:");
	puts("   -h: display this help message and exit");
	puts("Command example:");
	puts("./BitcoinMiner -o http://12.34.56.78:9332 -u username -p password");
}

// main function - accept user inputs;
// -o mining node URL address -u username -p password || -h is optional for help
int main (int argc, char **argv) {

	char *url = NULL;
	char *user = NULL;
	char *pwd = NULL;
	int c;

	// check for user inputs
	while ((c = getopt (argc, argv, "ho:u:p:")) != -1) {
		switch (c) {
			case 'h':
				use();
				return 0;
			case 'o':
				url = optarg;
				break;
			case 'u':
				user = optarg;
				break;
			case 'p':
				pwd = optarg;
				break;
			default:
				use();
				abort();
		}
	}

	// if user inputs are missing, warn user and terminate program
	if (url == NULL || user == NULL || pwd == NULL) {
		use();
		return 0;
	}

	// otherwise continue program
	int burstSize = 16; // CPU stream must be a multiple of 16 bytes
	int intervals = 16; // number of hashing intervals per block header
	int pipelines = BitcoinMiner_pipelines; // number of pipelines in kernel

	// round up the size of dummy CPU output stream
	uint64_t size = pow(2, 32) / intervals / pipelines;
	while (size % burstSize != 0) {
		++size;
	}

	// allocate memory for dummy CPU output stream
	uint8_t *output = calloc(size, sizeof(uint8_t));

	// allocate memory for mapped RAM (contains pairs of matches and nonces)
	uint64_t mappedRam[pipelines][8];
	for (int p = 0; p < pipelines; p++) {
		for (int r = 0; r < 8; r++) {
			mappedRam[p][r] = 0;
		}
	}

	// add headers of HTTP requests to an existing data structure
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	// initialize thread identifier
	pthread_t pth;

	// concatenate user's username and password into one string
	char userpwd[256];
	char *colon = ":";
	strcpy(userpwd, user);
	strcat(userpwd, colon);
	strcat(userpwd, pwd);

	// add user inputs to thread data
	struct threadStruct threadData;
	threadData.url = url;
	threadData.userpwd = userpwd;

	// initialize CURL handle and CURL status code
	CURL *CURL;
	CURLcode CURLcode;

	// data structure - content of HTTP response
	struct responseStruct response;
	// a string as part of HTTP request to fetch work for mining
	char *getwork = "{\"method\": \"getwork\", \"params\": [], \"id\": 1}";
	// data structure - json objects for parsing contents of HTTP responses
	struct json_object *json, *jsonResult, *jsonData, *jsonMidstate, *jsonTarget;

	// strings to parse work for bitcoin miner
	char *dataString = "";
	char *midsString = "";
	char *targString = "";

	// arrays to prepare work for bitcoin miner
	char temp[9];
	uint base;
	uint data[3];
	uint midstate[8];
	uint target[8];

	// data structure - time snapshots
	struct timeval start, stop, diff;

	// variables to forward matching nonce to a thread function
	uint nonce;
	char nonceString[8];

	// statistic variables - elapsed time and hash rate
	long double hashTime;
	long double hashRate;

	// announce the start of bitcoin miner
	puts("");
	puts("Starting Bitcoin Miner ...");
	puts("");

	// program will run in infinite loop
	while(1) {

		// initialize CURL handle
		CURL = curl_easy_init();

		// if CURL handle initialization succeeds, continue
		if (CURL) {

			// allocate memory for the content of HTTP response
			response.content = malloc(1);
			response.size = 0;

			// create a HTTP request to fetch work for bitcoin miner
			curl_easy_setopt(CURL, CURLOPT_CUSTOMREQUEST, "POST");
			curl_easy_setopt(CURL, CURLOPT_URL, url);
			curl_easy_setopt(CURL, CURLOPT_USERPWD, userpwd);
			curl_easy_setopt(CURL, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			curl_easy_setopt(CURL, CURLOPT_NOSIGNAL, 1);
			curl_easy_setopt(CURL, CURLOPT_TIMEOUT, 3);
			curl_easy_setopt(CURL, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(CURL, CURLOPT_POSTFIELDS, getwork);
			curl_easy_setopt(CURL, CURLOPT_WRITEFUNCTION, getInMemory);
			curl_easy_setopt(CURL, CURLOPT_WRITEDATA, (void *) &response);

			// send HTTP request to the mining node
			CURLcode = curl_easy_perform(CURL);
			curl_easy_cleanup(CURL);

			// if fetching work succeeds, continue
			if (CURLcode == CURLE_OK) {

				// reset counter of connection errors
				errorCount = 0;

				// parse the content of HTTP response (work for bitcoin miner)
				json = json_tokener_parse(response.content);
				json_object_object_get_ex(json, "result", &jsonResult);
				json_object_object_get_ex(jsonResult, "data", &jsonData);
				json_object_object_get_ex(jsonResult, "midstate", &jsonMidstate);
				json_object_object_get_ex(jsonResult, "target", &jsonTarget);

				dataString = (char *) json_object_to_json_string(jsonData);
				midsString = (char *) json_object_to_json_string(jsonMidstate);
				targString = (char *) json_object_to_json_string(jsonTarget);

				// convert the content of HTTP response into numbers
				for (int i = 0; i < 3; i++) {
					strncpy(temp, dataString + 129 + i * 8, 8);
					temp[8] = '\0'; // null-terminator must be added for a proper conversion
					data[i] = swap((uint) strtoul(temp, 0, 16));
				}

				for (int i = 0; i < 8; i++) {
					strncpy(temp, midsString + 1 + i * 8, 8);
					temp[8] = '\0'; // null-terminator must be added for a proper conversion
					midstate[i] = swap((uint) strtoul(temp, 0, 16));
				}

				for (int i = 0; i < 8; i++) {
					strncpy(temp, targString + 1 + i * 8, 8);
					temp[8] = '\0'; // null-terminator must be added for a proper conversion
					target[7-i] = swap((uint) strtoul(temp, 0, 16));
				}

				// get the starting time snapshot of DFE run
				gettimeofday(&start, NULL);

				// DFE will run for several hashing intervals
				for (int i = 0; i < intervals; i++) {

					// nonce base updates after each DFE run
					base = i * pipelines * (uint) size;

					// SLiC interface
					BitcoinMiner(size, base, data, midstate, target, output,
						mappedRam[0], mappedRam[0], mappedRam[1], mappedRam[1], mappedRam[2], mappedRam[2]);

					// check for matching nonces after each DFE hashing interval
					for (int p = 0; p < pipelines; p++) {
						for (int r = 0; r < 8; r += 2) {

							// if match indicator is positive ...
							if (mappedRam[p][r] == 1) {

								// wait for any existing threads to finish copying strings into memory
								while(!semaphore) { }
								semaphore = false;

								// prepare strings for a thread function
								threadData.string = dataString;
								nonce = (uint) mappedRam[p][r+1];
								sprintf(nonceString, "%08x", swap(nonce));
								threadData.nonce = nonceString;

								// create and detach a thread to handle sending share independently
								pthread_create(&pth, NULL, sendShare, (void *) &threadData);
								pthread_detach(pth);

								// reset contents of current mapped RAM instance
								mappedRam[p][r] = 0;
								mappedRam[p][r+1] = 0;

							}

						}

					}

				}

				// get the ending time snapshot of DFE run
				gettimeofday(&stop, NULL);

				// calculate and print out DFE hash rate
				timersub(&stop, &start, &diff);
				hashTime = (long double) diff.tv_sec + (long double) diff.tv_usec / 1000000;
				hashRate = pow(2, 32) / hashTime / 1000000;
				printf("%.0Lf Mhash/s\n", hashRate);

				// wait for any existing threads to finish copying strings into memory
				while(!semaphore) { }

				// free json objects to avoid memory leaks
				json_object_put(json);

			}

			// if fetching work fails, print out instructions for user ...
			else if (CURLcode == CURLE_URL_MALFORMAT || CURLcode == CURLE_COULDNT_RESOLVE_HOST) {
				puts("Please specify a valid bitcoin mining node's URL address (-o http://ip:port");
				return 0;
			}

			// ... or print out errors
			else {
				errorCount++;
				if (errorCount == 1) fprintf(stderr, "Work not fetched\n");
				else if (errorCount == 100) fprintf(stderr, "Trouble with the chosen bitcoin mining node\n");
			}

			// free memory to avoid memory leaks
			free(response.content);
		}

		// if CURL handle initialization fails, print out error
		else {
			fprintf(stderr, "ERROR: Trouble with CURL library\n");
		}

	}

	// end of infinite loop, shall never be reached
	free(output);
	return 0;

}
