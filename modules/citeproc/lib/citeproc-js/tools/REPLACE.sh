#!/bin/bash

cd $(dirname $0)

cd ..


if [ "$1" == "--action" ]; then
    ACTION="1"
    shift
fi

if [ "$2" == "" ]; then
    echo Must give two values.  First is the value to replace, the second is the replacement value.
    exit 1
fi
if [ "$3" != "" ]; then
    echo Must give ONLY two values.  First is the value to replace, the second is the replacement value.
    exit 1
fi


if [ "$2" == "NOTHING" ]; then
    ARG2=""
else
    ARG2="$2"
fi
for i in ./tests/std/humans/*.txt; do
    if [ $(grep -c "${1}" $i) -gt 0 ]; then
        if [ "$ACTION" == "1" ]; then
            #cat $i | sed -e "s~$1~${ARG2}~g" > $i.NEW
            cat $i | sed -e "s~${1}~${ARG2}~g" > $i.NEW
	    mv $i.NEW $i
        else
	    echo "-----OLD-----"
	    grep "${1}" $i
	    echo "-----NEW-----"
	    grep "${1}" $i | sed -e "s~${1}~${ARG2}~g"
	fi
    fi
done
