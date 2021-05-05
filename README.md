# RUNNER
Repository for my Bachelor thesis

* Tool to read data from a Polar watch(rc3gps) via USB and give user some useful information.
* Mainly a learning experience.

# DEPENDENCIES
* libusb (1.0.23)

# SETUP
- clone repo
- make sure make, gcc, libusb are installed
- edit INSTALL_PATH in makefile to be suitable to you(make sure the path exists)
```
make
make install
```
Make sure the directory is in $PATH, can be done with ```export PATH=$PATH:[path of dir]```
- if it needs deleting ```make clean``` WARNING: this also removes the binary from the installed directory, if this is not wanted, remove the second argument from makefile under make clean
- copy over the test files, can also create new ones.
```
mkdir $HOME/.config/runner
cp dotfiles/* $HOME/.config/runner
```

