#!/bin/bash

## To Run the first test make.sh -R example/ run_test2

<< 'Flags'
    # ARG[0]
    * -C : Compile
    * -R : Run
    * -A : Compile and Run
    * -T : Compile and Run Unit Tests
    * -Mov : Move Dir From/To $USER and /tmp/
    * -Clean: clean things so you can pull

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
    dlxsim -D p -x ./os/work/os.dlx.obj -a D p -u ./apps/work/userprog.dlx.obj
}

function run_example(){

    echo 'Compiling and running example program'
    make clean -C ./os/
    make -C ./os/
    make -C ./apps/example/
    # ee469_fixterminal
    make run -C ./apps/example/
}

function run_program(){
    if [ ${args[1]} ]
    then
      echo '---STARTING---'
      echo ${args[1]}

      echo '---CLEANING---'
      make clean -C ./os/
      make -C ./os/
      make clean -C ./apps/${args[1]}/
      echo '---COMPILING---'
      make -C ./apps/${args[1]}/
      ee469_fixterminal
      echo '---RUNNING---'
      make ${args[2]} -C ./apps/${args[1]}/
    else
      echo 'Nothing was Run, No Argment was Added'
    fi
}

function clean_all(){
    make clean -C ./os
    make clean -C ./apps/example/
    make clean -C ./apps/prio_test/
    make clean -C ./apps/q2/

}

function move() {
    if [ $(pwd) = /tmp/$USER/ee469 ]
    then
        cp -r . /home/shay/a/$USER/469ECE/DLXOS 2>/dev/null
        echo 'This project has been copied to:' /home/shay/a/$USER/469ECE/DLXOS
    else
        rm -rf /tmp/$USER/ee469/*
        cp -r . /tmp/$USER/ee469/ 2>/dev/null
        echo 'This project has been copied to:' /tmp/$USER/ee469/
    fi
}

## Main - Flag Control
case ${args[0]} in

  -C)
    echo 'C*O*M*P*I*L*I*N*G'
    compile
    ;;
  -R)
    # echo 'Is Your Kernel R*U*N*N*I*N*G? Well.....you better go catch it!'
    run_program
    ;;
  -A)
    echo 'Compiling and Running'
    compile
    run
    ;;
  -T)
    echo 'Running Tests'
    echo 'Test 1 - Hello World'
    ;;
  -Mov)
    echo 'This project is currently at:' $(pwd)
    move
    ;;
  -c)
    echo 'cleaning all'
    clean_all
    ;;
  *)
    echo "Total Arguments:" $#
    echo "All Arguments values:" $@
    ;;
esac