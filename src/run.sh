#!/bin/bash
rm ../output_files/app
g++ main.cpp utilityHelper.cpp scenarioHelper.cpp algorithmHelper.cpp -o ../output_files/app
./../output_files/app
