#!/usr/bin/python3
##
# This file is part of the Td5Gauge Firmware (https://github.com/k0sci3j/Td5Gauge).
# Copyright (c) 2022 Michal Kosciowski BinOwl.
# 
# This program is free software: you can redistribute it and/or modify  
# it under the terms of the GNU General Public License as published by  
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
#  obfuscate.py
#  Created on: 7.04.2022
#  Author: BinOwl
# 
##
import sys
import getopt

argv = sys.argv[1:]

try:
    opts, args = getopt.getopt(argv,'i:o:', ['infile','outfile'])
    if len(opts) < 2 or len(opts) > 2:
        print('usage: obfuscate.py -i input_file -o output_file')
        sys.exit(1)
except getopt.GetoptError:
    print('usage: obfuscate.py -i input_file -o output_file')
    sys.exit(1)

for opt in opts:
    if(opt[0] == "-i"):
        try:
            infile = open(opt[1], 'rb')
        except:
            print("Cannot open input_file")
            sys.exit(1)
    if(opt[0] == "-o"):
        try:
            outfile = open(opt[1], 'wb')
        except:
            print("Cannot open output_file")
            sys.exit(1)


array=[]
for l in infile.read():
    array.append(l)
for i in range(0, (round(len(array)/16)+1)*16-len(array)):
    array.append(0x0)
c=0
array_obf=[]
for l in array:
    if(c%0x21 == 1):
        l=l^0x16
    if(c%0x7 == 1):
        l=l^0x54
    if(c%0x3 == 1):
        l=l^0x20
    if(c%0x11 == 1):
        l=l^0x4
    if(c%0x37 == 1):
        l=l^0x78
    if(c%0x5 == 1):
        l=l^0x80
    if(c%0x13 == 1):
        l=l^0x60
    if(c%0x73 == 1):
        l=l^0x14
    if(c%0x43 == 1):
        l=l^0x12
    if(c%0x31 == 1):
        l=l^0x66
    if(c%0x97 == 1):
        l=l^0x22
    if(c%0x17 == 1):
        l=l^0x10
    c+=1
    if (l&0x35)%2 == 0:
        e=l^0x5
    else:
        e=l^0x29
    array_obf.append(e)

byte_array = bytearray(array_obf)
outfile.write(byte_array)
infile.close()
outfile.close()
print("Done!")

