#!/bin/bash

HTML_VAR="config_html"
SOURCE_HTML="config.html"
SOURCE_JS="config.js"
DESTINATION_JS="../src/js/pebble-js-app.js"
COMMENT="// File generated automatically from files in /config. Do not change this file as it will be replaced"

echo $COMMENT > $DESTINATION_JS
echo var $HTML_VAR = \"$(sed -e 's/"/\\"/g' -e 's/^[ \t]*//' $SOURCE_HTML | tr -d '\n')\"\; $'\n' >> $DESTINATION_JS
cat $SOURCE_JS >> $DESTINATION_JS

