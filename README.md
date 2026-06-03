WEB SCRAPPER
A fast and lightweight web scraper built in C. It establishes a network connection to any target website, parses the raw HTML data, and instantly extracts all embedded hyperlinks and image sources.

🛠️ GETTING STARTED
1. Install System Dependencies
Before compiling, you need to install libcurl and Google's gumbo parser headers. Run this command in your terminal:

sudo apt-get install libcurl4-openssl-dev libgumbo-dev
2. Build the Executable
Compile the source code using the automation rules defined in the Makefile:

make
3. Execute the Scraper
Run the compiled binary file and pass the URL you want to inspect as a command-line argument:

./scraper https://example.com
