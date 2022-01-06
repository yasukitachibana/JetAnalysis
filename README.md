# JetAnalysis by Yasuki Tachibana

The code to analyze Jet Observerbles for MC calculations

## Installation
cern ROOT, FastJet and FastJet Contrib are requreid. 

### ROOT
```
$ conda config --set channel_priority strict
$ conda create -c conda-forge --name [YOUR_CONDA_ENVIRONMENT_FOR_ROOT] root
```

### FastJet

1. Download
    ```
    $ mkdir -p [PATH_OF_DIRECTORY_FOR_FASTJET]
    $ cd [PATH_OF_DIRECTORY_FOR_FASTJET]
    $ wget http://fastjet.fr/repo/fastjet-3.4.0.tar.gz
    $ tar zxvf fastjet-3.4.0.tar.gz
    $ cd fastjet-3.4.0
    ```

1. Configure 

    On Mac (Intel or M1 with Rosetta 2)
    ```
    $ ./configure --prefix=$PWD/../fastjet-install
    ```


    On Wayne State Grid
    ```
    $ ./configure --prefix=$PWD/../fastjet-install CXXFLAGS="-fPIC -pipe -Wall -std=c++17 -Wno-reorder -Wno-unused-variable  -Wno-sign-compare -Wno-unused-but-set-variable -Wno-parentheses -fext-numeric-literals"
    ```

1. Make
    ```
    $ make
    $ make check
    $ make install
    ```

1. Edit .zshrc or .bashrc
    ```
    export FASTJETINSTALLDIR=[PATH_OF_DIRECTORY_FOR_FASTJET]
    export FASTJET=${FASTJETINSTALLDIR}/fastjet-install
    ```

### FastJet Contrib
1. Download
    ```
    cd [PATH_OF_DIRECTORY_FOR_FASTJET]
    wget http://fastjet.hepforge.org/contrib/downloads/fjcontrib-1.048.tar.gz
    tar zxvf fjcontrib-1.048.tar.gz
    cd fjcontrib-1.048
    ```

1. Configure

    On Mac (Intel or M1 with Rosetta 2)
    ```
    $ ./configure --fastjet-config=$PWD/../fastjet-3.4.0/fastjet-config
    ```

    On Wayne State Grid
    ```
    $ ./configure --fastjet-config=$PWD/../fastjet-3.4.0/fastjet-config --prefix=$PWD/../fastjet-install CXXFLAGS="-fPIC -pipe -Wall -std=c++17 -Wno-reorder -Wno-unused-variable  -Wno-sign-compare -Wno-unused-but-set-variable -Wno-parentheses -fext-numeric-literals"
    ```

1. Make
    ```
    $ make
    $ make check
    $ make install
    ```


## Build the code

If you use ROOT of anaconda, you first need to activate the conda environment for ROOT.
```
$ conda activate [YOUR_CONDA_ENVIRONMENT_FOR_ROOT]
```

Then build the code using CMake.
```
$ cd [PATH_OF_JETANALYSIS]
$ mkdir build
$ cd build
$ cmake ..
$ make -j
```

## Run the code

1. Test Run
    ```
    $ mkdir -p [DIRECTORY_FOR_TEST_OUTPUT]
    $ ./Main ../config/test.xml ../test_data [DIRECTORY_FOR_TEST_OUTPUT]
    ```
    Please check whether there is any error or not. 

1. Run
    ```
    $ ./Main [PATH_FOR_XML_FILE] [DIRECTORY_OF_INPUT_DATA] [DIRECTORY_OUTPUT]
    ```
