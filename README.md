<h1>WEB SCRAPPER</h1>

<p>A web scraper built in C that connects to a website and extracts all links and images.</p>

<br />

<h2>1. INSTALL DEPENDENCIES</h2>

<p>Open your terminal and run this command to install the required libraries:</p>

<pre>sudo apt-get install libcurl4-openssl-dev libgumbo-dev</pre>

<br />

<h2>2. COMPILE THE PROJECT</h2>

<p>Run this command in your terminal to build the executable binary:</p>

<pre>make</pre>

<br />

<h2>3. RUN THE SCRAPER</h2>

<p>Execute the program by passing the website URL you want to scrape:</p>

<pre>./scraper https://example.com</pre>

<br />

<h2>REPOSITORY FILES</h2>
<ul>
    <li><b>main.c</b> - Source code for fetching and parsing HTML.</li>
    <li><b>Makefile</b> - Handles compilation flags and library linking.</li>
    <li><b>README.md</b> - This documentation file.</li>
</ul>
