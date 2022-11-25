echo "Content-type: text/html"
echo ""
echo ""
echo "Uploaded file size: $HTTP_CONTENT_LENGTH"
#echo "Request Method: $REQUEST_METHOD"
#env

IFS=\; read CTYPE B <<< "$CONTENT_TYPE"
IFS=\= read NAME BOUNDARY <<< "$B"

echo "CTYPE: $CTYPE"
#echo "BOUNDARY: $BOUNDARY"

if [ "$REQUEST_METHOD" -ne "POST" ]; then
    echo "Wrong Request Method"
    exit -401
fi

if [ "$CTYPE" -ne "multipart/form-data" ]; then
    echo "Wrong Content-Type"
    exit -401
fi

while read LINE;
do
    if [[ $LINE == *"$BOUNDARY"* ]]; then
        read DISPOSITION

        IFS=\; read DISPOSITION NAME FILENAME <<<"$DISPOSITION"
        if [[ $DISPOSITION != "Content-Disposition: form-data" ]]; then
            echo "Wrong disposition";
            exit -400
        fi

        # Trim whitespace
        NAME=${NAME//[$'\t\r\n ']}
        FILENAME=${FILENAME//[$'\t\r\n ']}
        
        if [[ $NAME == "name=\"filename\"" ]]; then
            read EMPTYLINE
            read DATA
            echo "filename: $DATA"
        elif [[ $NAME == "name=\"filedata\"" ]]; then
            read DATA
            echo "filedata: $FILENAME; $DATA"
            read EMPTYLINE
            cp -b /dev/stdin ../upload.png
            SKIP=$(grep -oba -- "$BOUNDARY" ../upload.png |cut -d ':' -f 1)
            truncate -c -s $(($SKIP - 4)) ../upload.png
            break
        fi
    fi
done

#hexdump -C /dev/stdin |head -n 20


