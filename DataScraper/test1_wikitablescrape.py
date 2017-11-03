"""Test the wikitablescrape script on four articles."""

import os
import shutil

import wikitablescrape

# Delete previous output folder if it exists, then create a new one
try:
    shutil.rmtree('output')
except FileNotFoundError:
    pass

wikitablescrape.scrape(
    url="http://www.basketball-reference.com/draft/NBA_2014.html",
    output_name="test"
)


# Move all CSV folders into a single 'output' folder
os.makedirs('output')
shutil.move('./test', './output')

