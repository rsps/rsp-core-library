#!/bin/bash
#
# Script to traverse all header files and update include guards to a common style.

DIR=$1
DEFAULT_PREFIX="RSP_CORE_LIB_"
PREFIX="${2:-$DEFAULT_PREFIX}"

FILES=$(find "$DIR" -name "*.h" -type f -not -path "*/build/*" -not -path "*/.?*")

for f in $FILES
do
  tmp=${f/include\//}
  printf "Processing %-50s" "$tmp"
  tmp=$(echo "$tmp" | perl -pe 's/([a-z0-9])([A-Z])/$1_$2/g') # Convert camelCase to camel_Case
  tmp=$(echo "$tmp" | perl -pe 's/([A-Z])([A-Z][a-z0-9])/$1_$2/g') # Convert ALLCaps to ALL_Caps
  tmp=${tmp//\//_} # Convert path delimiters to _
  tmp=${tmp//./_}  # Convert . to _
  tmp=${tmp//-/_}  # Convert - to _
  tmp=${PREFIX}${tmp^^}
  tmp=${tmp//__/_} # Combine __ to _
  tmp=${tmp//__/_} # Combine __ to _ again. Bash string substitution is not global.
  tmp=${tmp//__/_} # Combine __ to _ again. Bash string substitution is not global.
  GUARD=$tmp
  echo "${GUARD}"

  regex="s/#ifndef\s+([A-Za-z0-9_-]+)\n#define \1\n(.*)\n#endif\s*(\/\/\s*\1|\/\*\s*\1\s*\*\/|\n)/#ifndef $GUARD\n#define $GUARD\n\2\n#endif \/\/ $GUARD/s"

  perl -0777 -pE "if ($regex) { exit 0 } else { exit 1 }" "$f"
  retVal=$?
  if [ $retVal -ne 0 ]; then
      echo "ERROR: No match in $f"
  else
    perl -0777 -pi -plE "$regex" "$f"
  fi
done
