#!/bin/bash

HTML_VAR="config_html"
SOURCE_HTML="config/config.html"
SOURCE_JS="config/config.js"
DESTINATION_DIR="src/js/"
DESTINATION_JS="pebble-js-app.js"
COMMENT="// File generated automatically from files in /config. Do not change this file as it will be replaced"

# Create js directory in src.
mkdir -p $DESTINATION_DIR
# Write the comment to the file, overwriting its contents.
echo $COMMENT > $DESTINATION_DIR$DESTINATION_JS
# Append the escaped HTML to the file.
echo var $HTML_VAR = \"$(sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/^[ \t]*//' -e 's/$/\\n/' $SOURCE_HTML | tr -d "\n")\"\; >> $DESTINATION_DIR$DESTINATION_JS
# Append the rest of the JS to the file.
cat $SOURCE_JS >> $DESTINATION_DIR$DESTINATION_JS

