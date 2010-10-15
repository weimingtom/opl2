#!/usr/bin/python
# PL2 format:
#  header: 16 null bytes
#  (for each file:)
#    filename: 32 bytes
#    offset: int
#    length: int
#    real size: int
#    unknown: int
#  file data...
#


import sys
import optparse
from os.path import isdir, isfile
from os import mkdir
from glob import glob
from fnmatch import fnmatch

def fsize (f):
    t = f.tell()
    f.seek(0,2)
    s = f.tell()
    f.seek(t)
    return s

def readbyte (f):
    b = f.read(1)
    if b: return ord(b)
    else: return None
def readshort (f):
    x = readbyte(f)
    y = readbyte(f)
    return (y << 8) | x
def readint (f):
    x = readshort(f)
    y = readshort(f)
    return (y << 16) | x

def decompress (fi, fo, size):
    buf = ['\0'] * 4096
    bytes = 0


    ###
    n = 0
    ###

    mask = 256

    while bytes < size:

        if mask > 128:
            flags = readbyte(fi)
            mask = 1

        if flags & mask:
            b = fi.read(1)
            buf[bytes % len(buf)] = b
            fo.write(b)
            bytes += 1

        else:
            x = readbyte(fi)
            y = readbyte(fi)

            # OOOL
            offset = (((y & 240) << 4) | x) + 18
            length = (y & 15) + 3

            #if n < 20:
            #    if sys.stdout.softspace: print
            #    print 'offset: %03x  length: %x (%2d)' % (offset, length - 3, length)
            #    n += 1

            length = min(length, size - bytes)

            for j in range(length):
                b = buf[(offset + j) % len(buf)]
                buf[bytes % len(buf)] = b
                fo.write(b)
                bytes += 1

        mask += mask

parser = optparse.OptionParser()

parser.add_option ('-l', '--list', action='store_true', help='only list contents, do not unpack')
#parser.add_option ('-o', '--overwrite', action='store_true', help='overwrite files that already exist')

options, args = parser.parse_args()

if args:
    pl2 = args[0]
    args = args[1:]
else:
    pl2 = '*.pl2'

if not args:
    args = ['*']

for fn in glob(pl2):
    if not isfile(fn):
        print 'File "%s" does not exist.' % fn
        exit()

    path = fn + '_files/'
    if not options.list and not isdir(path): mkdir(path)

    fi = open(fn, 'rb')
    flen = fsize(fi)

    index = []

    fi.read(16)

    offset = length = 0

    while (offset + length) < flen:
        name = fi.read(32).rstrip('\0')
        offset = readint(fi)
        length = readint(fi)
        realsize = readint(fi)
        readint(fi)

        for pat in args:
            if fnmatch(name, pat):
                index.append((name, offset, length, realsize))
                break

    if options.list:

        print "---------------------------------+----------+----------+---------"
        print "Filename                         | Offset   |   Packed | Unpacked"
        print "---------------------------------+----------+----------+---------"
        for info in index:
            print "%-32s | %08x | % 8d | % 8d" % info
        print "---------------------------------+----------+----------+---------"
        print "%d files in package" % len(index)
        print "-----------------------------------------------------------------"

    else:

        print "Extracting %d files..." % len(index)

        for name, offset, length, realsize in index:
            if length != realsize:
                print "%s (%d bytes -> %d bytes) ..." % (name, length, realsize),
            else:
                print "%s (%d bytes) ..." % (name, realsize),

            #try:
            fo = open(path + name, 'wb')
            fi.seek(offset)
            if length != realsize:
                decompress(fi, fo, realsize)
            else:
                fo.write(fi.read(length))
            fo.close()
            #except Exception, x:
            #    print "error,", x
            #    continue

            print "done"


