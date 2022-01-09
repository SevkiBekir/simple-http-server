#!/usr/bin/env bash

#*************************************************************************
#   defs.h
#   Author: Sevki Kocadag, skocadag@juniper.net
#
#   Description: Build Script for the project
#*************************************************************************

build(){
  echo "############# BUILD #############"
  cd build
  rm -rf debug
  mkdir debug
  cmake -DCMAKE_BUILD_TYPE=Debug --build debug --target simple_http_server
  make debug/.
  echo "######### END OF BUILD #########"
}


echo "Bulding Simple HTTP Test"
build

