#!/bin/bash

SOURCE=lib-SLProject/source

cppcheck --xml $SOURCE 2> report-cppcheck.xml
rats -w 3 --xml $SOURCE > report-rats.xml
find $SOURCE -regex ".*\.cpp\|.*\.hpp" > cpp-files
vera++ --show-rule --checkstyle-report report-vera.xml $(grep -v 'boost' cpp-files | tr '\n' ' ')
sonar-scanner
