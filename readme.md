# EmPOWER eNB protocols project

The EmPOWER eNB protocols is a library containing the protocols definitions and parsers used to format or decode messages exchanged with the controller. This component is usually included by the eNB Agent to correctly send and receive messages from EmPOWER Runtime application.

### Compatibility
This software has been developed and tested for Linux.

### Pre-requisites
In order to successfully build EMProto you need:
* Linux standard build suite (GCC, LD, AR, etc...).

In any Linux distribution (following the example for Ubuntu distribution), this can be accomplished using the terminal with the following command:

`sudo apt-get install build-essential`

You will need to log-in or perform the operation with an user having rights to install new software.

### Build from source
Building the protocols is quick and easy. The standard procedure assume that you want to install the software and the necessary headers in the system default directories `/usr/include/emage` for headers, and `/usr/lib` for the library shared objects. You can always personalize such installation by modifying the `INCLDIR` and `INSTDIR` variable present in the Makefile present in the root directory of the repository.

After having setup the required paths for installation, run (in the root directory) the following command to build the software:

`make`

### Install
As previously said, the software will be installed in your system alongside other libraries. To change this behavior you can modify the variables present in the makefile (see build instruction).

After having built the software, to install it run the following command:

`sudo make install`

Again, as for any other installation, you will need superuser rights in order to perform such changes on the filesystem.

### Uninstalling
You can always undo installation operations sometimes. In order to do so a "cleaning" operation is also present as a makefile option. This uninstalling steps revert the modification done by the installation stage, completely cleaning your Linux from the software:

`sudo make uninstall`

Again, as for any other installation, you will need superuser rights in order to perform such changes on the filesystem.

### License
The code is released under the Apache License, Version 2.0.
