#!/usr/bin/python

import sys
import os
import os.path
import shutil

if len(sys.argv) != 3:
	print ("Needs 2 arguments: directory of images and directory base of destiny")
	sys.exit()

filespath = sys.argv[1]
onlyfiles = [f for f in os.listdir(filespath) if os.path.isfile(os.path.join(filespath, f))]

savedprefix = ''
newpath = sys.argv[2]
index = 0
currentpath = ''

for file in onlyfiles:
	splitname = file.split('_')
	prefix = splitname[0]
	
	if prefix != savedprefix:
		index = index + 1
		currentpath = newpath+'/'+str(index)
		if not os.path.exists(currentpath):
			os.makedirs(currentpath)
		savedprefix = prefix
	shutil.copy2(filespath+'/'+file,currentpath+'/'+'_'.join(splitname))