#!/bin/bash

# Dependent style maker
# By Avram Lyon
# In the public domain. Enjoy, improve.

# Command for Sebastian to use with his CSV:
# while read line ; do echo $line | sed -e s/\",\"/\\t/g | sed -e s/\"//g | xargs -d\\t ./make-dependent.sh bmj; done < bmj-styles.csv

if [ "$#" -lt "3" ] 
then
	echo Usage: depend parent-style \"New Style\" new-style citation-format field.
	echo Does not support multiple fields. Sorry.
	echo But at least the last two arguments are optional. That\'s at least something.
	exit
fi

trim() { echo $1; }

declare -r HEAD=$(cat <<'HEREDOC'
<?xml version="1.0" encoding="utf-8"?>
<style xmlns="http://purl.org/net/xbiblio/csl" version="1.0">
   <info>
      <title>%s</title>
      <id>http://www.zotero.org/styles/%s</id>
      <link href="http://www.zotero.org/styles/%s" rel="independent-parent"/>
HEREDOC
)
declare -r FOOT='
      <updated/>
   </info>
</style>'

declare -r CF='      <category citation-format="%s"/>
'
declare -r FI='      <category field="%s"/>'

TARGET=$(trim $3;)

if [ -f $TARGET.csl ]
then
	echo Target file $TARGET.csl already exists.
	exit
fi

printf "$HEAD\n" "$2" "$TARGET" "$1" > $TARGET.csl

# citation-format specified
if [ "$#" -gt "3" ]
then
	printf "$CF" $4 >> $TARGET.csl
fi

# field specified
if [ "$#" -gt "4" ]
then
	printf "$FI" $5 >> $TARGET.csl
fi

printf "$FOOT" >> $TARGET.csl

echo Created $TARGET.csl for $2, dependent on style $1: $4, $5
