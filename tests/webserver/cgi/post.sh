echo "Content-type: text/html"
echo ""
echo ""
echo "Content length: $CONTENT_LENGTH"
echo "Request Method: $REQUEST_METHOD"

if [ "$REQUEST_METHOD" = "POST" ]; then
    if [ "$CONTENT_LENGTH" -gt 0 ]; then
      in_raw="$(cat)"
      echo "Body: $in_raw"
#      boundary=$(echo -n "$in_raw" | head -1 | tr -d '\r\n');
#      filename=$(echo -n "$in_raw" | grep --text --max-count=1 -oP "(?<=filename=\")[^\"]*");
#      file_content=$(echo -n "$in_raw" | sed '1,/Content-Type:/d' | tail -c +3 | head --lines=-1 | head --bytes=-4  );
#      echo "boundary: $boundary"
#      echo "filename: $filename"
#      echo "file_content: $file_content"
    fi
fi

#env
