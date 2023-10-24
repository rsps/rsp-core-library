#!/bin/bash
#
# Script to traverse all header files and update include guards to a common style.

DIR=$1
PREFIX="RSP_CORE_LIB_"

FILES=$(find "$DIR" -name "*.h" -type f -not -path "*/build/*" -not -path "*/.?*")

for f in $FILES
do
  tmp=${f/include\//}
  printf "Processing %-50s" "$tmp"
  tmp=$(echo "$tmp" | perl -pe 's/([a-z0-9])([A-Z])/$1_$2/g') # Convert camelCase to camel_Case
  tmp=$(echo "$tmp" | perl -pe 's/([A-Z])([A-Z][a-z0-9])/$1_$2/g') # Convert ALLCaps to ALL_Caps
  tmp=${tmp//\//_}
  tmp=${tmp//./_}
  tmp=${tmp//-/_}
  tmp=${tmp//__/_}
  tmp=${tmp//__/_}
  GUARD=${PREFIX}${tmp^^}
  echo "${GUARD}"

  regex="s/#ifndef\s+([A-Z0-9_]+)\n#define \1\n(.*)\n#endif\s*(\/\/\s*\1|\/\*\s*\1\s*\*\/|\n)/#ifndef $GUARD\n#define $GUARD\n\2\n#endif \/\/ $GUARD/s"

  perl -0777 -pE "if ($regex) { exit 0 } else { exit 1 }" "$f"
  retVal=$?
  if [ $retVal -ne 0 ]; then
      echo "ERROR: No match in $f"
  else
    perl -0777 -pi -plE "$regex" "$f"
  fi
done
