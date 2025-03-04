echo "Content-type: text/html"
echo ""
echo "Content Length: $HTTP_CONTENT_LENGTH"
#echo "Request Method: $REQUEST_METHOD"
#env

IFS=\; read CTYPE B <<< "$CONTENT_TYPE"
IFS=\= read NAME BOUNDARY <<< "$B"

echo "CTYPE: $CTYPE"
#echo "BOUNDARY: $BOUNDARY"

if [[ $REQUEST_METHOD != "POST" ]]; then
    echo "Wrong Request Method: '$REQUEST_METHOD'"
    exit -401
fi

if [[ $CTYPE != "multipart/form-data" ]]; then
    echo "Wrong Content-Type: '$CTYPE'"
    exit -401
fi

ACTUAL_FILENAME="upload.png"

while read LINE;
do
    if [[ $LINE == *"$BOUNDARY"* ]]; then
        read DISPOSITION

        IFS=\; read DISPOSITION NAME FILENAME <<<"$DISPOSITION"
        if [[ $DISPOSITION != "Content-Disposition: form-data" ]]; then
            echo "Wrong disposition: '$DISPOSITION'";
            exit -400
        fi

        # Trim whitespace
        NAME=${NAME//[$'\t\r\n ']}
        FILENAME=${FILENAME//[$'\t\r\n ']}
        
        if [[ $NAME == "name=\"filename\"" ]]; then
            read EMPTYLINE
            read DATA
            echo "filename: $DATA"
            ACTUAL_FILENAME=${DATA//[$'\t\r\n ']}
        elif [[ $NAME == "name=\"filedata\"" ]]; then
            read DATA
            echo "filedata: $FILENAME; $DATA"
            read EMPTYLINE
            cp -b /dev/stdin "../$ACTUAL_FILENAME"
            SKIP=$(grep -oba -- "$BOUNDARY" "../$ACTUAL_FILENAME" |cut -d ':' -f 1)
            truncate -c -s $(($SKIP - 4)) "../$ACTUAL_FILENAME"
            stat --format="file-size: %s" "../$ACTUAL_FILENAME"
            break
        fi
    fi
done

#hexdump -C /dev/stdin |head -n 20


