echo "Content-type: text/html"
echo ""
echo ""
echo "Uploaded file size: $HTTP_CONTENT_LENGTH"
echo "Request Method: $REQUEST_METHOD"
env
echo ""

hexdump -C /dev/stdin |head -n 20

#cp -b /dev/stdin ../upload.png

