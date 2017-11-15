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
    url="https://en.wikipedia.org/wiki/List_of_current_NBA_team_rosters",
    output_name="output"
)


# Move all CSV folders into a single 'output' folder
os.makedirs('output')
shutil.move('./test', './output')

