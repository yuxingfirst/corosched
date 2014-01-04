#!/bin/bash
HOME=`pwd`
cd ../
BASEHOME=`pwd`
cd $HOME
mkdir -p $HOME/build
cd $HOME/build
cmake -DCMAKE_INSTALL_PREFIX=$BASEHOME ../
make
make install
cd ../
rm -rf build
