#!/bin/bash

<< 'Flags'
    # ARG[0]
    * -C : Compile
    * -R : Run
    * -A : Compile and Run
    * -T : Compile and Run Unit Tests
    * -Mov : Move Dir From/To $USER and /tmp/

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

function move() {
    if [ $(pwd) = /tmp/$USER/ee469 ]
    then
        cp -r . /home/shay/a/$USER/469ECE/DLXOS 2>/dev/null
        echo 'This project has been copied to:' /home/shay/a/$USER/469ECE/DLXOS
    else
        rm -rf /tmp/$USER/ee469/*
        cp -r . /tmp/$USER/ee469/ 2>/dev/null
        echo 'This project has been copied to:' /tmp/$USER/ee469
    fi
}

## Main - Flag Control
if [ ${args[0]} = '-C' ]
then
    echo 'C*O*M*P*I*L*I*N*G'
    compile

elif [ ${args[0]} = '-R' ]
then
    echo 'Is Your Kernel R*U*N*N*I*N*G? Well.....you better go catch it!'
    if [ ${args[1]} = 'example' ]
    then
        echo 'Compiling and running example program'
        make clean -C ./os/
        make -C ./os/
        make -C ./apps/example/
        ee469_fixterminal
        make run -C ./apps/example/
        
    fi

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
    move

else
    echo "Total Arguments:" $#
    echo "All Arguments values:" $@
fi
