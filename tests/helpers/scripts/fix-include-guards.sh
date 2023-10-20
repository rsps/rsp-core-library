#!/bin/bash
#
# Script to traverse all header files and update include guards to a common style.

DIR=$1
PREFIX="RSP_CORE_LIB_"

FILES=$(find "$DIR" -name "*.h" -type f)

for f in $FILES
do
  tmp=${f/include\//}
  printf "Processing %-50s" "$tmp"
  tmp=$(echo "$tmp" | perl -pe 's/([a-z0-9])([A-Z])/$1_$2/g') # Convert camelCase to camel_Case
  tmp=$(echo "$tmp" | perl -pe 's/([A-Z])([A-Z][a-z0-9])/$1_$2/g') # Convert ALLCaps to ALL_Caps
  tmp=${tmp//\//_}
  tmp=${tmp//./_}
  tmp=${tmp//__/_}
  GUARD=${PREFIX}${tmp^^}
  echo "${GUARD}"

  perl -0777 -plE "print if \$t ||= s/#ifndef\s+([A-Z_]+)\n#define \1\n(.*)\n#endif\s*(\/\/\s*\1|\/\*\s*\1\s*\*\/)/#ifdef $GUARD\n#define $GUARD\n\2\n#endif \/\/ $GUARD/s }{ exit 1 if !\$t" "$f"
  retVal=$?
  if [ $retVal -ne 0 ]; then
      echo "No match in $f"
  fi
done
