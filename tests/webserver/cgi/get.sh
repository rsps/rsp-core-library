#!/bin/bash
#
# \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
# \author      Steffen Brummer
#
# CGI script to dump all request parameters
#
export LC_NUMERIC=C
REQUEST_TIME=$EPOCHREALTIME
export LC_TIME=C

IFS=$'\r\n' BODY="$(cat)"
OUT="../cgi.log"

if [[ $QUERY_STRING == "" ]]; then
  QUERY_STRING="null"
fi

if [[ $BODY == "" ]]; then
  BODY="null"
fi

{
  printf '{"time":"%s","method":"%s","uri":"%s","query":"%s",' "$REQUEST_TIME" "$REQUEST_METHOD" "$PATH_INFO" "$QUERY_STRING"
  printf '"type":"%s","length":%d,"token":"%s","body":' "$CONTENT_TYPE" "$HTTP_CONTENT_LENGTH" "$HTTP_AUTHORIZATION"
  printf "%s}\n" "$BODY"
#  env
} >> $OUT

printf "HTTP/1.1 200 Ok\n"
printf "Content-Type: application/json\n"
printf "X-HttpsTime: %f\n" "$EPOCHREALTIME"
printf "X-RequestTime: %f\n" "$REQUEST_TIME"
printf "\n"
printf "{\n"
printf '  "time": "%s",\n' "$REQUEST_TIME"
printf '  "request-uri": "%s",\n' "$REQUEST_URI"
printf '  "method": "%s",\n' "$REQUEST_METHOD"
printf '  "path": "%s",\n' "$PATH_INFO"
printf '  "query": "%s",\n' "$QUERY_STRING"
printf '  "type": "%s",\n' "$CONTENT_TYPE"
printf '  "length": %d,\n' "$HTTP_CONTENT_LENGTH"
printf '  "token": "%s",\n' "$HTTP_AUTHORIZATION"
printf '  "body": "%s"\n' "$BODY"
printf "}\n"

