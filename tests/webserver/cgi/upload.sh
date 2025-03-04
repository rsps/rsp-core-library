echo "Content-type: text/html"
echo ""
echo "Uploaded file size: $HTTP_CONTENT_LENGTH"
echo "Request Method: $REQUEST_METHOD"
echo "Filename: $HTTP_X_FILENAME"
#env

cp -b /dev/stdin "../$HTTP_X_FILENAME"
