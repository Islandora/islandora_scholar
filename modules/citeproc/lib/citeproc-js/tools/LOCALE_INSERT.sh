#!/bin/bash

set -e

cd $(dirname $0)
cd ..


if [ "$1" == "" ]; then
  echo Needs a first argument.  Should be a string, the last occurence of which will trigger the inserts.
  exit 1
else
  INSERT_TRIGGER_FRAG="$1"
fi

if [ "$2" == "" ]; then
  echo Needs a second argument.  Should be a string contained in the inserted chunk, that does not occur anywhere else in the locale files.
  exit 1
else
  ADDED_FRAG="$2"
  echo frag: "${ADDED_FRAG}"
fi

if [ "$3" == "" ]; then
  echo Needs a third argument.  Should be a path to a file containing a chunk of XML to be inserted.
  exit 1
fi

if [ -f ./tools/$3 ]; then
  echo found $3 go figure
  CHUNK_TO_ADD=./tools/$3
else
  echo Third argument must be a file with a chunk of XML to add to the locales
  exit 1
fi

for i in $(find ./locale -name "*.xml"); do
  if [ $(grep -c "${ADDED_FRAG}" $i) -gt 0 ]; then
    echo Skipping $i
    continue
  fi
  #echo "Insert trigger frag: " $INSERT_TRIGGER_FRAG
  LINE=$(cat $i \
     | sed "0,/.*${INSERT_TRIGGER_FRAG}.*/{p;};d" | wc -l)
  echo $i $LINE
  echo "  splitting at line: $LINE"
  #echo $((LINE--)) > /dev/null
  head -n ${LINE} $i > ${i}.head
  echo $((LINE++)) > /dev/null
  tail -n +${LINE} $i > ${i}.tail
  cat ${i}.head ${CHUNK_TO_ADD} ${i}.tail > ${i}.new
  mv ${i}.new ${i}
  rm ${i}.tail
  rm ${i}.head
done
