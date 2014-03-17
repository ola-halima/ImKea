ImKea
=====

####HHP Interior Decor & Design (ImKea) - a fun and unique software that helps users find interior decor color themes and ideas through image process and web search.

##ImKea works in the following way:
  - User inputs any word/image related to the theme they have in mind (e.g. "summer")
  - The input is then used as a Google Images search term 
  - From the first 5 images of the results page, we extract the three most recurring colors
  - The three colors are then used another Google Images search term with "interior decor" appended to it (e.g. "green red blue interior decor")
  - The user is directed to the Google Images results page 
  
##Objectives 
  - Analyze images for dominant colors (store and map those colors to a list of 'basic' colors chosen)
  - Develop color-name mapping algorithm 
  - Use the Google Custom Search API, with C
  - Appropriate input/output for users
  - Provide users with relevant and useful results

##Dependencies

###Libraries
  - OpenCv - for image analysis (http://opencv.org/)
  - libcurl - for requests to Google Search/image downloading (http://curl.haxx.se/libcurl/)
  - Jansson - for JSON parsing of search results (http://www.digip.org/jansson/)

###Other
  - A Google Custom Search Engine - for searching for images to analyze; see "Notes" (https://developers.google.com/custom-search/docs/overview)

##Notes

###Setting Google Custom Search IDs

Because of Google quota limits, the Google Custom Search Engine ID and Google API Key are kept private from this GitHub. If you're really nice you can ask the creators for their IDs and API keys, however this is the internet and we don't expect you actually care that much. In this case, to run ImKea, you only need to first set two environment variables:

```
export IMKEA_CSEID=000%3AXXX
export IMKEA_APIKEY=XXX
```

An IMKEA\_CSEID can be obtained by creating a Google Custom Search Engine (https://www.google.com/cse/manage/all). This is also known as your "cx". Because environment variables don't always play nicely, the ":" of this identifier should be replaced by "%3A". Also ensure that image search is enabled in your CSE, or of course you won't be able to retrieve image results!

An IMKEA\_APIKEY can be obtained from the Google Developers Console (https://console.developers.google.com/). You'll need to create a new project, turn on the CSE API, and create an API Key.

Or you could just ask the developers of this project. They won't bite, I swear.

##Running

```
make
./imkea "some query"
```

For example, try:

```
./imkea "american flag"
```

##Known Issues

###Images not downloading

Occasionally an image URL parsed out of the Google Image results will somehow not point to an image that can be downloaded. Thus, the program tries to load an image that has not yet been downloaded, and fails.

###Unexpected Results

The color mapping algorithm does not always work as expected. For simple troubleshooting, check the images downloaded to the imkea-src-images/ directory to see what images the analysis was completed on. You can also change the assets/lab_db.txt to add/remove defined colors. Removing "Gray" for example, may increase your chances of finding more black, white, or other colors.

####ImKea was made as a project for CS283: Systems Programming, a course at Drexel University. Though its creators may or may not be adding to it in the future, feedback and comments are appreciated. Thank you!
