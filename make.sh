#!/bin/bash

'''
ARGS[0] Flags:
* -C : Compile
* -R : Run
* -A : Compile and Run
* -T : Compile and Run Unit Tests
* -Mov_Ciupak : Move Dir to Ciupak Home Directory
* -Mov_Kemmet : Move Dir to Ciupak Home Directory

ARGS[1] Flags:
* $COMPILEFLAGS

ARGS[2] Flags:
* $RUNFLAGS
'''

args=("$@")

## Flag Control
if [ ${args[0]} = '-C' ]
then
    echo Lets Compile
elif [ ${args[0]} = '-R' ]
then
    echo Lets Run
elif [ ${args[0]} = '-A' ]
then
    echo Lets Compile and Run
elif [ ${args[0]} = '-T' ]
then
    echo Lets Compile and Run Our Tests
elif [ ${args[0]} = '-Mov_Ciupak' ]
then
    echo Lets Go Home
elif [ ${args[0]} = '-Mov_Kemmet' ]
then
    echo Lets Go Home
else
    echo "Total Arguments:" $#
    echo "All Arguments values:" $@
fi
