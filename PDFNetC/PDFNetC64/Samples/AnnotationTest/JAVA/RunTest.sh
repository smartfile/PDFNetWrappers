#!/bin/sh
TEST_NAME=AnnotationTest
javac -cp ../../../Lib/PDFNet.jar *.java
java -Djava.library.path=../../../Lib -cp .:../../../Lib/PDFNet.jar $TEST_NAME
