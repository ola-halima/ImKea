//
//  google-img.c
//  HHP Interior Decor & Design
//
//  Created by Heidi Muth, Persis Finny, Halima Olapade on 3/3/14.
//  Copyright (c) 2014 ImKea. All rights reserved.
//

#include "google-img.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cv.h>
#include <curl/curl.h>
#include <jansson.h>

#define VERBOSE 1

#define URL_MAX 256
#define RESPONSE_MAX 4096

#define INPUT_SEARCH_F "https://www.googleapis.com/customsearch/v1?searchType=image&fileType=jpg&cx=%s&key=%s&num=%d&q=%s"

#define IMKEA_OUTPUT_SEARCH 0
#define OUTPUT_SEARCH_F "https://www.google.com/search?tbm=isch&q=%s"
#define IMKEA_OUTPUT_SEARCH_F "https://www.cs.drexel.edu/~hbm25/imkea.html?q=%s"

/**
 * For query formatting, replaces ' ' with '+'
 * @param str		String to modify
 */
void replace_spaces(char** str) {
	int i;
	int len = strlen(*str);
	for (i = 0; i < len; i++) {
		char c = (*str)[i];
		if (c == ' ') {
			(*str)[i] = '+';
		}
	}
}

/**
 * Custom struct for HTTP response writing
 */
struct String { char* str; size_t len; };

/**
 * CURLOPT_WRITEFUNCTION callback handler 
 * For writing CURL response data to (String *) result.
 */
size_t write_string(void* source, size_t size, size_t nmemb, void* result) {
	size_t realSize = size * nmemb;
	struct String *s = (struct String *)result;

	s->str = realloc(s->str, s->len + realSize + 1);
	if (s->str == NULL) {
		fprintf(stderr, "Error reallocating memory.\n");
		return 0;
	}

	memcpy(&(s->str[s->len]), source, realSize);
	s->len += realSize;
	s->str[s->len] = '\0';

	return realSize;
}

/**
 * Makes a request to a URL, then returns data of the result in a string.
 * @param results		String of response received
 * @param url			URL to make request to
 * @returns			CURLcode returned by curl operation
 */
CURLcode get_results(char** results, char* url) {
	curl_global_init(CURL_GLOBAL_ALL);
	CURL * handle;
	CURLcode res;

        struct String s;
        s.len = 0;
        if ((s.str = malloc(RESPONSE_MAX)) == NULL) {
                fprintf(stderr, "Error allocating memory.\n");
                return CURLE_WRITE_ERROR;
        }
        s.str[0] = '\0';

	if (VERBOSE) {
		printf("Making request to URL: %s\n", url);
	}

	handle = curl_easy_init();

        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_string);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &s);

        CURLcode rv = curl_easy_perform(handle);

        curl_easy_cleanup(handle);

	if (VERBOSE) {
		printf("CURL Result: [%d] %s\n", rv, curl_easy_strerror(rv));
		printf("\nResponse:\n%s\n", s.str);
	}

        *results = s.str;

        return rv;
}

/**
 * Makes query for images in internally-defined search engine.
 * @param results		JSON-formatted query response
 * @param query			String to search for
 * @param nResults		Number of results to return
 * @returns			CURLcode returned by curl operation
*/
int get_image_results(char** results, char* query, int nResults) {
	
	char* cseId, *apiKey;
	if ((cseId = getenv("IMKEA_CSEID")) == NULL) {
		fprintf(stderr, "Error: IMKEA_CSEID not set. (Sample format: 00000%%3Axxxxx)\n");
		return CURLE_URL_MALFORMAT;
	}
	if ((apiKey = getenv("IMKEA_APIKEY")) == NULL) {
		fprintf(stderr, "Error: IMKEA_APIKEY not set.\n");
		return CURLE_URL_MALFORMAT;
	}
	
	replace_spaces(&query);

	char url[URL_MAX];
	sprintf(url, INPUT_SEARCH_F, cseId, apiKey, nResults, query);
	
	int rv = get_results(results, url);

	return rv;
}

/**
 * Parses a Google Custom Image Search JSON object for image URL results.
 * @param imageURLs		String array to store image URL results
 * @param nImages		Number of image URLs to extract
 * @param jsonResult		JSON object result of Google CSE query
 * @returns			Number of image URLs successfully extracted
 */
int extract_image_urls(char*** imageURLs, int* nImages, char* jsonResult) {
	json_t* root;
	json_error_t error;

	int i;

	root = json_loads(jsonResult, 0, &error);
	if (!root) {
		fprintf(stderr, "Error parsing JSON: %s\n", error.text);
		return -1;
	}	

	json_t* items;
	items = json_object_get(root, "items");
	*nImages = json_array_size(items);

	*imageURLs = malloc(*nImages*sizeof(char));

	int nImagesExtracted = 0;
	for (i = 0; i < *nImages; i++) {
		json_t* item = json_array_get(items, i);
		json_t* link = json_object_get(item, "link");
		const char* link_text = json_string_value(link);
		(*imageURLs)[i] = malloc(strlen(link_text)+1);
		strcpy((*imageURLs)[i], link_text);
		nImagesExtracted++;

		if (VERBOSE) {
			printf("Extracted image URL: %s\n", link_text);
		}
	}

	if (VERBOSE) {
		printf("Extracted %d images.\n", nImagesExtracted);
	}
	
	return nImagesExtracted;
}

/**
 * CURLOPT_WRITEFUNCTION callback handler 
 * For writing CURL response data to (FILE *) stream.
 */
size_t write_file(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/**
 * Makes a request to a URL, then returns data of the result to a file.
 * @param filename		Filename to write to
 * @param url			URL to make request to
 * @returns			0 if successful
 */
int download_image(char* filename, char* url) {
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* handle;
	FILE* file;
	CURLcode res;

	handle  = curl_easy_init();
	if (handle) {
		if((file = fopen(filename, "wb")) == NULL) {
			fprintf(stderr, "Error: could not open file \"%s\" for image download.\n", filename);
			return -1;
		}

		curl_easy_setopt(handle, CURLOPT_URL, url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, file);
		res = curl_easy_perform(handle);

		curl_easy_cleanup(handle);
		fclose(file);
	}
	if (res != 0) {
		fprintf(stderr, "Downloading image \"%s\" unsuccessful. CURL Result: [%d] %s\n", url, res, curl_easy_strerror(res));
		return -1;
	}

	if (VERBOSE) {
		printf("%s was downloaded to %s\n", url, filename);
	}

	return 0;
}

/**
 * Makes a Google Custom Image Search query.
 * @param images		Array of full paths to images found
 * @param query			What to search for
 * @param nResults		Number of results to return
 * @return			Number of images successfully returned (size of images)
 */
int get_images(char*** images, char* query, int nResults) {
        char* results;
        CURLcode r = get_image_results(&results, query, nResults);
        if (r != CURLE_OK) {
                fprintf(stderr, "Request unsuccessful: %s\n", curl_easy_strerror(r));
                return -1;
        }

        char** imageURLs;
        int nImages;
        if (extract_image_urls(&imageURLs, &nImages, results) <= 0) {
                fprintf(stderr, "Could not parse image URLS.\n");
                return -1;
        }

	*images = malloc(nImages*sizeof(char*));
	
        int i;
	int nImagesDownloaded = 0;
        for (i = 0; i < nImages; i++) {
		(*images)[i] = malloc(25*sizeof(char));
		sprintf((*images)[i], "/tmp/imkea-img%d", i);
                int r = download_image((*images)[i], imageURLs[i]);
		if (r == 0) {
			nImagesDownloaded++;
		}
        }

        return nImagesDownloaded;
}

/**
 * Formats URL for final ImKea results webpage.
 * @param query			space delimited full query to search for
 * @returns			URL of image search results
 */
char* make_imkea_query_url(char* query) {
	replace_spaces(&query);
	char* query_url = (char*) malloc(URL_MAX*sizeof(char));
	char* url_f = IMKEA_OUTPUT_SEARCH ? IMKEA_OUTPUT_SEARCH_F : OUTPUT_SEARCH_F;
	sprintf(query_url, url_f, query);
	return query_url;
}


/**
 * Opens a URL in the user's default browser.
 * @param URL			URL to open
 * @returns			0 if successful
*/
int open_webpage(char* URL) {
	// TODO: implement
	printf("%s\n", URL);
	
	return 0;
}

/**
 * Formats a query for interior design based on provided colors and opens that webpage in the user's default browser.
 * @param query			space delimited full query to search for
*/
void find_designs(char* query) {
	char* url = make_imkea_query_url(query);
	if (open_webpage(url) != 0) {
		fprintf(stderr, "Error: could not open webpage for:\n%s\n", url);
	}
}

