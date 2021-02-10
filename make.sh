#!/bin/bash

<< 'Flags'
    # ARG[0]
    * -C : Compile
    * -R : Run
    * -A : Compile and Run
    * -T : Compile and Run Unit Tests
    * -Mov_Ciupak : Move Dir to Ciupak Home Directory
    * -Mov_Kemmet : Move Dir to Ciupak Home Directory

    # ARGS[1]
    * $COMPILEFLAGS

    # AGRS[2]
    * $RUNFLAGS
Flags


args=("$@")

## Helper Functions

function compile() {
    ./os/make
    ./apps/make
}

function run() {
    dlxsim -x ./os/work/os.dlx.obj -a -u ./apps/work/userprog.dlx.obj
}

## Main - Flag Control
if [ ${args[0]} = '-C' ]
then
    echo 'C*O*M*P*I*L*I*N*G'
    compile

elif [ ${args[0]} = '-R' ]
then
    echo 'Is Your Kernel R*U*N*N*I*N*G? Well.....you better go catch it!'
    run

elif [ ${args[0]} = '-A' ]
then
    echo 'Compiling and Running'
    compile
    run

elif [ ${args[0]} = '-T' ]
then
    echo 'Running Tests'
    echo 'Test 1 - Hello World'

elif [ ${args[0]} = '-Mov' ]
then
    echo 'This project is currently at:' $(pwd)
    if [ ${args[1]} = 'ciupak' ]
    then
        pwd
    elif [ ${args[0]} = '-Mov' ]
    then
        pwd
    fi
# elif [ ${args[0]} = '-Mov' ]
# then
#     echo Lets Go Home

else
    echo "Total Arguments:" $#
    echo "All Arguments values:" $@
fi
