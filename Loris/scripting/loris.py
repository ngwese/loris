# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

"""

Loris is an Open Source sound modeling and processing software package
based on the Reassigned Bandwidth-Enhanced Additive Sound Model. Loris
supports modified resynthesis and manipulations of the model data,
such as time- and frequency-scale modification and sound morphing.


Loris is developed by Kelly Fitz and Lippold Haken at the CERL Sound
Group, and is distributed under the GNU General Public License (GPL).
For more information, please visit

   http://www.cerlsoundgroup.org/Loris/

"""

import _loris

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


class DoubleVector(_object):
    """Proxy of C++ DoubleVector class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, DoubleVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, DoubleVector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ std::vector<double > instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def empty(*args): 
        """empty(self) -> bool"""
        return _loris.DoubleVector_empty(*args)

    def size(*args): 
        """size(self) -> size_type"""
        return _loris.DoubleVector_size(*args)

    def clear(*args): 
        """clear(self)"""
        return _loris.DoubleVector_clear(*args)

    def swap(*args): 
        """swap(self, v)"""
        return _loris.DoubleVector_swap(*args)

    def get_allocator(*args): 
        """get_allocator(self) -> allocator_type"""
        return _loris.DoubleVector_get_allocator(*args)

    def pop_back(*args): 
        """pop_back(self)"""
        return _loris.DoubleVector_pop_back(*args)

    def __init__(self, *args):
        """
        __init__(self) -> DoubleVector
        __init__(self, ??) -> DoubleVector
        __init__(self, size) -> DoubleVector
        __init__(self, size, value) -> DoubleVector
        """
        _swig_setattr(self, DoubleVector, 'this', _loris.new_DoubleVector(*args))
        _swig_setattr(self, DoubleVector, 'thisown', 1)
    def push_back(*args): 
        """push_back(self, x)"""
        return _loris.DoubleVector_push_back(*args)

    def front(*args): 
        """front(self) -> value_type"""
        return _loris.DoubleVector_front(*args)

    def back(*args): 
        """back(self) -> value_type"""
        return _loris.DoubleVector_back(*args)

    def assign(*args): 
        """assign(self, n, x)"""
        return _loris.DoubleVector_assign(*args)

    def resize(*args): 
        """
        resize(self, new_size)
        resize(self, new_size, x)
        """
        return _loris.DoubleVector_resize(*args)

    def reserve(*args): 
        """reserve(self, n)"""
        return _loris.DoubleVector_reserve(*args)

    def capacity(*args): 
        """capacity(self) -> size_type"""
        return _loris.DoubleVector_capacity(*args)

    def __nonzero__(*args): 
        """__nonzero__(self) -> bool"""
        return _loris.DoubleVector___nonzero__(*args)

    def __len__(*args): 
        """__len__(self) -> size_type"""
        return _loris.DoubleVector___len__(*args)

    def pop(*args): 
        """pop(self) -> value_type"""
        return _loris.DoubleVector_pop(*args)

    def __getslice__(*args): 
        """__getslice__(self, i, j) -> std::vector<(double,std::allocator<(double)>)>"""
        return _loris.DoubleVector___getslice__(*args)

    def __setslice__(*args): 
        """__setslice__(self, i, j, v)"""
        return _loris.DoubleVector___setslice__(*args)

    def __delslice__(*args): 
        """__delslice__(self, i, j)"""
        return _loris.DoubleVector___delslice__(*args)

    def __delitem__(*args): 
        """__delitem__(self, i)"""
        return _loris.DoubleVector___delitem__(*args)

    def __getitem__(*args): 
        """__getitem__(self, i) -> value_type"""
        return _loris.DoubleVector___getitem__(*args)

    def __setitem__(*args): 
        """__setitem__(self, i, x)"""
        return _loris.DoubleVector___setitem__(*args)

    def append(*args): 
        """append(self, x)"""
        return _loris.DoubleVector_append(*args)

    def __del__(self, destroy=_loris.delete_DoubleVector):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class DoubleVectorPtr(DoubleVector):
    def __init__(self, this):
        _swig_setattr(self, DoubleVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, DoubleVector, 'thisown', 0)
        _swig_setattr(self, DoubleVector,self.__class__,DoubleVector)
_loris.DoubleVector_swigregister(DoubleVectorPtr)

class MarkerVector(_object):
    """Proxy of C++ MarkerVector class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MarkerVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MarkerVector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ std::vector<Marker > instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def empty(*args): 
        """empty(self) -> bool"""
        return _loris.MarkerVector_empty(*args)

    def size(*args): 
        """size(self) -> size_type"""
        return _loris.MarkerVector_size(*args)

    def clear(*args): 
        """clear(self)"""
        return _loris.MarkerVector_clear(*args)

    def swap(*args): 
        """swap(self, v)"""
        return _loris.MarkerVector_swap(*args)

    def get_allocator(*args): 
        """get_allocator(self) -> allocator_type"""
        return _loris.MarkerVector_get_allocator(*args)

    def pop_back(*args): 
        """pop_back(self)"""
        return _loris.MarkerVector_pop_back(*args)

    def __init__(self, *args):
        """
        __init__(self) -> MarkerVector
        __init__(self, ??) -> MarkerVector
        __init__(self, size) -> MarkerVector
        __init__(self, size, value) -> MarkerVector
        """
        _swig_setattr(self, MarkerVector, 'this', _loris.new_MarkerVector(*args))
        _swig_setattr(self, MarkerVector, 'thisown', 1)
    def push_back(*args): 
        """push_back(self, x)"""
        return _loris.MarkerVector_push_back(*args)

    def front(*args): 
        """front(self) -> value_type"""
        return _loris.MarkerVector_front(*args)

    def back(*args): 
        """back(self) -> value_type"""
        return _loris.MarkerVector_back(*args)

    def assign(*args): 
        """assign(self, n, x)"""
        return _loris.MarkerVector_assign(*args)

    def resize(*args): 
        """
        resize(self, new_size)
        resize(self, new_size, x)
        """
        return _loris.MarkerVector_resize(*args)

    def reserve(*args): 
        """reserve(self, n)"""
        return _loris.MarkerVector_reserve(*args)

    def capacity(*args): 
        """capacity(self) -> size_type"""
        return _loris.MarkerVector_capacity(*args)

    def __nonzero__(*args): 
        """__nonzero__(self) -> bool"""
        return _loris.MarkerVector___nonzero__(*args)

    def __len__(*args): 
        """__len__(self) -> size_type"""
        return _loris.MarkerVector___len__(*args)

    def pop(*args): 
        """pop(self) -> value_type"""
        return _loris.MarkerVector_pop(*args)

    def __getslice__(*args): 
        """__getslice__(self, i, j) -> std::vector<(Marker,std::allocator<(Marker)>)>"""
        return _loris.MarkerVector___getslice__(*args)

    def __setslice__(*args): 
        """__setslice__(self, i, j, v)"""
        return _loris.MarkerVector___setslice__(*args)

    def __delslice__(*args): 
        """__delslice__(self, i, j)"""
        return _loris.MarkerVector___delslice__(*args)

    def __delitem__(*args): 
        """__delitem__(self, i)"""
        return _loris.MarkerVector___delitem__(*args)

    def __getitem__(*args): 
        """__getitem__(self, i) -> value_type"""
        return _loris.MarkerVector___getitem__(*args)

    def __setitem__(*args): 
        """__setitem__(self, i, x)"""
        return _loris.MarkerVector___setitem__(*args)

    def append(*args): 
        """append(self, x)"""
        return _loris.MarkerVector_append(*args)

    def __del__(self, destroy=_loris.delete_MarkerVector):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class MarkerVectorPtr(MarkerVector):
    def __init__(self, this):
        _swig_setattr(self, MarkerVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MarkerVector, 'thisown', 0)
        _swig_setattr(self, MarkerVector,self.__class__,MarkerVector)
_loris.MarkerVector_swigregister(MarkerVectorPtr)


def channelize(*args):
    """
    channelize(partials, refFreqEnvelope, refLabel)

    Label Partials in a PartialList with the integer nearest to the
    amplitude-weighted average ratio of their frequency envelope to a
    reference frequency envelope. The frequency spectrum is
    partitioned into non-overlapping channels whose time-varying
    center frequencies track the reference frequency envelope. The
    reference label indicates which channel's center frequency is
    exactly equal to the reference envelope frequency, and other
    channels' center frequencies are multiples of the reference
    envelope frequency divided by the reference label. Each Partial in
    the PartialList is labeled with the number of the channel that
    best fits its frequency envelope. The quality of the fit is
    evaluated at the breakpoints in the Partial envelope and weighted
    by the amplitude at each breakpoint, so that high- amplitude
    breakpoints contribute more to the channel decision. Partials are
    labeled, but otherwise unmodified. In particular, their
    frequencies are not modified in any way.
    """
    return _loris.channelize(*args)

def dilate(*args):
    """
    dilate(partials, ivec, tvec)

    Dilate Partials in a PartialList according to the given initial
    and target time points. Partial envelopes are stretched and
    compressed so that temporal features at the initial time points
    are aligned with the final time points. Time points are sorted, so
    Partial envelopes are are only stretched and compressed, but
    breakpoints are not reordered. Duplicate time points are allowed.
    There must be the same number of initial and target time points.
    """
    return _loris.dilate(*args)

def distill(*args):
    """
    distill(partials)

    Distill labeled (channelized)  Partials in a PartialList into a 
    PartialList containing a single (labeled) Partial per label. 
    The distilled PartialList will contain as many Partials as
    there were non-zero labels (non-empty channels)
    in the original PartialList. Additionally, unlabeled (label 0) Partials are 
    "collated" into groups of temporally non-overlapping Partials,
    assigned an unused label, and fused into a single Partial per
    group.
    """
    return _loris.distill(*args)

def exportSdif(*args):
    """exportSdif(path, partials)"""
    return _loris.exportSdif(*args)

def importSdif(*args):
    """importSdif(path) -> PartialList"""
    return _loris.importSdif(*args)

def importSpc(*args):
    """importSpc(path) -> PartialList"""
    return _loris.importSpc(*args)

def crop(*args):
    """crop(partials, t1, t2)"""
    return _loris.crop(*args)

def copyLabeled(*args):
    """copyLabeled(partials, label) -> PartialList"""
    return _loris.copyLabeled(*args)

def extractLabeled(*args):
    """extractLabeled(partials, label) -> PartialList"""
    return _loris.extractLabeled(*args)

def removeLabeled(*args):
    """removeLabeled(partials, label)"""
    return _loris.removeLabeled(*args)

def resample(*args):
    """resample(partials, interval)"""
    return _loris.resample(*args)

def shiftTime(*args):
    """shiftTime(partials, offset)"""
    return _loris.shiftTime(*args)

def sift(*args):
    """sift(partials)"""
    return _loris.sift(*args)

def sortByLabel(*args):
    """sortByLabel(partials)"""
    return _loris.sortByLabel(*args)

def version(*args):
    """version() -> char"""
    return _loris.version(*args)
class Marker(_object):
    """Proxy of C++ Marker class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Marker, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Marker, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Marker instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Marker
        __init__(self, t, s) -> Marker
        __init__(self, other) -> Marker
        """
        _swig_setattr(self, Marker, 'this', _loris.new_Marker(*args))
        _swig_setattr(self, Marker, 'thisown', 1)
    def name(*args): 
        """name(self) -> char"""
        return _loris.Marker_name(*args)

    def time(*args): 
        """time(self) -> double"""
        return _loris.Marker_time(*args)

    def setName(*args): 
        """setName(self, s)"""
        return _loris.Marker_setName(*args)

    def setTime(*args): 
        """setTime(self, t)"""
        return _loris.Marker_setTime(*args)

    def __del__(self, destroy=_loris.delete_Marker):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class MarkerPtr(Marker):
    def __init__(self, this):
        _swig_setattr(self, Marker, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Marker, 'thisown', 0)
        _swig_setattr(self, Marker,self.__class__,Marker)
_loris.Marker_swigregister(MarkerPtr)

def createFreqReference(*args):
    """
    createFreqReference(partials, minFreq, maxFreq, numSamps) -> BreakpointEnvelope
    createFreqReference(partials, minFreq, maxFreq) -> BreakpointEnvelope

    Return a newly-constructed BreakpointEnvelope by sampling the
    frequency envelope of the longest Partial in a PartialList. Only
    Partials whose frequency at the Partial's loudest (highest
    amplitude) breakpoint is within the given frequency range are
    considered.

    If the number of sample points is not specified, then the longest
    Partial's frequency envelope is sampled every 30 ms (No fewer than
    10 samples are used, so the sampling maybe more dense for very
    short Partials.)

    For very simple sounds, this frequency reference may be a good
    first approximation to a reference envelope for channelization
    (see channelize).
    """
    return _loris.createFreqReference(*args)

def exportAiff(*args):
    """
    exportAiff(path, samples, samplerate=44100.0, bitsPerSamp=16)
    exportAiff(path, samples, samplerate=44100.0)
    exportAiff(path, samples)
    """
    return _loris.exportAiff(*args)

def exportSpc(*args):
    """
    exportSpc(path, partials, midiPitch, enhanced, endApproachTime)
    exportSpc(path, partials, midiPitch, enhanced)
    exportSpc(path, partials, midiPitch)
    """
    return _loris.exportSpc(*args)

def morph(*args):
    """
    morph(src0, src1, ffreq, famp, fbw) -> PartialList
    morph(src0, src1, freqweight, ampweight, bwweight) -> PartialList
    """
    return _loris.morph(*args)

def synthesize(*args):
    """
    synthesize(partials, srate=44100.0) -> DoubleVector
    synthesize(partials) -> DoubleVector
    """
    return _loris.synthesize(*args)

def scaleAmp(*args):
    """
    scaleAmp(partials, ampEnv)
    scaleAmp(partials, w)
    """
    return _loris.scaleAmp(*args)

def scaleBandwidth(*args):
    """
    scaleBandwidth(partials, bwEnv)
    scaleBandwidth(partials, w)
    """
    return _loris.scaleBandwidth(*args)

def scaleFrequency(*args):
    """
    scaleFrequency(partials, freqEnv)
    scaleFrequency(partials, w)
    """
    return _loris.scaleFrequency(*args)

def scaleNoiseRatio(*args):
    """
    scaleNoiseRatio(partials, noiseEnv)
    scaleNoiseRatio(partials, w)
    """
    return _loris.scaleNoiseRatio(*args)

def shiftPitch(*args):
    """
    shiftPitch(partials, pitchEnv)
    shiftPitch(partials, w)
    """
    return _loris.shiftPitch(*args)

class AiffFile(_object):
    """Proxy of C++ AiffFile class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AiffFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AiffFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ AiffFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_loris.delete_AiffFile):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def sampleRate(*args): 
        """sampleRate(self) -> double"""
        return _loris.AiffFile_sampleRate(*args)

    def midiNoteNumber(*args): 
        """midiNoteNumber(self) -> double"""
        return _loris.AiffFile_midiNoteNumber(*args)

    def sampleFrames(*args): 
        """sampleFrames(self) -> unsigned long"""
        return _loris.AiffFile_sampleFrames(*args)

    def addPartial(*args): 
        """
        addPartial(self, p, fadeTime=.001)
        addPartial(self, p)
        """
        return _loris.AiffFile_addPartial(*args)

    def setMidiNoteNumber(*args): 
        """setMidiNoteNumber(self, nn)"""
        return _loris.AiffFile_setMidiNoteNumber(*args)

    def write(*args): 
        """
        write(self, filename, bps=16)
        write(self, filename)
        """
        return _loris.AiffFile_write(*args)

    def __init__(self, *args):
        """
        __init__(self, filename) -> AiffFile
        __init__(self, vec, samplerate) -> AiffFile
        __init__(self, l, sampleRate=44100, fadeTime=.001) -> AiffFile
        __init__(self, l, sampleRate=44100) -> AiffFile
        __init__(self, l) -> AiffFile
        """
        _swig_setattr(self, AiffFile, 'this', _loris.new_AiffFile(*args))
        _swig_setattr(self, AiffFile, 'thisown', 1)
    def samples(*args): 
        """samples(self) -> DoubleVector"""
        return _loris.AiffFile_samples(*args)

    def channels(*args): 
        """channels(self) -> int"""
        return _loris.AiffFile_channels(*args)

    def addPartials(*args): 
        """
        addPartials(self, l, fadeTime=0.001)
        addPartials(self, l)
        """
        return _loris.AiffFile_addPartials(*args)

    def numMarkers(*args): 
        """numMarkers(self) -> int"""
        return _loris.AiffFile_numMarkers(*args)

    def getMarker(*args): 
        """getMarker(self, i) -> Marker"""
        return _loris.AiffFile_getMarker(*args)

    def removeMarker(*args): 
        """removeMarker(self, i)"""
        return _loris.AiffFile_removeMarker(*args)

    def addMarker(*args): 
        """addMarker(self, m)"""
        return _loris.AiffFile_addMarker(*args)

    def clearMarkers(*args): 
        """clearMarkers(self)"""
        return _loris.AiffFile_clearMarkers(*args)

    def markers(*args): 
        """markers(self) -> MarkerVector"""
        return _loris.AiffFile_markers(*args)

    def addMarkers(*args): 
        """addMarkers(self, markers)"""
        return _loris.AiffFile_addMarkers(*args)


class AiffFilePtr(AiffFile):
    def __init__(self, this):
        _swig_setattr(self, AiffFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AiffFile, 'thisown', 0)
        _swig_setattr(self, AiffFile,self.__class__,AiffFile)
_loris.AiffFile_swigregister(AiffFilePtr)

class Analyzer(_object):
    """Proxy of C++ Analyzer class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Analyzer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Analyzer, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Analyzer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, resolutionHz, windowWidthHz=0.) -> Analyzer
        __init__(self, resolutionHz) -> Analyzer
        """
        _swig_setattr(self, Analyzer, 'this', _loris.new_Analyzer(*args))
        _swig_setattr(self, Analyzer, 'thisown', 1)
    def copy(*args): 
        """copy(self) -> Analyzer"""
        return _loris.Analyzer_copy(*args)

    def analyze(*args): 
        """
        analyze(self, vec, srate) -> PartialList
        analyze(self, vec, srate, env) -> PartialList
        """
        return _loris.Analyzer_analyze(*args)

    def freqResolution(*args): 
        """freqResolution(self) -> double"""
        return _loris.Analyzer_freqResolution(*args)

    def ampFloor(*args): 
        """ampFloor(self) -> double"""
        return _loris.Analyzer_ampFloor(*args)

    def windowWidth(*args): 
        """windowWidth(self) -> double"""
        return _loris.Analyzer_windowWidth(*args)

    def sidelobeLevel(*args): 
        """sidelobeLevel(self) -> double"""
        return _loris.Analyzer_sidelobeLevel(*args)

    def freqFloor(*args): 
        """freqFloor(self) -> double"""
        return _loris.Analyzer_freqFloor(*args)

    def hopTime(*args): 
        """hopTime(self) -> double"""
        return _loris.Analyzer_hopTime(*args)

    def freqDrift(*args): 
        """freqDrift(self) -> double"""
        return _loris.Analyzer_freqDrift(*args)

    def cropTime(*args): 
        """cropTime(self) -> double"""
        return _loris.Analyzer_cropTime(*args)

    def bwRegionWidth(*args): 
        """bwRegionWidth(self) -> double"""
        return _loris.Analyzer_bwRegionWidth(*args)

    def setFreqResolution(*args): 
        """setFreqResolution(self, x)"""
        return _loris.Analyzer_setFreqResolution(*args)

    def setAmpFloor(*args): 
        """setAmpFloor(self, x)"""
        return _loris.Analyzer_setAmpFloor(*args)

    def setWindowWidth(*args): 
        """setWindowWidth(self, x)"""
        return _loris.Analyzer_setWindowWidth(*args)

    def setSidelobeLevel(*args): 
        """setSidelobeLevel(self, x)"""
        return _loris.Analyzer_setSidelobeLevel(*args)

    def setFreqFloor(*args): 
        """setFreqFloor(self, x)"""
        return _loris.Analyzer_setFreqFloor(*args)

    def setFreqDrift(*args): 
        """setFreqDrift(self, x)"""
        return _loris.Analyzer_setFreqDrift(*args)

    def setHopTime(*args): 
        """setHopTime(self, x)"""
        return _loris.Analyzer_setHopTime(*args)

    def setCropTime(*args): 
        """setCropTime(self, x)"""
        return _loris.Analyzer_setCropTime(*args)

    def setBwRegionWidth(*args): 
        """setBwRegionWidth(self, x)"""
        return _loris.Analyzer_setBwRegionWidth(*args)

    def __del__(self, destroy=_loris.delete_Analyzer):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class AnalyzerPtr(Analyzer):
    def __init__(self, this):
        _swig_setattr(self, Analyzer, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Analyzer, 'thisown', 0)
        _swig_setattr(self, Analyzer,self.__class__,Analyzer)
_loris.Analyzer_swigregister(AnalyzerPtr)

class BreakpointEnvelope(_object):
    """Proxy of C++ BreakpointEnvelope class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BreakpointEnvelope, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BreakpointEnvelope, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ BreakpointEnvelope instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> BreakpointEnvelope
        __init__(self, initialValue) -> BreakpointEnvelope
        """
        _swig_setattr(self, BreakpointEnvelope, 'this', _loris.new_BreakpointEnvelope(*args))
        _swig_setattr(self, BreakpointEnvelope, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_BreakpointEnvelope):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def copy(*args): 
        """copy(self) -> BreakpointEnvelope"""
        return _loris.BreakpointEnvelope_copy(*args)

    def insertBreakpoint(*args): 
        """insertBreakpoint(self, time, value)"""
        return _loris.BreakpointEnvelope_insertBreakpoint(*args)

    def valueAt(*args): 
        """valueAt(self, x) -> double"""
        return _loris.BreakpointEnvelope_valueAt(*args)


class BreakpointEnvelopePtr(BreakpointEnvelope):
    def __init__(self, this):
        _swig_setattr(self, BreakpointEnvelope, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BreakpointEnvelope, 'thisown', 0)
        _swig_setattr(self, BreakpointEnvelope,self.__class__,BreakpointEnvelope)
_loris.BreakpointEnvelope_swigregister(BreakpointEnvelopePtr)


def BreakpointEnvelopeWithValue(*args):
    """BreakpointEnvelopeWithValue(initialValue) -> BreakpointEnvelope"""
    return _loris.BreakpointEnvelopeWithValue(*args)
class SdifFile(_object):
    """Proxy of C++ SdifFile class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SdifFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SdifFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ SdifFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_loris.delete_SdifFile):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def write(*args): 
        """write(self, path)"""
        return _loris.SdifFile_write(*args)

    def write1TRC(*args): 
        """write1TRC(self, path)"""
        return _loris.SdifFile_write1TRC(*args)

    def __init__(self, *args):
        """
        __init__(self, filename) -> SdifFile
        __init__(self) -> SdifFile
        __init__(self, l) -> SdifFile
        """
        _swig_setattr(self, SdifFile, 'this', _loris.new_SdifFile(*args))
        _swig_setattr(self, SdifFile, 'thisown', 1)
    def partials(*args): 
        """partials(self) -> PartialList"""
        return _loris.SdifFile_partials(*args)

    def addPartials(*args): 
        """addPartials(self, l)"""
        return _loris.SdifFile_addPartials(*args)

    def numMarkers(*args): 
        """numMarkers(self) -> int"""
        return _loris.SdifFile_numMarkers(*args)

    def getMarker(*args): 
        """getMarker(self, i) -> Marker"""
        return _loris.SdifFile_getMarker(*args)

    def removeMarker(*args): 
        """removeMarker(self, i)"""
        return _loris.SdifFile_removeMarker(*args)

    def addMarker(*args): 
        """addMarker(self, m)"""
        return _loris.SdifFile_addMarker(*args)

    def clearMarkers(*args): 
        """clearMarkers(self)"""
        return _loris.SdifFile_clearMarkers(*args)

    def markers(*args): 
        """markers(self) -> MarkerVector"""
        return _loris.SdifFile_markers(*args)

    def addMarkers(*args): 
        """addMarkers(self, markers)"""
        return _loris.SdifFile_addMarkers(*args)


class SdifFilePtr(SdifFile):
    def __init__(self, this):
        _swig_setattr(self, SdifFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SdifFile, 'thisown', 0)
        _swig_setattr(self, SdifFile,self.__class__,SdifFile)
_loris.SdifFile_swigregister(SdifFilePtr)

class SpcFile(_object):
    """Proxy of C++ SpcFile class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SpcFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SpcFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ SpcFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_loris.delete_SpcFile):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def sampleRate(*args): 
        """sampleRate(self) -> double"""
        return _loris.SpcFile_sampleRate(*args)

    def midiNoteNumber(*args): 
        """midiNoteNumber(self) -> double"""
        return _loris.SpcFile_midiNoteNumber(*args)

    def addPartial(*args): 
        """
        addPartial(self, p)
        addPartial(self, p, label)
        """
        return _loris.SpcFile_addPartial(*args)

    def setMidiNoteNumber(*args): 
        """setMidiNoteNumber(self, nn)"""
        return _loris.SpcFile_setMidiNoteNumber(*args)

    def setSampleRate(*args): 
        """setSampleRate(self, rate)"""
        return _loris.SpcFile_setSampleRate(*args)

    def write(*args): 
        """
        write(self, filename, enhanced=True, endApproachTime=0)
        write(self, filename, enhanced=True)
        write(self, filename)
        """
        return _loris.SpcFile_write(*args)

    def __init__(self, *args):
        """
        __init__(self, filename) -> SpcFile
        __init__(self, midiNoteNum=60) -> SpcFile
        __init__(self) -> SpcFile
        __init__(self, l, midiNoteNum=60) -> SpcFile
        __init__(self, l) -> SpcFile
        """
        _swig_setattr(self, SpcFile, 'this', _loris.new_SpcFile(*args))
        _swig_setattr(self, SpcFile, 'thisown', 1)
    def partials(*args): 
        """partials(self) -> PartialList"""
        return _loris.SpcFile_partials(*args)

    def addPartials(*args): 
        """addPartials(self, l)"""
        return _loris.SpcFile_addPartials(*args)

    def numMarkers(*args): 
        """numMarkers(self) -> int"""
        return _loris.SpcFile_numMarkers(*args)

    def getMarker(*args): 
        """getMarker(self, i) -> Marker"""
        return _loris.SpcFile_getMarker(*args)

    def removeMarker(*args): 
        """removeMarker(self, i)"""
        return _loris.SpcFile_removeMarker(*args)

    def addMarker(*args): 
        """addMarker(self, m)"""
        return _loris.SpcFile_addMarker(*args)

    def clearMarkers(*args): 
        """clearMarkers(self)"""
        return _loris.SpcFile_clearMarkers(*args)

    def markers(*args): 
        """markers(self) -> MarkerVector"""
        return _loris.SpcFile_markers(*args)

    def addMarkers(*args): 
        """addMarkers(self, markers)"""
        return _loris.SpcFile_addMarkers(*args)


class SpcFilePtr(SpcFile):
    def __init__(self, this):
        _swig_setattr(self, SpcFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SpcFile, 'thisown', 0)
        _swig_setattr(self, SpcFile,self.__class__,SpcFile)
_loris.SpcFile_swigregister(SpcFilePtr)

class NewPlistIterator(_object):
    """Proxy of C++ NewPlistIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewPlistIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NewPlistIterator, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ NewPlistIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def atEnd(*args): 
        """atEnd(self) -> bool"""
        return _loris.NewPlistIterator_atEnd(*args)

    def next(*args): 
        """next(self) -> Partial"""
        return _loris.NewPlistIterator_next(*args)

    def partial(*args): 
        """partial(self) -> Partial"""
        return _loris.NewPlistIterator_partial(*args)


class NewPlistIteratorPtr(NewPlistIterator):
    def __init__(self, this):
        _swig_setattr(self, NewPlistIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewPlistIterator, 'thisown', 0)
        _swig_setattr(self, NewPlistIterator,self.__class__,NewPlistIterator)
_loris.NewPlistIterator_swigregister(NewPlistIteratorPtr)

class NewPartialIterator(_object):
    """Proxy of C++ NewPartialIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewPartialIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NewPartialIterator, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ NewPartialIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def atEnd(*args): 
        """atEnd(self) -> bool"""
        return _loris.NewPartialIterator_atEnd(*args)

    def hasNext(*args): 
        """hasNext(self) -> bool"""
        return _loris.NewPartialIterator_hasNext(*args)

    def next(*args): 
        """next(self) -> BreakpointPosition"""
        return _loris.NewPartialIterator_next(*args)


class NewPartialIteratorPtr(NewPartialIterator):
    def __init__(self, this):
        _swig_setattr(self, NewPartialIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewPartialIterator, 'thisown', 0)
        _swig_setattr(self, NewPartialIterator,self.__class__,NewPartialIterator)
_loris.NewPartialIterator_swigregister(NewPartialIteratorPtr)

class PartialList(_object):
    """Proxy of C++ PartialList class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialList, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialList, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ PartialList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> PartialList
        __init__(self, rhs) -> PartialList
        """
        _swig_setattr(self, PartialList, 'this', _loris.new_PartialList(*args))
        _swig_setattr(self, PartialList, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_PartialList):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args): 
        """clear(self)"""
        return _loris.PartialList_clear(*args)

    def size(*args): 
        """size(self) -> unsigned long"""
        return _loris.PartialList_size(*args)

    def timeSpan(*args): 
        """timeSpan(self, tmin_out, tmax_out)"""
        return _loris.PartialList_timeSpan(*args)

    def iterator(*args): 
        """iterator(self) -> NewPlistIterator"""
        return _loris.PartialList_iterator(*args)

    def __iter__(*args): 
        """__iter__(self) -> NewPlistIterator"""
        return _loris.PartialList___iter__(*args)

    def append(*args): 
        """append(self, partial)"""
        return _loris.PartialList_append(*args)

    def first(*args): 
        """first(self) -> Partial"""
        return _loris.PartialList_first(*args)

    def last(*args): 
        """last(self) -> Partial"""
        return _loris.PartialList_last(*args)

    def begin(*args): 
        """begin(self) -> PartialListIterator"""
        return _loris.PartialList_begin(*args)

    def end(*args): 
        """end(self) -> PartialListIterator"""
        return _loris.PartialList_end(*args)

    def erase(*args): 
        """
        erase(self, partial)
        erase(self, position)
        """
        return _loris.PartialList_erase(*args)

    def splice(*args): 
        """
        splice(self, other)
        splice(self, position, list)
        """
        return _loris.PartialList_splice(*args)

    def insert(*args): 
        """
        insert(self, position, partial) -> NewPlistIterator
        insert(self, position, partial) -> PartialListIterator
        insert(self, partial) -> PartialListIterator
        """
        return _loris.PartialList_insert(*args)

    def copy(*args): 
        """copy(self) -> PartialList"""
        return _loris.PartialList_copy(*args)


class PartialListPtr(PartialList):
    def __init__(self, this):
        _swig_setattr(self, PartialList, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialList, 'thisown', 0)
        _swig_setattr(self, PartialList,self.__class__,PartialList)
_loris.PartialList_swigregister(PartialListPtr)

class Partial(_object):
    """Proxy of C++ Partial class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Partial, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Partial, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Partial instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Partial
        __init__(self, ??) -> Partial
        """
        _swig_setattr(self, Partial, 'this', _loris.new_Partial(*args))
        _swig_setattr(self, Partial, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_Partial):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def label(*args): 
        """label(self) -> int"""
        return _loris.Partial_label(*args)

    def initialPhase(*args): 
        """initialPhase(self) -> double"""
        return _loris.Partial_initialPhase(*args)

    def startTime(*args): 
        """startTime(self) -> double"""
        return _loris.Partial_startTime(*args)

    def endTime(*args): 
        """endTime(self) -> double"""
        return _loris.Partial_endTime(*args)

    def duration(*args): 
        """duration(self) -> double"""
        return _loris.Partial_duration(*args)

    def numBreakpoints(*args): 
        """numBreakpoints(self) -> long"""
        return _loris.Partial_numBreakpoints(*args)

    def setLabel(*args): 
        """setLabel(self, l)"""
        return _loris.Partial_setLabel(*args)

    def frequencyAt(*args): 
        """frequencyAt(self, time) -> double"""
        return _loris.Partial_frequencyAt(*args)

    def amplitudeAt(*args): 
        """amplitudeAt(self, time) -> double"""
        return _loris.Partial_amplitudeAt(*args)

    def bandwidthAt(*args): 
        """bandwidthAt(self, time) -> double"""
        return _loris.Partial_bandwidthAt(*args)

    def phaseAt(*args): 
        """phaseAt(self, time) -> double"""
        return _loris.Partial_phaseAt(*args)

    def iterator(*args): 
        """iterator(self) -> NewPartialIterator"""
        return _loris.Partial_iterator(*args)

    def __iter__(*args): 
        """__iter__(self) -> NewPartialIterator"""
        return _loris.Partial___iter__(*args)

    def first(*args): 
        """first(self) -> Breakpoint"""
        return _loris.Partial_first(*args)

    def last(*args): 
        """last(self) -> Breakpoint"""
        return _loris.Partial_last(*args)

    def begin(*args): 
        """begin(self) -> PartialIterator"""
        return _loris.Partial_begin(*args)

    def end(*args): 
        """end(self) -> PartialIterator"""
        return _loris.Partial_end(*args)

    def erase(*args): 
        """
        erase(self, pos)
        erase(self, pos)
        """
        return _loris.Partial_erase(*args)

    def insert(*args): 
        """insert(self, time, bp) -> PartialIterator"""
        return _loris.Partial_insert(*args)

    def findAfter(*args): 
        """findAfter(self, time) -> PartialIterator"""
        return _loris.Partial_findAfter(*args)

    def findNearest(*args): 
        """findNearest(self, time) -> PartialIterator"""
        return _loris.Partial_findNearest(*args)

    def copy(*args): 
        """copy(self) -> Partial"""
        return _loris.Partial_copy(*args)


class PartialPtr(Partial):
    def __init__(self, this):
        _swig_setattr(self, Partial, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Partial, 'thisown', 0)
        _swig_setattr(self, Partial,self.__class__,Partial)
_loris.Partial_swigregister(PartialPtr)

class Breakpoint(_object):
    """Proxy of C++ Breakpoint class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Breakpoint, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Breakpoint, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Breakpoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, f, a, b, p=0.) -> Breakpoint
        __init__(self, f, a, b) -> Breakpoint
        __init__(self, rhs) -> Breakpoint
        """
        _swig_setattr(self, Breakpoint, 'this', _loris.new_Breakpoint(*args))
        _swig_setattr(self, Breakpoint, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_Breakpoint):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def frequency(*args): 
        """frequency(self) -> double"""
        return _loris.Breakpoint_frequency(*args)

    def amplitude(*args): 
        """amplitude(self) -> double"""
        return _loris.Breakpoint_amplitude(*args)

    def bandwidth(*args): 
        """bandwidth(self) -> double"""
        return _loris.Breakpoint_bandwidth(*args)

    def phase(*args): 
        """phase(self) -> double"""
        return _loris.Breakpoint_phase(*args)

    def setFrequency(*args): 
        """setFrequency(self, x)"""
        return _loris.Breakpoint_setFrequency(*args)

    def setAmplitude(*args): 
        """setAmplitude(self, x)"""
        return _loris.Breakpoint_setAmplitude(*args)

    def setBandwidth(*args): 
        """setBandwidth(self, x)"""
        return _loris.Breakpoint_setBandwidth(*args)

    def setPhase(*args): 
        """setPhase(self, x)"""
        return _loris.Breakpoint_setPhase(*args)

    def copy(*args): 
        """copy(self) -> Breakpoint"""
        return _loris.Breakpoint_copy(*args)


class BreakpointPtr(Breakpoint):
    def __init__(self, this):
        _swig_setattr(self, Breakpoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Breakpoint, 'thisown', 0)
        _swig_setattr(self, Breakpoint,self.__class__,Breakpoint)
_loris.Breakpoint_swigregister(BreakpointPtr)

class BreakpointPosition(_object):
    """Proxy of C++ BreakpointPosition class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BreakpointPosition, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BreakpointPosition, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ BreakpointPosition instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def time(*args): 
        """time(self) -> double"""
        return _loris.BreakpointPosition_time(*args)

    def breakpoint(*args): 
        """breakpoint(self) -> Breakpoint"""
        return _loris.BreakpointPosition_breakpoint(*args)

    def frequency(*args): 
        """frequency(self) -> double"""
        return _loris.BreakpointPosition_frequency(*args)

    def amplitude(*args): 
        """amplitude(self) -> double"""
        return _loris.BreakpointPosition_amplitude(*args)

    def bandwidth(*args): 
        """bandwidth(self) -> double"""
        return _loris.BreakpointPosition_bandwidth(*args)

    def phase(*args): 
        """phase(self) -> double"""
        return _loris.BreakpointPosition_phase(*args)

    def setFrequency(*args): 
        """setFrequency(self, x)"""
        return _loris.BreakpointPosition_setFrequency(*args)

    def setAmplitude(*args): 
        """setAmplitude(self, x)"""
        return _loris.BreakpointPosition_setAmplitude(*args)

    def setBandwidth(*args): 
        """setBandwidth(self, x)"""
        return _loris.BreakpointPosition_setBandwidth(*args)

    def setPhase(*args): 
        """setPhase(self, x)"""
        return _loris.BreakpointPosition_setPhase(*args)


class BreakpointPositionPtr(BreakpointPosition):
    def __init__(self, this):
        _swig_setattr(self, BreakpointPosition, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BreakpointPosition, 'thisown', 0)
        _swig_setattr(self, BreakpointPosition,self.__class__,BreakpointPosition)
_loris.BreakpointPosition_swigregister(BreakpointPositionPtr)

class PartialIterator(_object):
    """Proxy of C++ PartialIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ PartialIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def time(*args): 
        """time(self) -> double"""
        return _loris.PartialIterator_time(*args)

    def breakpoint(*args): 
        """breakpoint(self) -> Breakpoint"""
        return _loris.PartialIterator_breakpoint(*args)

    def copy(*args): 
        """copy(self) -> PartialIterator"""
        return _loris.PartialIterator_copy(*args)

    def next(*args): 
        """next(self) -> PartialIterator"""
        return _loris.PartialIterator_next(*args)

    def prev(*args): 
        """prev(self) -> PartialIterator"""
        return _loris.PartialIterator_prev(*args)

    def equals(*args): 
        """equals(self, other) -> int"""
        return _loris.PartialIterator_equals(*args)

    def isInRange(*args): 
        """isInRange(self, begin, end) -> int"""
        return _loris.PartialIterator_isInRange(*args)

    def __init__(self, *args):
        """__init__(self) -> PartialIterator"""
        _swig_setattr(self, PartialIterator, 'this', _loris.new_PartialIterator(*args))
        _swig_setattr(self, PartialIterator, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_PartialIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class PartialIteratorPtr(PartialIterator):
    def __init__(self, this):
        _swig_setattr(self, PartialIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialIterator, 'thisown', 0)
        _swig_setattr(self, PartialIterator,self.__class__,PartialIterator)
_loris.PartialIterator_swigregister(PartialIteratorPtr)

class PartialListIterator(_object):
    """Proxy of C++ PartialListIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialListIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialListIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ PartialListIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def copy(*args): 
        """copy(self) -> PartialListIterator"""
        return _loris.PartialListIterator_copy(*args)

    def next(*args): 
        """next(self) -> PartialListIterator"""
        return _loris.PartialListIterator_next(*args)

    def prev(*args): 
        """prev(self) -> PartialListIterator"""
        return _loris.PartialListIterator_prev(*args)

    def partial(*args): 
        """partial(self) -> Partial"""
        return _loris.PartialListIterator_partial(*args)

    def equals(*args): 
        """equals(self, other) -> int"""
        return _loris.PartialListIterator_equals(*args)

    def isInRange(*args): 
        """isInRange(self, begin, end) -> int"""
        return _loris.PartialListIterator_isInRange(*args)

    def __init__(self, *args):
        """__init__(self) -> PartialListIterator"""
        _swig_setattr(self, PartialListIterator, 'this', _loris.new_PartialListIterator(*args))
        _swig_setattr(self, PartialListIterator, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_PartialListIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class PartialListIteratorPtr(PartialListIterator):
    def __init__(self, this):
        _swig_setattr(self, PartialListIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialListIterator, 'thisown', 0)
        _swig_setattr(self, PartialListIterator,self.__class__,PartialListIterator)
_loris.PartialListIterator_swigregister(PartialListIteratorPtr)


