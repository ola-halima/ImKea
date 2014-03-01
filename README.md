ImKea
=====

HHP Interior Decor & Design - software that helps users find interior decor color themes and ideas through
image process and web search.

HHP Interiot Decor & Design works in the following way:
  - User inputs any word/image related to the theme they have in mind (e.g. "summer")
  - The input is then used as a Google Images search term 
  - From the first 5 images of the results page, we extract the three most recurring colors
  - The three colors are then used another Google Images search term with "interior decor" appended to it (e.g. blue white green interior decor)
  - The user is directed to the Google Images results page 
  
Objectives 
  - Analyze images for dominant colors (store and map those colors to a list of 'basic' colors)
  - Develop color-name mapping algorithm (What constitutes red? How many color names should we map to? Which ones?)
  - Use the Google Custom Search API, with C
  - Appropriate input/output for users? webpage interface? Or redirect from console? 
  - Provide users with relevant and useful results
