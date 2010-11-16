#!/usr/bin/python

import sys
import optparse
from os.path import isdir, isfile, splitext, basename, dirname, join as pathjoin
from os import mkdir, SEEK_SET, SEEK_CUR, SEEK_END
from glob import glob
from fnmatch import fnmatch
from PIL import Image

from PL2 import *

mqoHeader = """\
Metasequoia Document
Format Text Ver 1.0

Scene {
    pos 0.0000 -80.0000 1500.0000
    lookat 0.0000 0.0000 0.0000
    head -0.5236
    pich 0.5236
    ortho 0
    zoom2 10.0000
    amb 0.250 0.250 0.250
}
"""

parser = optparse.OptionParser()

#parser.add_option ('-l', '--list', action='store_true', help='only list contents, do not unpack')
#parser.add_option ('-o', '--overwrite', action='store_true', help='overwrite files that already exist')

options, args = parser.parse_args()

if args:
    tmb = args[0]
    args = args[1:]
else:
    tmb = '*.tmb'

if not args:
    args = ['*']

for fn in glob(tmb):
    if not isfile(fn):
        print 'File "%s" does not exist.' % fn
        continue

    print fn, '...'

    path = splitext(fn)[0] + '_files/'
    if not isdir(path): mkdir(path)

    fi = open(fn, 'rb')
    mdl = pl2Model.load(fi)
    fi.close()

    for img in mdl.textures:
        name = img.info['name']
        print '%s %dx%d' % (name, img.size[0], img.size[1])
        img.save(pathjoin(path, name + '.png'))

    print

    mqo = open(path + basename(splitext(fn)[0]) + '.mqo', 'wt')
    dump = open(path + 'dump.txt', 'wt')

    print >>mqo, mqoHeader
    print >>dump, 'PL2 Model Dump of "%s"' % basename(fn)

    print >>mqo, 'Material %d {' % len(mdl.materials)

    print >>dump
    print >>dump, '='*80
    print >>dump, 'Materials:'
    print >>dump

    for i, mtl in enumerate(mdl.materials):
        name = mdl.textures[mtl[-1]].info['name']

        #r, g, b, a = mtl[4:8]
        #A = sum(mtl[ 0: 3]) / 3 # ambient
        #D = sum(mtl[ 4: 7]) / 3 # diffuse
        #S = sum(mtl[ 8:11]) / 3 # specular
        #E = sum(mtl[12:15]) / 3 # emissive
        #P = mtl[16]             # power
        #Ar, Ag, Ab, Aa, Dr, Dg, Db, Da, Sr, Sg, Sb, Sa, Er, Eg, Eb, Ea, P, M = mtl
        #print >>mqo, '\t"%d %s" shader(3) vcol(1) col(%.3f %.3f %.3f %.3f) dif(%.3f) amb(%.3f) emi(%.3f) spc(%.3f) power(%.3f) tex("%s")' % (i, name, r, g, b, a, D, A, E, S, P, name + '.png')
        #print '<%6.3f, %6.3f, %6.3f, %6.3f> <%6.3f, %6.3f, %6.3f, %6.3f> <%6.3f, %6.3f, %6.3f, %6.3f> <%6.3f, %6.3f, %6.3f, %6.3f> %6.3f %8d' % mtl

        print >>mqo, '\t"%d %s" shader(3) vcol(1) col(1.000 1.000 1.000 1.000) dif(0.800) amb(0.600) emi(0.200) spc(0.200) power(10.00) tex("%s")' % (i, name, name + '.png' )
        print >>dump, '#%d:' % i
        print >>dump, '\tAmb\t<%6.3f %6.3f %6.3f %6.3f>' % mtl[0]
        print >>dump, '\tDiff\t<%6.3f %6.3f %6.3f %6.3f>' % mtl[1]
        print >>dump, '\tSpec\t<%6.3f %6.3f %6.3f %6.3f>' % mtl[2]
        print >>dump, '\tEmit\t<%6.3f %6.3f %6.3f %6.3f>' % mtl[3]
        print >>dump, '\tPower\t%6.3f' % mtl[4]
        print >>dump, '\tTexture\t%s' % name

    print >>mqo, '}'

    print >>dump
    print >>dump, '='*80
    print >>dump, 'Objects:'
    print >>dump

    for obj in mdl.objects:
        print >>dump, '-'*80
        print >>dump, 'Object "%s":' % obj.name

        print >>dump
        print >>dump, 'Transform matrix:'
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % obj.matrix[ 0: 4]
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % obj.matrix[ 4: 8]
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % obj.matrix[ 8:12]
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % obj.matrix[12:16]

        print >>dump
        print >>dump, 'Materials:'
        for mtl, start, faces in obj.materials:
            if start == 0xffffffff: break
            print >>dump, '\t#%3d: start = %8d, count = %8d' % (mtl, start, faces)

        uverts = []
        unique = {}

        print >>dump
        print >>dump, 'Vertices:'
        for v in obj.verts:
            print >>dump, '\t<%10.6f %10.6f %10.6f>  bones=%3d(%5.1f%%) %3d(%5.1f%%) %3d(%5.1f%%)  normal=<%10.6f %10.6f %10.6f>  color=%08X  uv=<%10.6f %10.6f>' % (v[0],v[1],v[2],v[6],v[3]*100,v[7],v[4]*100,v[8],v[5]*100,v[10],v[11],v[12],v[13],v[14],v[15])

            t = v[:3]
            if t not in unique:
                unique[t] = len(uverts)
                uverts.append(t)

        #print 'object "%s" has %d vertices, %d unique' % (obj.name, len(obj.vertices), len(uverts))
        print '%s %d %d' % (obj.name, len(uverts), len(obj.verts)/3)

        print >>mqo, 'Object "%s" {' % obj.name
        print >>mqo, '\tvisible 15'
        print >>mqo, '\tlocking 0'
        print >>mqo, '\tshading 1'
        print >>mqo, '\tfacet 180.0'
        print >>mqo, '\tcolor 0.898 0.498 0.698'
        print >>mqo, '\tcolor_type 1'

        print >>mqo, '\tvertex %d {' % len(uverts)
        for v in uverts:
            print >>mqo, '\t\t%.6f %.6f %.6f' % v
        print >>mqo, '\t}'

        print >>mqo, '\tface %d {' % (len(obj.verts) / 3)
        for mtl, start, verts in obj.materials:
            #print mtl, start, faces
            if start == -1: break
            for i in xrange(start, start+verts, 3):
                if i >= len(obj.verts):
                    print i, len(obj.verts)
                    print mtl, start, verts
                    break
                v0 = obj.verts[i+2]
                v1 = obj.verts[i+1]
                v2 = obj.verts[i+0]
                t = (unique[v0[:3]], unique[v1[:3]], unique[v2[:3]], mtl,
                     v0[-2], v0[-1], v1[-2], v1[-1], v2[-2], v2[-1])
                print >>mqo, '\t\t3 V(%d %d %d) M(%d) UV(%.6f %.6f %.6f %.6f %.6f %.6f)' % t
        print >>mqo, '\t}'

        print >>mqo, '}'

        print >>dump

    print >>dump
    print >>dump, '='*80
    print >>dump, 'Bones:'
    print >>dump

    for i, bone in enumerate(mdl.bones):
        print >>dump, '#%3d:' % i
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % bone[ 0: 4]
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % bone[ 4: 8]
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % bone[ 8:12]
        print >>dump, '\t[%10.6f  %10.6f  %10.6f  %10.6f]' % bone[12:16]

    print >>dump
    print >>dump, '='*80
    print >>dump, 'Unknown:'
    print >>dump

    for point in mdl.points:
        #print >>dump, '\t%08X %08X %08X %08X %08X %08X %08X %10.6f %10.6f %10.6f' % point
        print >>dump, '\t%08X %08X %08X %08X %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f' % point

    print >>mqo, 'Eof'

    mqo.close()
    dump.close()

    continue

    if fi.read(4) != 'TMB0':
        print '"%s" has an invalid format'
        continue

    ntextures = fi.read('<L')

    for i in xrange(ntextures):
        name   = fi.read(32).rstrip('\0')
        width  = fi.read('<H')
        height = fi.read('<H')

        print name

        image_size = width * height * 4

        image_data = fi.read(image_size)

        if not options.list:
            for pat in args:
                if fnmatch(name, pat):
                    img = Image.fromstring('RGBA', (width, height), image_data)
                    r, g, b, a = img.split()
                    img = Image.merge('RGBA', (b, g, r, a))
                    img.save(path + name + '.png')

    nmaterials = fi.read('<L')

    fo = open(path + 'material.txt', 'wt')

    for i in xrange(nmaterials):
        mtl = fi.read('<17fL')
        print >>fo, '<%10.6f, %10.6f, %10.6f, %10.6f>, <%10.6f, %10.6f, %10.6f, %10.6f>, <%10.6f, %10.6f, %10.6f, %10.6f>, <%10.6f, %10.6f, %10.6f, %10.6f>, %10.6f, %08X' % mtl

    fo.close()

    nobjs = fi.read('<L')

    fo = open(path + 'object.txt', 'wt')

    for i in xrange(nobjs):
        objname = fi.read(32).rstrip('\0')

        print >>fo, '='*80
        print >>fo, objname
        print >>fo, '-'*80

        matrix = fi.read('<16f')
        print >>fo, '[%10.6f, %10.6f, %10.6f, %10.6f]' % matrix[ 0: 4]
        print >>fo, '[%10.6f, %10.6f, %10.6f, %10.6f]' % matrix[ 4: 8]
        print >>fo, '[%10.6f, %10.6f, %10.6f, %10.6f]' % matrix[ 8:12]
        print >>fo, '[%10.6f, %10.6f, %10.6f, %10.6f]' % matrix[12:16]

        info = fi.read('<3L')
        print >>fo, 'faces=%8d  unk=%8d  mtls=%8d' % info

        nfaces, nunk1, nobjmtls = info

        print >>fo, '-'*5

        for j in xrange(nobjmtls):
            objmtl = fi.read('<3L')
            print >>fo, 'mtl=%8d  start=%8d  faces=%8d' % objmtl

        print >>fo, '-'*5

        for j in xrange(3*nfaces):
            v = fi.read('<6f4B3fL2f')
            print >>fo, '<%10.6f, %10.6f, %10.6f>, <%10.6f, %10.6f, %10.6f>, %02X %02X %02X %02X, <%10.6f, %10.6f, %10.6f>, %08X, <%10.6f, %10.6f>' % v
            if j % 3 == 2: print >>fo


    here = fi.tell()
    fi.seek(0, SEEK_END)
    end = fi.tell()

    if here < end:
        print >>fo, 'TMB is %d bytes (%08X hex) but only read %d (%08X).' % (end, end, here, here)

    fi.close()
    fo.close()

    if not options.list:
        print 'done.'
