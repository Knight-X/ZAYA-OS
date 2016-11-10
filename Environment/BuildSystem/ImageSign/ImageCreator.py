# -*- coding: utf-8 -*-
import os
import os.path
import sys
import config
from shutil import copyfile
import subprocess
import win32api
import time

# TODO 
# 1. No need for intel hex file. We can create our signed intelhex using binary file. 
# 2. While we are modifiying intel hex file (until fix previous TODO item), user app intel hex file also tries to write CRP (:04000005000102CD27), 
#	 and user app should not write here and it can be problem if bootloader or kernel write different value to CRP. 

print ('Image Creator Started')

mypath = os.path.abspath(__file__)
mydir = os.path.dirname(mypath)

rootPath = '..\..\..'
imageName = sys.argv[1]
projectPath = rootPath + '\\Projects\\' + imageName + '\\uVision'
projectOutPath = projectPath + '\\Objects'
outDirectory = rootPath + '\\out\\Projects\\' + imageName

intelHexFilePath = projectOutPath + '\\' + imageName + '.hex'
intelHEXOutFilePath = outDirectory + '\\' + imageName + '.hex'

binaryFilePath = projectOutPath + '\\' + imageName + '.bin'
binaryOutFilePath = outDirectory + '\\' + imageName + '.bin'

if (os.path.exists(intelHexFilePath) == False):
	print ' Image Intel HEX File does not exist : ' + intelHexFilePath
	sys.exit()

if (os.path.exists(binaryFilePath) == False):
	print ' Image Binary File does not exist : ' + binaryFilePath
	sys.exit()

if not os.path.exists(outDirectory):
    os.makedirs(outDirectory)
	
copyfile(intelHexFilePath, intelHEXOutFilePath)
copyfile(binaryFilePath, binaryOutFilePath)

intelHexSourceFile = open(intelHEXOutFilePath)
intelHexDestinationFile = open(intelHEXOutFilePath + ".signed", "w")

def int_to_bytes(val, num_bytes):
    return [(val & (0xff << pos*8)) >> pos*8 for pos in range(num_bytes)]

def get_intel_hex_line(dataLength, address, recordType, lineData):
	line = ':{:02X}{:04X}{:02X}'.format(dataLength, address, recordType)
	line +=  ''.join('{:02X}'.format(x) for x in lineData[0:16])
	
	crc = dataLength + (address & 0xFF) + (address >> 8) + recordType
		
	for item in lineData:
		crc += item
	
	crc = ((~crc) + 1) & 0xFF
	
	line += '{:02X}\n'.format(crc)
	return line

def writeImageSignature(signatureFile, offset):
	signLines = signatureFile.readlines()
	for signLine in signLines:
		offset += 16
		signLine = signLine.replace('\n','').replace('\r','')
		#print signLine
		signLineBytes = bytearray.fromhex(signLine)
		line = get_intel_hex_line(16, offset, 0, signLineBytes)
		intelHexDestinationFile.write(line)

def writeImageMetaData(signatureFile, codeOffset, codeSize, ramOffset, ramSize):
	b = [0xFF] * 16

	b[0:4] = int_to_bytes(codeOffset, 4)
	b[4:4] = int_to_bytes(codeSize, 4)
	b[8:4] = int_to_bytes(ramOffset, 4)
	b[12:4] = int_to_bytes(ramSize, 4)

	offset = 0

	line = get_intel_hex_line(16, offset, 0, b[0:16])
	intelHexDestinationFile.write(line)
	
	lineIndex = 0
	b = [0xFF] * 16

	while lineIndex < 15:
		offset += 16
		line = get_intel_hex_line(16, offset, 0, b)
		lineIndex = lineIndex + 1
		intelHexDestinationFile.write(line)
	
	writeImageSignature(signatureFile, offset)

def prepare_signed_image(intelHexSourceFile):
	start = os.path.join(mydir, "rsa_sign.exe " + outDirectory + '\\' + imageName + '.bin ' + mydir + '\\rsa_priv.txt')

	win32api.WinExec(start)

	signatureFilePath = binaryOutFilePath + '.sig'
	codeOffset = config.imageMetaDataHeader['codeOffset']
	codeSize = config.imageMetaDataHeader['codeSize']
	ramOffset = config.imageMetaDataHeader['ramOffset']
	ramSize = config.imageMetaDataHeader['ramSize']

	while not os.path.exists(signatureFilePath):
		time.sleep(0.05)
	
	signatureFile = open(signatureFilePath)

	firstLine = intelHexSourceFile.readline()
	intelHexDestinationFile.write(firstLine)

	writeImageMetaData(signatureFile, codeOffset, codeSize, ramOffset, ramSize)

	otherLines = intelHexSourceFile.readlines()

	for line in otherLines:
		intelHexDestinationFile.write(line)

prepare_signed_image(intelHexSourceFile)
