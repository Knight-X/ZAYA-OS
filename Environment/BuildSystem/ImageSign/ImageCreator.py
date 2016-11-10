# -*- coding: utf-8 -*-
import os
import os.path
import sys
import config
from shutil import copyfile
import subprocess

import sys, string, os

print ('Image Creator Started')

rootPath = '..\..\..'
imageName = sys.argv[1]
projectPath = rootPath + '\\Projects\\' + imageName + '\\uVision'
projectOutPath = projectPath + '\\Objects'
outDirectory = rootPath + '\\out\\Projects\\' + imageName

print projectOutPath

intelHexFilePath = projectOutPath + '\\' + imageName + '.hex'
binaryFilePath = projectOutPath + '\\' + imageName + '.bin'

if not os.path.exists(outDirectory):
    os.makedirs(outDirectory)

copyfile(intelHexFilePath, outDirectory + '\\' + imageName + '.hex')
copyfile(binaryFilePath, outDirectory + '\\' + imageName + '.bin')

rsaSignCmd = 'rsa_sign.exe ' + outDirectory + '\\' + imageName + '.bin'
print rsaSignCmd
#subprocess.call(rsaSignCmd, stdin=None, stdout=None, stderr=None, shell=False)
#os.system( rsaSignCmd )
#os.system(r'"rsa_sign.exe"')
#subprocess.Popen([r"rsa_sign.exe " + outDirectory + '\\' + imageName + '.bin'])

#os.system('"rsa_sign.exe"')
#subprocess.call(['C:\Program Files\Notepad++\\notepad++.exe', 'D:\\test.txt'])
#subprocess.call([r"rsa_sign.exe"])


mypath = os.path.abspath(__file__)
mydir = os.path.dirname(mypath)
start = os.path.join(mydir, "rsa_sign.exe " + outDirectory + '\\' + imageName + '.bin ' + mydir + '\\rsa_priv.txt')
#subprocess.call([sys.executable, start])

import win32api
win32api.WinExec(start)

print start

#if not os.path.exists('D:\\dev\\SP-OS\\Environment\\BuildSystem\\ImageSign\\rsa_sign.exe'):
#	print "DOES NOT !!!!!!!"
#os.system(r'"rsa_sign.exe"')


#win32api.WinExec('rsa_sign.exe')

#os.system('"D:\dev\SP-OS\Environment\BuildSystem\ImageSign\rsa_sign.exe"')

signatureFilePath = config.imageMetaDataHeader['signatureFilePath']
codeOffset = config.imageMetaDataHeader['codeOffset']
codeSize = config.imageMetaDataHeader['codeSize']
ramOffset = config.imageMetaDataHeader['ramOffset']
ramSize = config.imageMetaDataHeader['ramSize']

if (os.path.exists(intelHexFilePath) == False):
	print ' Image Intel HEX File does not exist : ' + intelHexFilePath
	sys.exit()

#if (os.path.exists(binaryFilePath) == False):
#	print ' Image Binary File does not exist : ' + binaryFilePath
#	sys.exit()

if (os.path.exists(signatureFilePath) == False):
	print ' Image Signature File does not exist : ' + signatureFilePath
	sys.exit()

intelHexSourceFile = open(intelHexFilePath)
intelHexDestinationFile = open(intelHexFilePath + ".signed", "w")
signatureFile = open(signatureFilePath)

# TODO 
# 1. No need for intel hex file. We can create our signed intelhex using binary file. 
# 2. While we are modifiying intel hex file (until fix previous TODO item), user app intel hex file also tries to write CRP (:04000005000102CD27), 
#	 and user app should not write here and it can be problem if bootloader or kernel write different value to CRP. 

def int_to_bytes(val, num_bytes):
    return [(val & (0xff << pos*8)) >> pos*8 for pos in range(num_bytes)]

def get_intel_hex_line(dataLength, address, recordType, lineData):
	line = ':{:02X}{:04X}{:02X}'.format(dataLength, address, recordType)
	line +=  ''.join('{:02X}'.format(x) for x in lineData[0:16])
	
	crc = dataLength + (address & 0xFF) + (address >> 8) + recordType
		
	for item in lineData:
		crc += item
	
	crc = ((~crc) + 1) & 0xFF
	
	line += '{:02X}\r\n'.format(crc)
	
	return line

def writeImageSignature(offset):
	signLines = signatureFile.readlines()
	for signLine in signLines:
		offset += 16
		signLine = signLine.replace('\n','').replace('\r','')
		#print signLine
		signLineBytes = bytearray.fromhex(signLine)
		line = get_intel_hex_line(16, offset, 0, signLineBytes)
		intelHexDestinationFile.write(line)

def writeImageMetaData(codeOffset, codeSize, ramOffset, ramSize):
	b = [0xFF] * 16

	b[0:4] = int_to_bytes(codeOffset, 4)
	b[4:4] = int_to_bytes(codeSize, 4)
	b[8:4] = int_to_bytes(ramOffset, 4)
	b[12:4] = int_to_bytes(ramSize, 4)

	offset = 0

	line = get_intel_hex_line(16, offset, 0, b[0:16])
	intelHexDestinationFile.write(line) 
	# print line

	lineIndex = 0
	b = [0xFF] * 16

	while lineIndex < 15:
		offset += 16
		line = get_intel_hex_line(16, offset, 0, b)
		lineIndex = lineIndex + 1
		intelHexDestinationFile.write(line)
		#print line
	
	writeImageSignature(offset)

import subprocess
subprocess.call(['', 'C:\\test.txt'])

firstLine = intelHexSourceFile.readline()
intelHexDestinationFile.write(firstLine)

writeImageMetaData(codeOffset, codeSize, ramOffset, ramSize)

otherLines = intelHexSourceFile.readlines()

for line in otherLines:
	intelHexDestinationFile.write(line)
