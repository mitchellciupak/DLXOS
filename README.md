# DLXOS-LAB1-ECE469
#### DLXOS Introduction at https://engineering.purdue.edu/~ee469/labs_2021/lab1.html
#### Due: 2021-02-02 23:59 EST

## WARNING
* before you exit from ssh session: Please make sure that you copy the contents of your tmp folder into your home for backup

        cp -r /tmp/$USER/ee469 ~/

* When you login again to ecegrid, you can restore your work to tmp as:

        cp -r ~/ee469 /tmp/$USER/"

## File Structure
    ./
        apps/: user programs go here
        bin/: put any executable DLX files here
        include/:
            header files for user programs are here
            os/: header files for the operating system are here
        lib/: precompiled object files for user programs (i.e. usertraps.o) go here
        os/