import struct, Image
from os.path import splitext

__all__ = ['pl2Object', 'pl2Model', 'pl2Sequence', 'pl2Camera', 'pl2Package']

LONG     = struct.Struct('<L')
TOKEN    = struct.Struct('<4s')
TEXINFO  = struct.Struct('<2H')
MATRIX3  = struct.Struct('<9f')
MATRIX4  = struct.Struct('<16f')
MATERIAL = struct.Struct('<17fl')
OBJMTL   = struct.Struct('<3l')
VERTEX   = struct.Struct('<6f4B3fL2f')
POINT    = struct.Struct('<4B9f')

def _open(f):
    if isinstance(f, str):
        f = open(f, 'rb')
    if not hasattr(f, 'read'):
        raise TypeError, 'file object required'
    return f

def _read(f, s):
    if isinstance(s, str):
        s = struct.Struct(s)
    t = f.read(s.size)
    if len(t) != s.size:
        raise struct.error,'unpack needs %d bytes but got only %d' % (s.size, len(t))
    r = s.unpack(t)
    if len(r) == 1: return r[0]
    return r

def _write(f, s, *args):
    if isinstance(s, str):
        s = struct.Struct(s)
    f.write(s.pack(*args))

def _transpose(M):
    (AA, AB, AC, AD,
     BA, BB, BC, BD,
     CA, CB, CC, CD,
     DA, DB, DC, DD) = M
    return (
     AA, BA, CA, DA,
     AB, BB, CB, DB,
     AC, BC, CC, DC,
     AD, BD, CD, DD)

IDENTITY = (
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0)

class pl2Object:
    def __init__(self):
        self.name = ''
        self.matrix = IDENTITY
        self.materials = []
        self.verts = []
        self.tverts = []

    @staticmethod
    def load(f):
        f = _open(f)

        self = pl2Object()

        self.name = f.read(32).rstrip('\0')
        self.matrix = _transpose(_read(f, MATRIX4))

        nfaces  = _read(f, LONG)
        self.unknown = _read(f, LONG)
        nmtls   = _read(f, LONG)

        for i in xrange(nmtls):
            mtl, ofs, len = _read(f, OBJMTL)
            if ofs < 0: break
            self.materials.append((mtl, ofs, 3 * len))

        for i in xrange(3*nfaces):
            vtx = _read(f, VERTEX)
            self.verts.append(vtx)
            self.tverts.append(vtx)

        return self

class pl2Model:
    MAGIC = 'TMB0'

    def __init__(self):
        self.textures = []
        self.materials = []
        self.objects = []
        self.bones = []
        self.points = []

    @staticmethod
    def load(f):
        f = _open(f)

        self = pl2Model()

        self.magic = _read(f, TOKEN)
        if self.magic != self.MAGIC:
            raise ValueError, 'wrong magic number'

        # textures
        for i in xrange(_read(f, LONG)):
            name = f.read(32).rstrip('\0')
            width, height = _read(f, TEXINFO)
            data = f.read(4 * width * height)

            img = Image.fromstring('RGBA', (width, height), data)
            r, g, b, a = img.split()
            img = Image.merge('RGBA', (b, g, r, a))

            img.info['name'] = name

            self.textures.append(img)

        for i in xrange(_read(f, LONG)):
            mtl = _read(f, MATERIAL)
            D = mtl[ 0: 4]
            A = mtl[ 4: 8]
            S = mtl[ 8:12]
            E = mtl[12:16]
            P, T = mtl[16:18]
            self.materials.append((D, A, S, E, P, T))

        for i in xrange(_read(f, LONG)):
            self.objects.append(pl2Object.load(f))

        for i in xrange(_read(f, LONG)):
            self.bones.append(_transpose(_read(f, MATRIX4)))

        #for i in xrange(_read(f, LONG)):
        #    self.points.append(_read(f, POINT))

        return self

class pl2Sequence:
    MAGIC = 'TSB0'

    def __init__(self):
        self.frames = []
        self.special = []
        self.loopFrame = 0

    @staticmethod
    def load(f):
        f = _open(f)

        self = pl2Sequence()

        self.magic = _read(f, TOKEN)
        if self.magic != self.MAGIC:
            raise ValueError, 'wrong magic number'

        _read(f, LONG)
        _read(f, LONG)
        _read(f, LONG)

        nbones = _read(f, LONG)
        nframes = _read(f, LONG)
        self.loopFrame = _read(f, LONG)
        nspecial = _read(f, LONG)

        for i in xrange(nframes):
            frame = []
            for j in xrange(nbones):
                frame.append(_transpose(_read(f, MATRIX4)))
            self.frames.append(frame)

        for i in xrange(nframes):
            frame = []
            for j in xrange(nspecial):
                frame.append(_read(f, MATRIX3))
            self.special.append(frame)

        return self

class pl2Camera:
    def __init__(self):
        self.frames = []
        self.fstart = 0
        self.fstop = 0

    @staticmethod
    def load(f):
        f = _open(f)

        self = pl2Camera()

        text = ''

        for line in f:
            c = line.find('#')
            if c >= 0: line = line[:c]
            text += line

        tokens = text.split()

        i = 0
        s = 0 # simple state counter
        while i < len(tokens) and tokens[i] != '<___end___>':
            if s == 0:
                if tokens[i] == '@TCM100':
                    s = 1
            elif s == 1:
                if tokens[i] == '[CAMERAANIM]':
                    s = 2
            elif s == 2:
                self.fstart, self.fstop = tokens[i], tokens[i+1]
                i += 1
                s = 3
            elif s == 3:
                self.frames.append(
                    # location
                    ((float(tokens[i+0]), float(tokens[i+1]), float(tokens[i+2])),
                    # look-at
                     (float(tokens[i+3]), float(tokens[i+4]), float(tokens[i+5])),
                    # fov
                      float(tokens[i+6])))
                i += 6
            else:
                s = -1

            i += 1

        return self

class pl2Package:
    def __init__(self, f):
        f = _open(f)

        self.file = f

        f.read(16)

        self.files = {}
        self.ifiles = []

        name = f.read(32).rstrip('\0')
        offset = _read(f, LONG)
        lenpck = _read(f, LONG)
        length = _read(f, LONG)
        _read(f, LONG)

        self.files[name] = (offset, lenpck, length)
        self.ifiles.append((name, offset, lenpck, length))

        nfiles = offset / 48

        for i in xrange(1, nfiles):
            name = f.read(32).rstrip('\0')
            offset = _read(f, LONG)
            lenpck = _read(f, LONG)
            length = _read(f, LONG)
            _read(f, LONG)
            self.files[name] = (offset, lenpck, length)
            self.ifiles.append((name, offset, lenpck, length))

    def __getitem__(self, key):
        if isinstance(key, str):
            return self.files[key]
        elif isinstance(key, int):
            return self.ifiles[key]
        else:
            raise KeyError(key)

    def __iter__(self):
        return (k for k in self.files)

    def close(self):
        self.file.close()

    def read(self, f):
        if isinstance(f, str) or isinstance(f, int):
            f = self[f]
        if not isinstance(f, tuple):
            raise TypeError, "expected file name, index, or tuple"
        if len(f) == 3:
            offset, lenpck, length = f
        elif len(f) == 4:
            name, offset, lenpck, length = f
        else:
            raise ValueError, "tuple must have length 3 or 4"

        self.file.seek(offset)
        s = self.file.read(lenpck)

        if lenpck < length:
            s = self._decompress(s, length)

        return s

    def _decompress (self, s, size):
        rv = ''
        buf = ['\0'] * 4096

        mask = 256
        x = None

        for c in s:
            if len(rv) >= size: break

            if mask > 128:
                flags = ord(c)
                mask = 1

            if flags & mask:
                buf[len(rv) % len(buf)] = c
                rv += c

            else:
                if x == None:
                    x = ord(c)
                    continue
                else:
                    y = ord(c)

                    # OOOL
                    offset = (((y & 240) << 4) | x) + 18
                    length = (y & 15) + 3

                    length = min(length, size - len(rv))

                    for j in range(length):
                        c = buf[(offset + j) % len(buf)]
                        buf[len(rv) % len(buf)] = c
                        rv += c

                    x = None

            mask += mask

        return rv
