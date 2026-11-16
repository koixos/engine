#!/bin/bash

echo "Cleaning everything..."


make -f "Makefile.engine.linux.mak" clean
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "ERROR:"$ERRORLEVEL && exit
fi


make -f "Makefile.testbed.linux.mak" clean
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "ERROR:"$ERRORLEVEL && exit
fi


make -f "Makefile.tests.linux.mak" clean
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "ERROR:"$ERRORLEVEL && exit
fi


echo "All assemblies cleaned successfully."