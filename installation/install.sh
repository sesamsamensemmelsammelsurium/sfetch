#!/bin/sh

cc -o sfetch ../program/sfetch.c && mv sfetch /usr/local/bin && cp -r ../ascii /usr/local/share
