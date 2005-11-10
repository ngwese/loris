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

def collate(*args):
    """
    collate(partials)

    Collate unlabeled (zero-labeled) Partials into the smallest-possible 
    number of Partials that does not combine any overlapping Partials.
    Collated Partials assigned labels higher than any label in the original 
    list, and appear at the end of the sequence, after all previously-labeled
    Partials.
    """
    return _loris.collate(*args)

def createF0Estimate(*args):
    """
    createF0Estimate(partials, minFreq, maxFreq, interval) -> LinearEnvelope

    Return a newly-constructed LinearEnvelope that estimates
    the time-varying fundamental frequency of the sound
    represented by the Partials in a PartialList. This uses
    the experimental Fundamental class to construct an estimator
    of fundamental frequency, and returns a LinearEnvelope that
    samples the estimator at the specified time interval (in 
    seconds). Only estimates in the specified frequency range will 
    be considered valid, estimates outside this range will be 
    ignored.
    """
    return _loris.createF0Estimate(*args)

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

    Distill labeled (channelized) Partials in a PartialList into a 
    PartialList containing at most one Partial per label. Unlabeled 
    (zero-labeled) Partials are left unmodified at the end of the 
    distilled Partials.
    """
    return _loris.distill(*args)

def exportSdif(*args):
    """
    exportSdif(path, partials)

    Export Partials in a PartialList to a SDIF file at the specified
    file path (or name). SDIF data is written in the Loris RBEP
    format. For more information about SDIF, see the SDIF website at:
    	www.ircam.fr/equipes/analyse-synthese/sdif/  
    """
    return _loris.exportSdif(*args)

def importSdif(*args):
    """
    importSdif(path) -> PartialList

    Import Partials from an SDIF file at the given file path (or
    name), and return them in a PartialList. Loris can import
    SDIF data stored in 1TRC format or in the Loris RBEP format.
    For more information about SDIF, see the SDIF website at:
    	www.ircam.fr/equipes/analyse-synthese/sdif/
    """
    return _loris.importSdif(*args)

def importSpc(*args):
    """
    importSpc(path) -> PartialList

    Import Partials from an Spc file at the given file path (or
    name), and return them in a PartialList.
    """
    return _loris.importSpc(*args)

def setAmplitudeMorphShape(*args):
    """
    setAmplitudeMorphShape(shape)

    Set the shaping parameter for the amplitude morphing
    function. This shaping parameter controls the slope of
    the amplitude morphing function, for values greater than
    1, this function gets nearly linear (like the old
    amplitude morphing function), for values much less than
    1 (e.g. 1E-5) the slope is gently curved and sounds
    pretty 'linear', for very small values (e.g. 1E-12) the
    curve is very steep and sounds un-natural because of the
    huge jump from zero amplitude to very small amplitude.

    Use LORIS_DEFAULT_AMPMORPHSHAPE to obtain the default
    amplitude morphing shape for Loris, (equal to 1E-5,
    which works well for many musical instrument morphs,
    unless Loris was compiled with the symbol
    LINEAR_AMP_MORPHS defined, in which case
    LORIS_DEFAULT_AMPMORPHSHAPE is equal to
    LORIS_LINEAR_AMPMORPHSHAPE).

    Use LORIS_LINEAR_AMPMORPHSHAPE to approximate the linear
    amplitude morphs performed by older versions of Loris.

    The amplitude shape must be positive.
    """
    return _loris.setAmplitudeMorphShape(*args)

def crop(*args):
    """
    crop(partials, t1, t2)

    Trim Partials by removing Breakpoints outside a specified time span.
    Insert a Breakpoint at the boundary when cropping occurs.

    """
    return _loris.crop(*args)

def copyLabeled(*args):
    """
    copyLabeled(partials, label) -> PartialList

    Copy Partials in the source PartialList having the specified label
    into a new PartialList. The source PartialList is unmodified.

    """
    return _loris.copyLabeled(*args)

def extractLabeled(*args):
    """
    extractLabeled(partials, label) -> PartialList

    Extract Partials in the source PartialList having the specified
    label and return them in a new PartialList.
    """
    return _loris.extractLabeled(*args)

def removeLabeled(*args):
    """
    removeLabeled(partials, label)

    Remove from a PartialList all Partials having the specified label.
    """
    return _loris.removeLabeled(*args)

def resample(*args):
    """
    resample(partials, interval)

    Resample all Partials in a PartialList using the specified
    sampling interval, so that the Breakpoints in the Partial
    envelopes will all lie on a common temporal grid. The Breakpoint
    times in resampled Partials will comprise a contiguous sequence of
    integer multiples of the sampling interval, beginning with the
    multiple nearest to the Partial's start time and ending with the
    multiple nearest to the Partial's end time. Resampling is
    performed in-place.
    """
    return _loris.resample(*args)

def shiftTime(*args):
    """
    shiftTime(partials, offset)

    Shift the time of all the Breakpoints in a Partial by a constant
    amount (in seconds).
    """
    return _loris.shiftTime(*args)

def sift(*args):
    """
    sift(partials)

    Eliminate overlapping Partials having the same label
    (except zero). If any two partials with same label
    overlap in time, keep only the longer of the two.
    Set the label of the shorter duration partial to zero.
    """
    return _loris.sift(*args)

def sortByLabel(*args):
    """
    sortByLabel(partials)

    Sort the Partials in a PartialList in order of increasing label.
    The sort is stable; Partials having the same label are not
    reordered.
    """
    return _loris.sortByLabel(*args)

def timeSpan(*args):
    """
    timeSpan(partials, tmin_out, tmax_out)

    Return the minimum start time and maximum end time
    of all Partials in this PartialList.
    """
    return _loris.timeSpan(*args)

def version(*args):
    """
    version() -> char

    Return a string describing the Loris version number.
    """
    return _loris.version(*args)
class Marker(_object):
    """
    Proxy of C++ Marker class

    Class Marker represents a labeled time point in a set of Partials
    or a vector of samples. Collections of Markers (see the MarkerContainer
    definition below) are held by the File I/O classes in Loris (AiffFile,
    SdifFile, and SpcFile) to identify temporal features in imported
    and exported data.
    """
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

        Initialize a Marker with the specified time (in seconds) and name,
        or copy the time and name from another Marker. If unspecified, time 
        is zero and the label is empty.
        """
        _swig_setattr(self, Marker, 'this', _loris.new_Marker(*args))
        _swig_setattr(self, Marker, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_Marker):
        """
        __del__(self)

        Return a string describing the Loris version number.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def name(*args): 
        """
        name(self) -> char

        Return the name of this Marker.
        """
        return _loris.Marker_name(*args)

    def time(*args): 
        """
        time(self) -> double

        Return the time (in seconds) associated with this Marker.
        """
        return _loris.Marker_time(*args)

    def setName(*args): 
        """
        setName(self, s)

        Set the name of the Marker.
        """
        return _loris.Marker_setName(*args)

    def setTime(*args): 
        """
        setTime(self, t)

        Set the time (in seconds) associated with this Marker.
        """
        return _loris.Marker_setTime(*args)


class MarkerPtr(Marker):
    def __init__(self, this):
        _swig_setattr(self, Marker, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Marker, 'thisown', 0)
        _swig_setattr(self, Marker,self.__class__,Marker)
_loris.Marker_swigregister(MarkerPtr)

def createFreqReference(*args):
    """
    createFreqReference(partials, minFreq, maxFreq, numSamps) -> LinearEnvelope
    createFreqReference(partials, minFreq, maxFreq) -> LinearEnvelope

    Return a newly-constructed LinearEnvelope by sampling the
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
    exportAiff(path, samples, samplerate=44100.0, bitsPerSamp=16, 
        nchansignored=1)
    exportAiff(path, samples, samplerate=44100.0, bitsPerSamp=16)
    exportAiff(path, samples, samplerate=44100.0)
    exportAiff(path, samples)

    Export audio samples stored in a vector to an AIFF file having the
    specified number of channels and sample rate at the given file
    path (or name). The floating point samples in the vector are
    clamped to the range (-1.,1.) and converted to integers having
    bitsPerSamp bits. The default values for the sample rate and
    sample size, if unspecified, are 44100 Hz (CD quality) and 16 bits
    per sample, respectively.

    Only mono files can be exported, the last argument is ignored, 
    and is included only for backward compatability
    """
    return _loris.exportAiff(*args)

def exportSpc(*args):
    """
    exportSpc(path, partials, midiPitch, enhanced, endApproachTime)
    exportSpc(path, partials, midiPitch, enhanced)
    exportSpc(path, partials, midiPitch)

    Export Partials in a PartialList to a Spc file at the specified
    file path (or name). The fractional MIDI pitch must be specified.
    The optional enhanced parameter defaults to true (for
    bandwidth-enhanced spc files), but an be specified false for
    pure-sines spc files. The optional endApproachTime parameter is in
    seconds; its default value is zero (and has no effect). A nonzero
    endApproachTime indicates that the PartialList does not include a
    release, but rather ends in a static spectrum corresponding to the
    final breakpoint values of the partials. The endApproachTime
    specifies how long before the end of the sound the amplitude,
    frequency, and bandwidth values are to be modified to make a
    gradual transition to the static spectrum.
    """
    return _loris.exportSpc(*args)

def harmonify(*args):
    """
    harmonify(partials, refLabel, env, threshold_dB)
    harmonify(partials, refLabel, threshold_dB)

    Apply a reference Partial to fix the frequencies of Breakpoints
    whose amplitude is below threshold_dB. 0 harmonifies full-amplitude
    Partials, to apply only to quiet Partials, specify a lower 
    threshold like -90). The reference Partial is the first Partial
    in the PartialList labeled refLabel (usually 1). The LinearEnvelope,
    iif specified, is a time-varying weighting on the harmonifing process. 
    When 1, harmonic frequencies are used, when 0, breakpoint frequencies are 
    unmodified. 
    """
    return _loris.harmonify(*args)

def morph(*args):
    """
    morph(src0, src1, ffreq, famp, fbw) -> PartialList
    morph(src0, src1, freqweight, ampweight, bwweight) -> PartialList
    morph(src0, src1, src0RefLabel, src1RefLabel, ffreq, famp, 
        fbw) -> PartialList
    morph(src0, src1, src0RefLabel, src1RefLabel, freqweight, 
        ampweight, bwweight) -> PartialList

    Morph labeled Partials in two PartialLists according to the
    given frequency, amplitude, and bandwidth (noisiness) morphing
    envelopes, and return the morphed Partials in a PartialList.

    Optionally specify the labels of the Partials to be used as 
    reference Partial for the two morph sources. The reference 
    partial is used to compute frequencies for very low-amplitude 
    Partials whose frequency estimates are not considered reliable. 
    The reference Partial is considered to have good frequency 
    estimates throughout. A reference label of 0 indicates that 
    no reference Partial should be used for the corresponding
    morph source.

    Loris morphs Partials by interpolating frequency, amplitude,
    and bandwidth envelopes of corresponding Partials in the
    source PartialLists. For more information about the Loris
    morphing algorithm, see the Loris website:
    	www.cerlsoundgroup.org/Loris/
    """
    return _loris.morph(*args)
cvar = _loris.cvar
LORIS_DEFAULT_AMPMORPHSHAPE = cvar.LORIS_DEFAULT_AMPMORPHSHAPE
LORIS_LINEAR_AMPMORPHSHAPE = cvar.LORIS_LINEAR_AMPMORPHSHAPE

def synthesize(*args):
    """
    synthesize(partials, srate=44100.0) -> DoubleVector
    synthesize(partials) -> DoubleVector

    Synthesize Partials in a PartialList at the given sample rate, and
    return the (floating point) samples in a vector. The vector is
    sized to hold as many samples as are needed for the complete
    synthesis of all the Partials in the PartialList. If the sample
    rate is unspecified, the deault value of 44100 Hz (CD quality) is
    used.
    """
    return _loris.synthesize(*args)

def scaleAmp(*args):
    """
    scaleAmp(partials, ampEnv)
    scaleAmp(partials, val)

    Shift the pitch of all Partials in a PartialList according to 
    the given pitch envelope. The pitch envelope is assumed to have 
    units of cents (1/100 of a halfstep).
    """
    return _loris.scaleAmp(*args)

def scaleAmplitude(*args):
    """
    scaleAmplitude(partials, ampEnv)
    scaleAmplitude(partials, val)

    Shift the pitch of all Partials in a PartialList according to 
    the given pitch envelope. The pitch envelope is assumed to have 
    units of cents (1/100 of a halfstep).
    """
    return _loris.scaleAmplitude(*args)

def scaleBandwidth(*args):
    """
    scaleBandwidth(partials, bwEnv)
    scaleBandwidth(partials, val)

    Shift the pitch of all Partials in a PartialList according to 
    the given pitch envelope. The pitch envelope is assumed to have 
    units of cents (1/100 of a halfstep).
    """
    return _loris.scaleBandwidth(*args)

def scaleFrequency(*args):
    """
    scaleFrequency(partials, freqEnv)
    scaleFrequency(partials, val)

    Shift the pitch of all Partials in a PartialList according to 
    the given pitch envelope. The pitch envelope is assumed to have 
    units of cents (1/100 of a halfstep).
    """
    return _loris.scaleFrequency(*args)

def scaleNoiseRatio(*args):
    """
    scaleNoiseRatio(partials, noiseEnv)
    scaleNoiseRatio(partials, val)

    Shift the pitch of all Partials in a PartialList according to 
    the given pitch envelope. The pitch envelope is assumed to have 
    units of cents (1/100 of a halfstep).
    """
    return _loris.scaleNoiseRatio(*args)

def shiftPitch(*args):
    """
    shiftPitch(partials, pitchEnv)
    shiftPitch(partials, val)

    Shift the pitch of all Partials in a PartialList according to 
    the given pitch envelope. The pitch envelope is assumed to have 
    units of cents (1/100 of a halfstep).
    """
    return _loris.shiftPitch(*args)

class AiffFile(_object):
    """
    Proxy of C++ AiffFile class

    An AiffFile represents a sample file (on disk) in the Audio Interchange
    File Format. The file is read from disk and the samples stored in memory
    upon construction of an AiffFile instance. The samples are accessed by 
    the samples() method, which converts them to double precision floats and
    returns them in a vector.
    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AiffFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AiffFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ AiffFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_loris.delete_AiffFile):
        """
        __del__(self)

        Destroy this AiffFile.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def sampleRate(*args): 
        """
        sampleRate(self) -> double

        Return the sample rate in Hz for this AiffFile.
        """
        return _loris.AiffFile_sampleRate(*args)

    def midiNoteNumber(*args): 
        """
        midiNoteNumber(self) -> double

        Return the MIDI note number for this AiffFile. The defaul
        note number is 60, corresponding to middle C.
        """
        return _loris.AiffFile_midiNoteNumber(*args)

    def sampleFrames(*args): 
        """
        sampleFrames(self) -> unsigned long

        Return the number of sample frames (equal to the number of samples
        in a single channel file) stored by this AiffFile.
        """
        return _loris.AiffFile_sampleFrames(*args)

    def addPartial(*args): 
        """
        addPartial(self, p, fadeTime=.001)
        addPartial(self, p)

        Render the specified Partial using the (optionally) specified
        Partial fade time, and accumulate the resulting samples into
        the sample vector for this AiffFile.
        """
        return _loris.AiffFile_addPartial(*args)

    def setMidiNoteNumber(*args): 
        """
        setMidiNoteNumber(self, nn)

        Set the fractional MIDI note number assigned to this AiffFile. 
        If the sound has no definable pitch, use note number 60.0 
        (the default).
        """
        return _loris.AiffFile_setMidiNoteNumber(*args)

    def write(*args): 
        """
        write(self, filename, bps=16)
        write(self, filename)

        Export the sample data represented by this AiffFile to
        the file having the specified filename or path. Export
        signed integer samples of the specified size, in bits
        (8, 16, 24, or 32).
        """
        return _loris.AiffFile_write(*args)

    def __init__(self, *args):
        """
        __init__(self, filename) -> AiffFile
        __init__(self, vec, samplerate) -> AiffFile
        __init__(self, l, sampleRate=44100, fadeTime=.001) -> AiffFile
        __init__(self, l, sampleRate=44100) -> AiffFile
        __init__(self, l) -> AiffFile

        An AiffFile instance can be initialized in any of the following ways:

        Initialize a new AiffFile from a vector of samples and sample rate.

        Initialize a new AiffFile using data read from a named file.

        Initialize an instance of AiffFile having the specified sample 
        rate, accumulating samples rendered at that sample rate from
        all Partials on the specified half-open (STL-style) range with
        the (optionally) specified Partial fade time (see Synthesizer.h
        for an examplanation of fade time). 

        """
        _swig_setattr(self, AiffFile, 'this', _loris.new_AiffFile(*args))
        _swig_setattr(self, AiffFile, 'thisown', 1)
    def samples(*args): 
        """
        samples(self) -> DoubleVector

        Return a copy of the samples (as floating point numbers
        on the range -1,1) stored in this AiffFile.
        """
        return _loris.AiffFile_samples(*args)

    def channels(*args): 
        """
        channels(self) -> int

        The number of channels is always 1. 
        Loris only deals in mono AiffFiles
        """
        return _loris.AiffFile_channels(*args)

    def addPartials(*args): 
        """
        addPartials(self, l, fadeTime=0.001)
        addPartials(self, l)

        Render all Partials on the specified half-open (STL-style) range
        with the (optionally) specified Partial fade time (see Synthesizer.h
        for an examplanation of fade time), and accumulate the resulting 
        samples.
        """
        return _loris.AiffFile_addPartials(*args)

    def markers(*args): 
        """
        markers(self) -> MarkerVector

        Return the (possibly empty) collection of Markers for 
        this AiffFile.
        """
        return _loris.AiffFile_markers(*args)

    def addMarkers(*args): 
        """
        addMarkers(self, markers)

        Append a collection of Markers for to the existing
        set of Markers for this AiffFile.
        """
        return _loris.AiffFile_addMarkers(*args)

    def setMarkers(*args): 
        """
        setMarkers(self, markers)

        Specify a new (possibly empty) collection of Markers for
        this AiffFile.
        """
        return _loris.AiffFile_setMarkers(*args)


class AiffFilePtr(AiffFile):
    def __init__(self, this):
        _swig_setattr(self, AiffFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AiffFile, 'thisown', 0)
        _swig_setattr(self, AiffFile,self.__class__,AiffFile)
_loris.AiffFile_swigregister(AiffFilePtr)

class Analyzer(_object):
    """
    Proxy of C++ Analyzer class

    An Analyzer represents a configuration of parameters for
    performing Reassigned Bandwidth-Enhanced Additive Analysis
    of sampled waveforms. This analysis process yields a collection 
    of Partials, each having a trio of synchronous, non-uniformly-
    sampled breakpoint envelopes representing the time-varying 
    frequency, amplitude, and noisiness of a single bandwidth-
    enhanced sinusoid. 

    For more information about Reassigned Bandwidth-Enhanced 
    Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
    Model, refer to the Loris website: 

    	http://www.cerlsoundgroup.org/Loris/

    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Analyzer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Analyzer, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Analyzer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, resolutionHz) -> Analyzer
        __init__(self, resolutionHz, windowWidthHz) -> Analyzer
        __init__(self, another) -> Analyzer

        Construct and return a new Analyzer configured with the given	
        frequency resolution (minimum instantaneous frequency	
        difference between Partials) and analysis window main 
        lobe width (between zeros). All other Analyzer parameters 	
        are computed from the specified resolution and window
        width. If the window width is not specified, 
        then it is assumed to be equal to the resolution.

        An Analyzer configuration can also be copied from another
        instance.
        """
        _swig_setattr(self, Analyzer, 'this', _loris.new_Analyzer(*args))
        _swig_setattr(self, Analyzer, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_Analyzer):
        """
        __del__(self)

        Destroy this Analyzer.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def analyze(*args): 
        """
        analyze(self, vec, srate) -> PartialList
        analyze(self, vec, srate, env) -> PartialList

        Analyze a vector of (mono) samples at the given sample rate 	  	
        (in Hz) and return the resulting Partials in a PartialList.
        If specified, use a frequency envelope as a fundamental reference for
        Partial formation.
        """
        return _loris.Analyzer_analyze(*args)

    def ampFloor(*args): 
        """
        ampFloor(self) -> double

        Return the amplitude floor (lowest detected spectral amplitude),              
        in (negative) dB, for this Analyzer.
        """
        return _loris.Analyzer_ampFloor(*args)

    def bwRegionWidth(*args): 
        """
        bwRegionWidth(self) -> double

        Return the width (in Hz) of the Bandwidth Association regions
        used by this Analyzer.
        """
        return _loris.Analyzer_bwRegionWidth(*args)

    def cropTime(*args): 
        """
        cropTime(self) -> double

        Return the crop time (maximum temporal displacement of a time-
        frequency data point from the time-domain center of the analysis
        window, beyond which data points are considered "unreliable")
        for this Analyzer.
        """
        return _loris.Analyzer_cropTime(*args)

    def freqDrift(*args): 
        """
        freqDrift(self) -> double

        Return the maximum allowable frequency difference between                     
        consecutive Breakpoints in a Partial envelope for this Analyzer.
        """
        return _loris.Analyzer_freqDrift(*args)

    def freqFloor(*args): 
        """
        freqFloor(self) -> double

        Return the frequency floor (minimum instantaneous Partial                  
        frequency), in Hz, for this Analyzer.
        """
        return _loris.Analyzer_freqFloor(*args)

    def freqResolution(*args): 
        """
        freqResolution(self) -> double

        Return the frequency resolution (minimum instantaneous frequency          
        difference between Partials) for this Analyzer.
        """
        return _loris.Analyzer_freqResolution(*args)

    def hopTime(*args): 
        """
        hopTime(self) -> double

        Return the hop time (which corresponds approximately to the 
        average density of Partial envelope Breakpoint data) for this 
        Analyzer.
        """
        return _loris.Analyzer_hopTime(*args)

    def sidelobeLevel(*args): 
        """
        sidelobeLevel(self) -> double

        Return the sidelobe attenutation level for the Kaiser analysis window in
        positive dB. Higher numbers (e.g. 90) give very good sidelobe 
        rejection but cause the window to be longer in time. Smaller 
        numbers raise the level of the sidelobes, increasing the likelihood
        of frequency-domain interference, but allow the window to be shorter
        in time.
        """
        return _loris.Analyzer_sidelobeLevel(*args)

    def windowWidth(*args): 
        """
        windowWidth(self) -> double

        Return the frequency-domain main lobe width (measured between 
        zero-crossings) of the analysis window used by this Analyzer.
        """
        return _loris.Analyzer_windowWidth(*args)

    def setAmpFloor(*args): 
        """
        setAmpFloor(self, x)

        Set the amplitude floor (lowest detected spectral amplitude), in              
        (negative) dB, for this Analyzer.
        """
        return _loris.Analyzer_setAmpFloor(*args)

    def setBwRegionWidth(*args): 
        """
        setBwRegionWidth(self, x)

        Set the width (in Hz) of the Bandwidth Association regions
        used by this Analyzer.
        """
        return _loris.Analyzer_setBwRegionWidth(*args)

    def setCropTime(*args): 
        """
        setCropTime(self, x)

        Set the crop time (maximum temporal displacement of a time-
        frequency data point from the time-domain center of the analysis
        window, beyond which data points are considered "unreliable")
        for this Analyzer.
        """
        return _loris.Analyzer_setCropTime(*args)

    def setFreqDrift(*args): 
        """
        setFreqDrift(self, x)

        Set the maximum allowable frequency difference between                     
        consecutive Breakpoints in a Partial envelope for this Analyzer.
        """
        return _loris.Analyzer_setFreqDrift(*args)

    def setFreqFloor(*args): 
        """
        setFreqFloor(self, x)

        Set the amplitude floor (minimum instantaneous Partial                  
        frequency), in Hz, for this Analyzer.
        """
        return _loris.Analyzer_setFreqFloor(*args)

    def setFreqResolution(*args): 
        """
        setFreqResolution(self, x)

        Set the frequency resolution (minimum instantaneous frequency          
        difference between Partials) for this Analyzer. (Does not cause     
        other parameters to be recomputed.)
        """
        return _loris.Analyzer_setFreqResolution(*args)

    def setHopTime(*args): 
        """
        setHopTime(self, x)

        Set the hop time (which corresponds approximately to the average
        density of Partial envelope Breakpoint data) for this Analyzer.
        """
        return _loris.Analyzer_setHopTime(*args)

    def setSidelobeLevel(*args): 
        """
        setSidelobeLevel(self, x)

        Set the sidelobe attenutation level for the Kaiser analysis window in
        positive dB. Larger numbers (e.g. 90) give very good sidelobe 
        rejection but cause the window to be longer in time. Smaller 
        numbers raise the level of the sidelobes, increasing the likelihood
        of frequency-domain interference, but allow the window to be shorter
        in time.
        """
        return _loris.Analyzer_setSidelobeLevel(*args)

    def setWindowWidth(*args): 
        """
        setWindowWidth(self, x)

        Set the frequency-domain main lobe width (measured between 
        zero-crossings) of the analysis window used by this Analyzer.
        """
        return _loris.Analyzer_setWindowWidth(*args)


class AnalyzerPtr(Analyzer):
    def __init__(self, this):
        _swig_setattr(self, Analyzer, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Analyzer, 'thisown', 0)
        _swig_setattr(self, Analyzer,self.__class__,Analyzer)
_loris.Analyzer_swigregister(AnalyzerPtr)

class LinearEnvelope(_object):
    """
    Proxy of C++ LinearEnvelope class

    A LinearEnvelope represents a linear segment breakpoint 
    function with infinite extension at each end (that is, the 
    values past either end of the breakpoint function have the 
    values at the nearest end).
    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, LinearEnvelope, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, LinearEnvelope, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ LinearEnvelope instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> LinearEnvelope
        __init__(self, ??) -> LinearEnvelope
        __init__(self, initialValue) -> LinearEnvelope

        Construct and return a new LinearEnvelope, empty,
        or having a single breakpoint at time 0 with the 
        specified value.

        An LinearEnvelope can also be copied from another
        instance.
        """
        _swig_setattr(self, LinearEnvelope, 'this', _loris.new_LinearEnvelope(*args))
        _swig_setattr(self, LinearEnvelope, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_LinearEnvelope):
        """
        __del__(self)

        Destroy this LinearEnvelope.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def insertBreakpoint(*args): 
        """
        insertBreakpoint(self, time, value)

        Insert a new breakpoint into the envelope at the specified
        time and value.
        """
        return _loris.LinearEnvelope_insertBreakpoint(*args)

    def insert(*args): 
        """
        insert(self, time, value)

        Insert a new breakpoint into the envelope at the specified
        time and value.
        """
        return _loris.LinearEnvelope_insert(*args)

    def valueAt(*args): 
        """
        valueAt(self, x) -> double

        Return the (linearly-interpolated) value of the envelope
        at the specified time.
        """
        return _loris.LinearEnvelope_valueAt(*args)


class LinearEnvelopePtr(LinearEnvelope):
    def __init__(self, this):
        _swig_setattr(self, LinearEnvelope, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, LinearEnvelope, 'thisown', 0)
        _swig_setattr(self, LinearEnvelope,self.__class__,LinearEnvelope)
_loris.LinearEnvelope_swigregister(LinearEnvelopePtr)

class BreakpointEnvelope(LinearEnvelope):
    """
    Proxy of C++ BreakpointEnvelope class

    BreakpointEnvelope is deprecated, use LinearEnvelope instead.
    """
    __swig_setmethods__ = {}
    for _s in [LinearEnvelope]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BreakpointEnvelope, name, value)
    __swig_getmethods__ = {}
    for _s in [LinearEnvelope]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BreakpointEnvelope, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ BreakpointEnvelope instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> BreakpointEnvelope
        __init__(self, ??) -> BreakpointEnvelope
        __init__(self, initialValue) -> BreakpointEnvelope

        BreakpointEnvelope is deprecated, use LinearEnvelope instead.
        """
        _swig_setattr(self, BreakpointEnvelope, 'this', _loris.new_BreakpointEnvelope(*args))
        _swig_setattr(self, BreakpointEnvelope, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_BreakpointEnvelope):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def insertBreakpoint(*args): 
        """
        insertBreakpoint(self, time, value)

        Insert a new breakpoint into the envelope at the specified
        time and value.
        """
        return _loris.BreakpointEnvelope_insertBreakpoint(*args)

    def valueAt(*args): 
        """
        valueAt(self, x) -> double

        Return the (linearly-interpolated) value of the envelope
        at the specified time.
        """
        return _loris.BreakpointEnvelope_valueAt(*args)


class BreakpointEnvelopePtr(BreakpointEnvelope):
    def __init__(self, this):
        _swig_setattr(self, BreakpointEnvelope, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BreakpointEnvelope, 'thisown', 0)
        _swig_setattr(self, BreakpointEnvelope,self.__class__,BreakpointEnvelope)
_loris.BreakpointEnvelope_swigregister(BreakpointEnvelopePtr)


def BreakpointEnvelopeWithValue(*args):
    """
    BreakpointEnvelopeWithValue(initialValue) -> LinearEnvelope

    BreakpointEnvelopeWithValue is deprecated, use LinearEnvelope instead.
    """
    return _loris.BreakpointEnvelopeWithValue(*args)
class SdifFile(_object):
    """
    Proxy of C++ SdifFile class

    Class SdifFile represents reassigned bandwidth-enhanced Partial 
    data in a SDIF-format data file. Construction of an SdifFile 
    from a stream or filename automatically imports the Partial
    data.
    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SdifFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SdifFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ SdifFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_loris.delete_SdifFile):
        """
        __del__(self)

        Destroy this SdifFile.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def write(*args): 
        """
        write(self, path)

        Export the Partials represented by this SdifFile to
        the file having the specified filename or path.
        """
        return _loris.SdifFile_write(*args)

    def write1TRC(*args): 
        """
        write1TRC(self, path)

        Export the envelope Partials represented by this SdifFile to
        the file having the specified filename or path in the 1TRC
        format, resampled, and without phase or bandwidth information.
        """
        return _loris.SdifFile_write1TRC(*args)

    def __init__(self, *args):
        """
        __init__(self, filename) -> SdifFile
        __init__(self) -> SdifFile
        __init__(self, l) -> SdifFile

        Initialize an instance of SdifFile by importing Partial data from
        the file having the specified filename or path, 
        or initialize an instance of SdifFile storing the Partials in
        the specified PartialList. If no PartialList is specified,
        construct an empty SdifFile.
        """
        _swig_setattr(self, SdifFile, 'this', _loris.new_SdifFile(*args))
        _swig_setattr(self, SdifFile, 'thisown', 1)
    def partials(*args): 
        """
        partials(self) -> PartialList

        Return a copy of the Partials represented by this SdifFile.
        """
        return _loris.SdifFile_partials(*args)

    def addPartials(*args): 
        """
        addPartials(self, l)

        Add all the Partials in a PartialList to this SdifFile.
        """
        return _loris.SdifFile_addPartials(*args)

    def markers(*args): 
        """
        markers(self) -> MarkerVector

        Return the (possibly empty) collection of Markers for 
        this SdifFile.
        """
        return _loris.SdifFile_markers(*args)

    def addMarkers(*args): 
        """
        addMarkers(self, markers)

        Append a collection of Markers for to the existing
        set of Markers for this SdifFile.
        """
        return _loris.SdifFile_addMarkers(*args)

    def setMarkers(*args): 
        """
        setMarkers(self, markers)

        Specify a new (possibly empty) collection of Markers for
        this SdifFile.
        """
        return _loris.SdifFile_setMarkers(*args)


class SdifFilePtr(SdifFile):
    def __init__(self, this):
        _swig_setattr(self, SdifFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SdifFile, 'thisown', 0)
        _swig_setattr(self, SdifFile,self.__class__,SdifFile)
_loris.SdifFile_swigregister(SdifFilePtr)

class SpcFile(_object):
    """
    Proxy of C++ SpcFile class

    Class SpcFile represents a collection of reassigned bandwidth-enhanced
    Partial data in a SPC-format envelope stream data file, used by the
    real-time bandwidth-enhanced additive synthesizer implemented on the
    Symbolic Sound Kyma Sound Design Workstation. Class SpcFile manages 
    file I/O and conversion between Partials and envelope parameter streams.
    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SpcFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SpcFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ SpcFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_loris.delete_SpcFile):
        """
        __del__(self)

        Destroy this SpcFile.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def sampleRate(*args): 
        """
        sampleRate(self) -> double

        Return the sample rate for this SpcFile in Hz.
        """
        return _loris.SpcFile_sampleRate(*args)

    def midiNoteNumber(*args): 
        """midiNoteNumber(self) -> double"""
        return _loris.SpcFile_midiNoteNumber(*args)

    def addPartial(*args): 
        """
        addPartial(self, p)
        addPartial(self, p, label)

        Add the specified Partial to the enevelope parameter streams
        represented by this SpcFile. If a label is specified, use that
        label, instead of the Partial's label, for the Partial added to
        the SpcFile.

        A SpcFile can contain only one Partial having any given (non-zero) 
        label, so an added Partial will replace a Partial having the 
        same label, if such a Partial exists.

        This may throw an InvalidArgument exception if an attempt is made
        to add unlabeled Partials, or Partials labeled higher than the
        allowable maximum.
        """
        return _loris.SpcFile_addPartial(*args)

    def setMidiNoteNumber(*args): 
        """
        setMidiNoteNumber(self, nn)

        Set the fractional MIDI note number assigned to this SpcFile. 
        If the sound has no definable pitch, use note number 60.0 (the default).
        """
        return _loris.SpcFile_setMidiNoteNumber(*args)

    def setSampleRate(*args): 
        """
        setSampleRate(self, rate)

        Set the sampling freqency in Hz for the spc data in this
        SpcFile. This is the rate at which Kyma must be running to ensure
        proper playback of bandwidth-enhanced Spc data.
        The default sample rate is 44100 Hz.
        """
        return _loris.SpcFile_setSampleRate(*args)

    def write(*args): 
        """
        write(self, filename, enhanced=True, endApproachTime=0)
        write(self, filename, enhanced=True)
        write(self, filename)

        Export the envelope parameter streams represented by this SpcFile to
        the file having the specified filename or path. Export phase-correct 
        bandwidth-enhanced envelope parameter streams if enhanced is true 
        (the default), or pure sinsoidal streams otherwise.

        A nonzero endApproachTime indicates that the Partials do not include a
        release or decay, but rather end in a static spectrum corresponding to the
        final Breakpoint values of the partials. The endApproachTime specifies how
        long before the end of the sound the amplitude, frequency, and bandwidth
        values are to be modified to make a gradual transition to the static spectrum.

        If the endApproachTime is not specified, it is assumed to be zero, 
        corresponding to Partials that decay or release normally.
        """
        return _loris.SpcFile_write(*args)

    def __init__(self, *args):
        """
        __init__(self, filename) -> SpcFile
        __init__(self, midiNoteNum=60) -> SpcFile
        __init__(self) -> SpcFile
        __init__(self, l, midiNoteNum=60) -> SpcFile
        __init__(self, l) -> SpcFile

        Construct and return a new SpcFile by importing envelope parameter 
        streams from the file having the specified filename or path, 
        or initialize an instance of SpcFile having the specified fractional
        MIDI note number. If a PartialList is specified, add those
        Partials to the file. Otherwise, the new SpcFile contains 
        no Partials (or envelope parameter streams).
        The default MIDI note number is 60 (middle C).
        """
        _swig_setattr(self, SpcFile, 'this', _loris.new_SpcFile(*args))
        _swig_setattr(self, SpcFile, 'thisown', 1)
    def partials(*args): 
        """
        partials(self) -> PartialList

        Return a copy of the Partials represented by this SdifFile.
        """
        return _loris.SpcFile_partials(*args)

    def addPartials(*args): 
        """
        addPartials(self, l)

        Add all the Partials in a PartialList to this SpcFile.
        			
        A SpcFile can contain only one Partial having any given (non-zero) 
        label, so an added Partial will replace a Partial having the 
        same label, if such a Partial exists.

        This may throw an InvalidArgument exception if an attempt is made
        to add unlabeled Partials, or Partials labeled higher than the
        allowable maximum.
        """
        return _loris.SpcFile_addPartials(*args)

    def markers(*args): 
        """
        markers(self) -> MarkerVector

        Return the (possibly empty) collection of Markers for 
        this SpcFile.
        """
        return _loris.SpcFile_markers(*args)

    def addMarkers(*args): 
        """
        addMarkers(self, markers)

        Append a collection of Markers for to the existing
        set of Markers for this SpcFile.
        """
        return _loris.SpcFile_addMarkers(*args)

    def setMarkers(*args): 
        """
        setMarkers(self, markers)

        Specify a new (possibly empty) collection of Markers for
        this SpcFile.
        """
        return _loris.SpcFile_setMarkers(*args)


class SpcFilePtr(SpcFile):
    def __init__(self, this):
        _swig_setattr(self, SpcFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SpcFile, 'thisown', 0)
        _swig_setattr(self, SpcFile,self.__class__,SpcFile)
_loris.SpcFile_swigregister(SpcFilePtr)

class PartialListIterator(_object):
    """
    Proxy of C++ PartialListIterator class

    An iterator over a PartialList. Access Partials
    in a PartialList by invoking next until atEnd 
    returns true.
    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialListIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialListIterator, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ SwigPListIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def atEnd(*args): 
        """
        atEnd(self) -> bool

        Return true if there are no more Partials in the PartialList.
        """
        return _loris.PartialListIterator_atEnd(*args)

    def next(*args): 
        """
        next(self) -> Partial

        Return the next Partial in the PartialList that has not yet
        been returned by this iterator.
        """
        return _loris.PartialListIterator_next(*args)


class PartialListIteratorPtr(PartialListIterator):
    def __init__(self, this):
        _swig_setattr(self, PartialListIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialListIterator, 'thisown', 0)
        _swig_setattr(self, PartialListIterator,self.__class__,PartialListIterator)
_loris.PartialListIterator_swigregister(PartialListIteratorPtr)

class PartialIterator(_object):
    """Proxy of C++ PartialIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialIterator, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ SwigPartialIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def atEnd(*args): 
        """
        atEnd(self) -> bool

        Return true if there are no more Breakpoints in the Partial.
        """
        return _loris.PartialIterator_atEnd(*args)

    def next(*args): 
        """
        next(self) -> BreakpointPosition

        Return the next Breakpoint in the Partial that has not yet
        been returned by this iterator.
        """
        return _loris.PartialIterator_next(*args)


class PartialIteratorPtr(PartialIterator):
    def __init__(self, this):
        _swig_setattr(self, PartialIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialIterator, 'thisown', 0)
        _swig_setattr(self, PartialIterator,self.__class__,PartialIterator)
_loris.PartialIterator_swigregister(PartialIteratorPtr)

class PartialList(_object):
    """
    Proxy of C++ PartialList class

    A PartialList represents a collection of Bandwidth-Enhanced 
    Partials, each having a trio of synchronous, non-uniformly-
    sampled breakpoint envelopes representing the time-varying 
    frequency, amplitude, and noisiness of a single bandwidth-
    enhanced sinusoid.

    For more information about Bandwidth-Enhanced Partials and the  
    Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
    the Loris website: www.cerlsoundgroup.org/Loris/
    """
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

        Construct a new empty PartialList, or a PartialList
        that is a copy of another (containing identical copies
        of the Partials in another).
        """
        _swig_setattr(self, PartialList, 'this', _loris.new_PartialList(*args))
        _swig_setattr(self, PartialList, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_PartialList):
        """
        __del__(self)

        Delete this PartialList.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args): 
        """
        clear(self)

        Remove all the Partials from this PartialList.
        """
        return _loris.PartialList_clear(*args)

    def size(*args): 
        """
        size(self) -> unsigned long

        Return the number of Partials in this PartialList.
        """
        return _loris.PartialList_size(*args)

    def iterator(*args): 
        """
        iterator(self) -> PartialListIterator

        Return an iterator on the Partials in this PartialList.
        """
        return _loris.PartialList_iterator(*args)

    def __iter__(*args): 
        """
        __iter__(self) -> PartialListIterator

        Return an iterator on the Partials in this PartialList.
        """
        return _loris.PartialList___iter__(*args)

    def __len__(*args): 
        """
        __len__(self) -> unsigned long

        Return the number of Partials in this PartialList.
        """
        return _loris.PartialList___len__(*args)

    def append(*args): 
        """
        append(self, partial)
        append(self, other)

        Append a copy of a Partial, or copies of all the Partials in
        another PartialList, to this PartialList.
        """
        return _loris.PartialList_append(*args)

    def remove(*args): 
        """
        remove(self, partial)

        Remove the specified Partial from this PartialList. An
        exception is raised if the specified Partial is not a member
        of this PartialList. The Partial itself must be a member, not
        merely identical to a Partial in this PartialList.
        """
        return _loris.PartialList_remove(*args)

    def first(*args): 
        """
        first(self) -> Partial

        Return the first Partial this PartialList, or 0 if this
        PartialList is empty.
        """
        return _loris.PartialList_first(*args)

    def last(*args): 
        """
        last(self) -> Partial

        Return the last Partial this PartialList, or 0 if this
        PartialList is empty.
        """
        return _loris.PartialList_last(*args)

    def copy(*args): 
        """
        copy(self) -> PartialList

        Construct a new a PartialList that is a copy of 
        another (containing identical copies of the Partials 
        in another). 

        This member is deprecated, use the normal copy constructor:
           plist_copy = PartialList( plist )

        """
        return _loris.PartialList_copy(*args)


class PartialListPtr(PartialList):
    def __init__(self, this):
        _swig_setattr(self, PartialList, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialList, 'thisown', 0)
        _swig_setattr(self, PartialList,self.__class__,PartialList)
_loris.PartialList_swigregister(PartialListPtr)

class Partial(_object):
    """
    Proxy of C++ Partial class

    A Partial represents a single component in the
    reassigned bandwidth-enhanced additive model. A Partial
    consists of a chain of Breakpoints describing the
    time-varying frequency, amplitude, and bandwidth (or
    noisiness) envelopes of the component, and a 4-byte
    label. The Breakpoints are non-uniformly distributed in
    time. For more information about Reassigned
    Bandwidth-Enhanced Analysis and the Reassigned
    Bandwidth-Enhanced Additive Sound Model, refer to the
    Loris website: 
        www.cerlsoundgroup.org/Loris/

    """
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

        Construct a new empty Partial, having no Breakpoints,
        or a Partial that is a copy of another (containing
        identical of the Breakpoints in another).
        """
        _swig_setattr(self, Partial, 'this', _loris.new_Partial(*args))
        _swig_setattr(self, Partial, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_Partial):
        """
        __del__(self)

        Delete this Partial.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def label(*args): 
        """
        label(self) -> int

        Return the label (an integer) for this Partial. The
        default label is 0.
        """
        return _loris.Partial_label(*args)

    def initialPhase(*args): 
        """
        initialPhase(self) -> double

        Return the starting phase (in radians) for this
        Partial. An exception is raised if there are no
        Breakpoints in this Partial.
        """
        return _loris.Partial_initialPhase(*args)

    def startTime(*args): 
        """
        startTime(self) -> double

        Return the time (in seconds) of the first Breakpoint in
        this Partial. An exception is raised if there are no
        Breakpoints in this Partial.
        """
        return _loris.Partial_startTime(*args)

    def endTime(*args): 
        """
        endTime(self) -> double

        Return the time (in seconds) of the last Breakpoint in
        this Partial. An exception is raised if there are no
        Breakpoints in this Partial.
        """
        return _loris.Partial_endTime(*args)

    def duration(*args): 
        """
        duration(self) -> double

        Return the difference in time (in seconds) between the 
        first and last Breakpoints in this Partial.
        """
        return _loris.Partial_duration(*args)

    def numBreakpoints(*args): 
        """
        numBreakpoints(self) -> long

        Return the number of Breakpoints in this Partial.
        """
        return _loris.Partial_numBreakpoints(*args)

    def setLabel(*args): 
        """
        setLabel(self, l)

        Set the label (an integer) for this Partial. Unlabeled
        Partials have the default label of 0.
        """
        return _loris.Partial_setLabel(*args)

    def frequencyAt(*args): 
        """
        frequencyAt(self, time) -> double

        Return the interpolated frequency (in Hz) of this
        Partial at the specified time in seconds. The frequency
        at times earlier than the first Breakpoint is the
        frequency of the first Breakpoint. The frequency at
        times later than the last Breakpoint is the frequency of
        the last Breakpoint. An exception is raised if there are
        no Breakpoints in this Partial.
        """
        return _loris.Partial_frequencyAt(*args)

    def amplitudeAt(*args): 
        """
        amplitudeAt(self, time) -> double

        Return the interpolated amplitude of this Partial at
        the specified time in seconds. The amplitude at times
        earlier than the first Breakpoint and at times later
        than the last Breakpoint is zero. An exception is raised
        if there are no Breakpoints in this Partial.
        """
        return _loris.Partial_amplitudeAt(*args)

    def bandwidthAt(*args): 
        """
        bandwidthAt(self, time) -> double

        Return the interpolated bandwidth (between 0 and 1) of
        this Partial at the specified time in seconds. The
        bandwidth at times earlier than the first Breakpoint and
        at times later than the last Breakpoint is zero. An
        exception is raised if there are no Breakpoints in this
        Partial.
        """
        return _loris.Partial_bandwidthAt(*args)

    def phaseAt(*args): 
        """
        phaseAt(self, time) -> double

        Return the interpolated phase (in radians) of this
        Partial at the specified time in seconds. The phase at
        times earlier than the first Breakpoint is extrapolated
        from phase of the first Breakpoint assuming constant
        frequency. The phase at times later than the last
        Breakpoint is the extrapolated from the phase of the
        last Breakpoint assuming constant frequency. An
        exception is raised if there are no Breakpoints in this
        Partial.
        """
        return _loris.Partial_phaseAt(*args)

    def iterator(*args): 
        """
        iterator(self) -> PartialIterator

        Return an iterator on the BreakpointPositions in this
        Partial.
        """
        return _loris.Partial_iterator(*args)

    def __iter__(*args): 
        """
        __iter__(self) -> PartialIterator

        Return an iterator on the BreakpointPositions in this 
        Partial.
        """
        return _loris.Partial___iter__(*args)

    def remove(*args): 
        """
        remove(self, pos)

        Remove the specified Breakpoint from this Partial. An
        exception is raised if the specified Breakpoint is not a
        member of this Partial. The Breakpoint itself must be a
        member, not merely identical to a Breakpoint in this
        Partial.
        """
        return _loris.Partial_remove(*args)

    def first(*args): 
        """
        first(self) -> Breakpoint

        Return the first Breakpoint this Partial, or 0 if this
        Partial is empty.
        """
        return _loris.Partial_first(*args)

    def last(*args): 
        """
        last(self) -> Breakpoint

        Return the last Breakpoint this Partial, or 0 if this
        Partial is empty.
        """
        return _loris.Partial_last(*args)

    def insert(*args): 
        """
        insert(self, time, bp)

        Insert a copy of the Breakpoint bp into this Partial at
        the specified time in seconds. Return nothing.
        """
        return _loris.Partial_insert(*args)

    def findAfter(*args): 
        """
        findAfter(self, time) -> PartialIterator

        Return an iterator of BreakpointPositions positioned at
        the first Breakpoint in this Partial that is later than
        the specified time. The iterator might be at its end
        (return no more Breakpoints) if there are no Breakpoints
        in this Partial later than the specified time.
        """
        return _loris.Partial_findAfter(*args)

    def findNearest(*args): 
        """
        findNearest(self, time) -> PartialIterator

        Return an iterator of BreakpointPositions positioned at
        the Breakpoint in this Partial that is nearest to the
        specified time.
        """
        return _loris.Partial_findNearest(*args)


class PartialPtr(Partial):
    def __init__(self, this):
        _swig_setattr(self, Partial, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Partial, 'thisown', 0)
        _swig_setattr(self, Partial,self.__class__,Partial)
_loris.Partial_swigregister(PartialPtr)

class Breakpoint(_object):
    """
    Proxy of C++ Breakpoint class

    A Breakpoint represents a single breakpoint in the
    time-varying frequency, amplitude, and bandwidth
    envelope of a Reassigned Bandwidth-Enhanced Partial.

    Instantaneous phase is also stored, but is only used at
    the onset of a partial, or when it makes a transition
    from zero to nonzero amplitude.

    A Partial represents a Reassigned Bandwidth-Enhanced
    model component. For more information about
    Bandwidth-Enhanced Partials and the Reassigned
    Bandwidth-Enhanced Additive Sound Model, refer to the
    Loris website:
        www.cerlsoundgroup.org/Loris/

    """
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

        A Breakpoint represents a single breakpoint in the
        time-varying frequency, amplitude, and bandwidth
        envelope of a Reassigned Bandwidth-Enhanced Partial.

        Instantaneous phase is also stored, but is only used at
        the onset of a partial, or when it makes a transition
        from zero to nonzero amplitude.

        A Partial represents a Reassigned Bandwidth-Enhanced
        model component. For more information about
        Bandwidth-Enhanced Partials and the Reassigned
        Bandwidth-Enhanced Additive Sound Model, refer to the
        Loris website:
            www.cerlsoundgroup.org/Loris/

        """
        _swig_setattr(self, Breakpoint, 'this', _loris.new_Breakpoint(*args))
        _swig_setattr(self, Breakpoint, 'thisown', 1)
    def __del__(self, destroy=_loris.delete_Breakpoint):
        """
        __del__(self)

        Delete this Breakpoint.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def frequency(*args): 
        """
        frequency(self) -> double

        Return the frequency (in Hz) of this Breakpoint.
        """
        return _loris.Breakpoint_frequency(*args)

    def amplitude(*args): 
        """
        amplitude(self) -> double

        Return the amplitude (absolute) of this Breakpoint.
        """
        return _loris.Breakpoint_amplitude(*args)

    def bandwidth(*args): 
        """
        bandwidth(self) -> double

        Return the bandwidth, or noisiness (0 to 1) of 
        this Breakpoint.
        """
        return _loris.Breakpoint_bandwidth(*args)

    def phase(*args): 
        """
        phase(self) -> double

        Return the phase (in radians) of this Breakpoint.
        """
        return _loris.Breakpoint_phase(*args)

    def setFrequency(*args): 
        """
        setFrequency(self, x)

        Set the frequency (in Hz) of this Breakpoint.
        """
        return _loris.Breakpoint_setFrequency(*args)

    def setAmplitude(*args): 
        """
        setAmplitude(self, x)

        Set the amplitude (absolute) of this Breakpoint.
        """
        return _loris.Breakpoint_setAmplitude(*args)

    def setBandwidth(*args): 
        """
        setBandwidth(self, x)

        Set the bandwidth, or noisiness (0 to 1) of 
        this Breakpoint.
        """
        return _loris.Breakpoint_setBandwidth(*args)

    def setPhase(*args): 
        """
        setPhase(self, x)

        Set the phase (in radians) of this Breakpoint.
        """
        return _loris.Breakpoint_setPhase(*args)


class BreakpointPtr(Breakpoint):
    def __init__(self, this):
        _swig_setattr(self, Breakpoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Breakpoint, 'thisown', 0)
        _swig_setattr(self, Breakpoint,self.__class__,Breakpoint)
_loris.Breakpoint_swigregister(BreakpointPtr)

class BreakpointPosition(_object):
    """
    Proxy of C++ BreakpointPosition class

    A BreakpointPosition represents the position of a 
    Breakpoint within a Partial.
    """
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BreakpointPosition, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BreakpointPosition, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ BreakpointPosition instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def time(*args): 
        """
        time(self) -> double

        Return the time (in seconds) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_time(*args)

    def breakpoint(*args): 
        """
        breakpoint(self) -> Breakpoint

        Return the Breakpoint (not a copy!) at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_breakpoint(*args)

    def frequency(*args): 
        """
        frequency(self) -> double

        Return the frequency (in Hz) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_frequency(*args)

    def amplitude(*args): 
        """
        amplitude(self) -> double

        Return the amplitude (absolute) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_amplitude(*args)

    def bandwidth(*args): 
        """
        bandwidth(self) -> double

        Return the bandwidth, or noisiness (0 to 1) of the
        Breakpoint at this BreakpointPosition.
        """
        return _loris.BreakpointPosition_bandwidth(*args)

    def phase(*args): 
        """
        phase(self) -> double

        Return the phase (in radians) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_phase(*args)

    def setFrequency(*args): 
        """
        setFrequency(self, x)

        Set the frequency (in Hz) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_setFrequency(*args)

    def setAmplitude(*args): 
        """
        setAmplitude(self, x)

        Set the amplitude (absolute) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_setAmplitude(*args)

    def setBandwidth(*args): 
        """
        setBandwidth(self, x)

        Set the bandwidth, or noisiness (0 to 1) of the
        Breakpoint at this BreakpointPosition.
        """
        return _loris.BreakpointPosition_setBandwidth(*args)

    def setPhase(*args): 
        """
        setPhase(self, x)

        Set the phase (in radians) of the Breakpoint at this
        BreakpointPosition.
        """
        return _loris.BreakpointPosition_setPhase(*args)


class BreakpointPositionPtr(BreakpointPosition):
    def __init__(self, this):
        _swig_setattr(self, BreakpointPosition, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BreakpointPosition, 'thisown', 0)
        _swig_setattr(self, BreakpointPosition,self.__class__,BreakpointPosition)
_loris.BreakpointPosition_swigregister(BreakpointPositionPtr)


