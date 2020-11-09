# IncludeList
#
# Author:  Adam Sawicki, http://asawicki.info, adam__REMOVE__@asawicki.info
# Version: 20.11.9
# License: Public Domain
#
# See blog post:
# https://asawicki.info/news_1737

import argparse
import os.path
import re

parser = argparse.ArgumentParser(description='Parse #include in given files with additional include directories to find full list of files included.')
parser.add_argument('File', nargs='*', help='file to parse')
parser.add_argument('-I', dest='include', action='append', help='additional include directory')

reIncludeGlobal = re.compile('^\s*#\s*include\s*<(.+)>\s*$')
reIncludeLocal  = re.compile('^\s*#\s*include\s*"(.+)"\s*$')

def AddInclude(includeAbsolutePath):
    global filesToProcess
    includeAbsolutePath = os.path.abspath(includeAbsolutePath) # Normalize
    filesToProcess.add(includeAbsolutePath)

def ProcessInclude(currFilePath, includeRelativePath, isGlobal):
    global additionalIncludeDirectories
    currFileDir = os.path.dirname(currFilePath)
    if not isGlobal:
        includeAbsolutePath = os.path.join(currFileDir, includeRelativePath)
        if os.path.isfile(includeAbsolutePath):
            AddInclude(includeAbsolutePath)
            return
    for includeDir in additionalIncludeDirectories:
        includeAbsolutePath = os.path.join(includeDir, includeRelativePath)
        if os.path.isfile(includeAbsolutePath):
            AddInclude(includeAbsolutePath)
            return
    if isGlobal:
        includeAbsolutePath = os.path.join(currFileDir, includeRelativePath)
        if os.path.isfile(includeAbsolutePath):
            AddInclude(includeAbsolutePath)
            return
    if isGlobal:
        print('ERROR: Failed #include <{0}>'.format(includeRelativePath))
    else:
        print('ERROR: Failed #include "{0}"'.format(includeRelativePath))

def ProcessFile(filePath):
    global filesToProcess, filesProcessed
    file = open(filePath, 'r')
    line = file.readline()
    while line != '':
        match = reIncludeLocal.fullmatch(line)
        if match:
            ProcessInclude(filePath, match[1], False)
        else:
            match = reIncludeGlobal.fullmatch(line)
            if match:
                ProcessInclude(filePath, match[1], True)
        line = file.readline()
    file.close()
    filesProcessed.add(filePath)


args = parser.parse_args()
filesToProcess = set()
for path in args.File:
    filesToProcess.add(os.path.abspath(path)) # Normalize
filesProcessed = set()
additionalIncludeDirectories = args.include
if not additionalIncludeDirectories:
    additionalIncludeDirectories = []
while len(filesToProcess) > 0:
    filePath = filesToProcess.pop()
    if filePath not in filesProcessed:
        ProcessFile(filePath)
filesProcessedList = list(filesProcessed)
filesProcessedList.sort()
for filePath in filesProcessedList:
    print(filePath)
