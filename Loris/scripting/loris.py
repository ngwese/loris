# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _loris
def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

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


channelize = _loris.channelize

createFreqReference = _loris.createFreqReference

dilate = _loris.dilate

distill = _loris.distill

exportAiff = _loris.exportAiff

exportSdif = _loris.exportSdif

exportSpc = _loris.exportSpc

importSdif = _loris.importSdif

importSpc = _loris.importSpc

morph = _loris.morph

synthesize = _loris.synthesize

crop = _loris.crop

extractLabeled = _loris.extractLabeled

scaleAmp = _loris.scaleAmp

scaleBandwidth = _loris.scaleBandwidth

scaleFrequency = _loris.scaleFrequency

scaleNoiseRatio = _loris.scaleNoiseRatio

shiftPitch = _loris.shiftPitch

shiftTime = _loris.shiftTime

resample = _loris.resample

sift = _loris.sift

version = _loris.version

class AiffFile(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AiffFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AiffFile, name)
    def __del__(self, destroy= _loris.delete_AiffFile):
        try:
            if self.thisown: destroy(self)
        except: pass
    def sampleRate(*args): return apply(_loris.AiffFile_sampleRate,args)
    def midiNoteNumber(*args): return apply(_loris.AiffFile_midiNoteNumber,args)
    def sampleFrames(*args): return apply(_loris.AiffFile_sampleFrames,args)
    def addPartial(*args): return apply(_loris.AiffFile_addPartial,args)
    def setMidiNoteNumber(*args): return apply(_loris.AiffFile_setMidiNoteNumber,args)
    def __init__(self,*args):
        _swig_setattr(self, AiffFile, 'this', apply(_loris.new_AiffFile,args))
        _swig_setattr(self, AiffFile, 'thisown', 1)
    def samples(*args): return apply(_loris.AiffFile_samples,args)
    def channels(*args): return apply(_loris.AiffFile_channels,args)
    def addPartials(*args): return apply(_loris.AiffFile_addPartials,args)
    def __repr__(self):
        return "<C AiffFile instance at %s>" % (self.this,)

class AiffFilePtr(AiffFile):
    def __init__(self,this):
        _swig_setattr(self, AiffFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AiffFile, 'thisown', 0)
        _swig_setattr(self, AiffFile,self.__class__,AiffFile)
_loris.AiffFile_swigregister(AiffFilePtr)

class Analyzer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Analyzer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Analyzer, name)
    def __init__(self,*args):
        _swig_setattr(self, Analyzer, 'this', apply(_loris.new_Analyzer,args))
        _swig_setattr(self, Analyzer, 'thisown', 1)
    def copy(*args): return apply(_loris.Analyzer_copy,args)
    def analyze(*args): return apply(_loris.Analyzer_analyze,args)
    def freqResolution(*args): return apply(_loris.Analyzer_freqResolution,args)
    def ampFloor(*args): return apply(_loris.Analyzer_ampFloor,args)
    def windowWidth(*args): return apply(_loris.Analyzer_windowWidth,args)
    def sidelobeLevel(*args): return apply(_loris.Analyzer_sidelobeLevel,args)
    def freqFloor(*args): return apply(_loris.Analyzer_freqFloor,args)
    def hopTime(*args): return apply(_loris.Analyzer_hopTime,args)
    def freqDrift(*args): return apply(_loris.Analyzer_freqDrift,args)
    def cropTime(*args): return apply(_loris.Analyzer_cropTime,args)
    def bwRegionWidth(*args): return apply(_loris.Analyzer_bwRegionWidth,args)
    def setFreqResolution(*args): return apply(_loris.Analyzer_setFreqResolution,args)
    def setAmpFloor(*args): return apply(_loris.Analyzer_setAmpFloor,args)
    def setWindowWidth(*args): return apply(_loris.Analyzer_setWindowWidth,args)
    def setSidelobeLevel(*args): return apply(_loris.Analyzer_setSidelobeLevel,args)
    def setFreqFloor(*args): return apply(_loris.Analyzer_setFreqFloor,args)
    def setFreqDrift(*args): return apply(_loris.Analyzer_setFreqDrift,args)
    def setHopTime(*args): return apply(_loris.Analyzer_setHopTime,args)
    def setCropTime(*args): return apply(_loris.Analyzer_setCropTime,args)
    def setBwRegionWidth(*args): return apply(_loris.Analyzer_setBwRegionWidth,args)
    def __del__(self, destroy= _loris.delete_Analyzer):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C Analyzer instance at %s>" % (self.this,)

class AnalyzerPtr(Analyzer):
    def __init__(self,this):
        _swig_setattr(self, Analyzer, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Analyzer, 'thisown', 0)
        _swig_setattr(self, Analyzer,self.__class__,Analyzer)
_loris.Analyzer_swigregister(AnalyzerPtr)

class BreakpointEnvelope(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BreakpointEnvelope, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BreakpointEnvelope, name)
    def __init__(self,*args):
        _swig_setattr(self, BreakpointEnvelope, 'this', apply(_loris.new_BreakpointEnvelope,args))
        _swig_setattr(self, BreakpointEnvelope, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_BreakpointEnvelope):
        try:
            if self.thisown: destroy(self)
        except: pass
    def copy(*args): return apply(_loris.BreakpointEnvelope_copy,args)
    def insertBreakpoint(*args): return apply(_loris.BreakpointEnvelope_insertBreakpoint,args)
    def valueAt(*args): return apply(_loris.BreakpointEnvelope_valueAt,args)
    def __repr__(self):
        return "<C BreakpointEnvelope instance at %s>" % (self.this,)

class BreakpointEnvelopePtr(BreakpointEnvelope):
    def __init__(self,this):
        _swig_setattr(self, BreakpointEnvelope, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BreakpointEnvelope, 'thisown', 0)
        _swig_setattr(self, BreakpointEnvelope,self.__class__,BreakpointEnvelope)
_loris.BreakpointEnvelope_swigregister(BreakpointEnvelopePtr)

BreakpointEnvelopeWithValue = _loris.BreakpointEnvelopeWithValue

class SampleVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SampleVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SampleVector, name)
    def __init__(self,*args):
        _swig_setattr(self, SampleVector, 'this', apply(_loris.new_SampleVector,args))
        _swig_setattr(self, SampleVector, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_SampleVector):
        try:
            if self.thisown: destroy(self)
        except: pass
    def clear(*args): return apply(_loris.SampleVector_clear,args)
    def resize(*args): return apply(_loris.SampleVector_resize,args)
    def size(*args): return apply(_loris.SampleVector_size,args)
    def copy(*args): return apply(_loris.SampleVector_copy,args)
    def getAt(*args): return apply(_loris.SampleVector_getAt,args)
    def setAt(*args): return apply(_loris.SampleVector_setAt,args)
    def __repr__(self):
        return "<C SampleVector instance at %s>" % (self.this,)

class SampleVectorPtr(SampleVector):
    def __init__(self,this):
        _swig_setattr(self, SampleVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SampleVector, 'thisown', 0)
        _swig_setattr(self, SampleVector,self.__class__,SampleVector)
_loris.SampleVector_swigregister(SampleVectorPtr)

class NewPlistIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewPlistIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NewPlistIterator, name)
    def atEnd(*args): return apply(_loris.NewPlistIterator_atEnd,args)
    def next(*args): return apply(_loris.NewPlistIterator_next,args)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C NewPlistIterator instance at %s>" % (self.this,)

class NewPlistIteratorPtr(NewPlistIterator):
    def __init__(self,this):
        _swig_setattr(self, NewPlistIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewPlistIterator, 'thisown', 0)
        _swig_setattr(self, NewPlistIterator,self.__class__,NewPlistIterator)
_loris.NewPlistIterator_swigregister(NewPlistIteratorPtr)

class NewPartialIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewPartialIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NewPartialIterator, name)
    def atEnd(*args): return apply(_loris.NewPartialIterator_atEnd,args)
    def next(*args): return apply(_loris.NewPartialIterator_next,args)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C NewPartialIterator instance at %s>" % (self.this,)

class NewPartialIteratorPtr(NewPartialIterator):
    def __init__(self,this):
        _swig_setattr(self, NewPartialIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewPartialIterator, 'thisown', 0)
        _swig_setattr(self, NewPartialIterator,self.__class__,NewPartialIterator)
_loris.NewPartialIterator_swigregister(NewPartialIteratorPtr)

class PartialList(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialList, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialList, name)
    def __init__(self,*args):
        _swig_setattr(self, PartialList, 'this', apply(_loris.new_PartialList,args))
        _swig_setattr(self, PartialList, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_PartialList):
        try:
            if self.thisown: destroy(self)
        except: pass
    def copy(*args): return apply(_loris.PartialList_copy,args)
    def timeSpan(*args): return apply(_loris.PartialList_timeSpan,args)
    def __iter__(*args): return apply(_loris.PartialList___iter__,args)
    def iterator(*args): return apply(_loris.PartialList_iterator,args)
    def clear(*args): return apply(_loris.PartialList_clear,args)
    def size(*args): return apply(_loris.PartialList_size,args)
    def begin(*args): return apply(_loris.PartialList_begin,args)
    def end(*args): return apply(_loris.PartialList_end,args)
    def insert(*args): return apply(_loris.PartialList_insert,args)
    def erase(*args): return apply(_loris.PartialList_erase,args)
    def splice(*args): return apply(_loris.PartialList_splice,args)
    def __repr__(self):
        return "<C PartialList instance at %s>" % (self.this,)

class PartialListPtr(PartialList):
    def __init__(self,this):
        _swig_setattr(self, PartialList, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialList, 'thisown', 0)
        _swig_setattr(self, PartialList,self.__class__,PartialList)
_loris.PartialList_swigregister(PartialListPtr)

class PartialListIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialListIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialListIterator, name)
    def copy(*args): return apply(_loris.PartialListIterator_copy,args)
    def next(*args): return apply(_loris.PartialListIterator_next,args)
    def prev(*args): return apply(_loris.PartialListIterator_prev,args)
    def partial(*args): return apply(_loris.PartialListIterator_partial,args)
    def equals(*args): return apply(_loris.PartialListIterator_equals,args)
    def isInRange(*args): return apply(_loris.PartialListIterator_isInRange,args)
    def __init__(self,*args):
        _swig_setattr(self, PartialListIterator, 'this', apply(_loris.new_PartialListIterator,args))
        _swig_setattr(self, PartialListIterator, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_PartialListIterator):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C PartialListIterator instance at %s>" % (self.this,)

class PartialListIteratorPtr(PartialListIterator):
    def __init__(self,this):
        _swig_setattr(self, PartialListIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialListIterator, 'thisown', 0)
        _swig_setattr(self, PartialListIterator,self.__class__,PartialListIterator)
_loris.PartialListIterator_swigregister(PartialListIteratorPtr)

class Partial(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Partial, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Partial, name)
    def __init__(self,*args):
        _swig_setattr(self, Partial, 'this', apply(_loris.new_Partial,args))
        _swig_setattr(self, Partial, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_Partial):
        try:
            if self.thisown: destroy(self)
        except: pass
    def label(*args): return apply(_loris.Partial_label,args)
    def initialPhase(*args): return apply(_loris.Partial_initialPhase,args)
    def startTime(*args): return apply(_loris.Partial_startTime,args)
    def endTime(*args): return apply(_loris.Partial_endTime,args)
    def duration(*args): return apply(_loris.Partial_duration,args)
    def numBreakpoints(*args): return apply(_loris.Partial_numBreakpoints,args)
    def setLabel(*args): return apply(_loris.Partial_setLabel,args)
    def begin(*args): return apply(_loris.Partial_begin,args)
    def end(*args): return apply(_loris.Partial_end,args)
    def __iter__(*args): return apply(_loris.Partial___iter__,args)
    def iterator(*args): return apply(_loris.Partial_iterator,args)
    def insert(*args): return apply(_loris.Partial_insert,args)
    def findAfter(*args): return apply(_loris.Partial_findAfter,args)
    def findNearest(*args): return apply(_loris.Partial_findNearest,args)
    def erase(*args): return apply(_loris.Partial_erase,args)
    def frequencyAt(*args): return apply(_loris.Partial_frequencyAt,args)
    def amplitudeAt(*args): return apply(_loris.Partial_amplitudeAt,args)
    def bandwidthAt(*args): return apply(_loris.Partial_bandwidthAt,args)
    def phaseAt(*args): return apply(_loris.Partial_phaseAt,args)
    def copy(*args): return apply(_loris.Partial_copy,args)
    def equals(*args): return apply(_loris.Partial_equals,args)
    def __repr__(self):
        return "<C Partial instance at %s>" % (self.this,)

class PartialPtr(Partial):
    def __init__(self,this):
        _swig_setattr(self, Partial, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Partial, 'thisown', 0)
        _swig_setattr(self, Partial,self.__class__,Partial)
_loris.Partial_swigregister(PartialPtr)

class PartialIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PartialIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PartialIterator, name)
    def time(*args): return apply(_loris.PartialIterator_time,args)
    def breakpoint(*args): return apply(_loris.PartialIterator_breakpoint,args)
    def copy(*args): return apply(_loris.PartialIterator_copy,args)
    def next(*args): return apply(_loris.PartialIterator_next,args)
    def prev(*args): return apply(_loris.PartialIterator_prev,args)
    def equals(*args): return apply(_loris.PartialIterator_equals,args)
    def isInRange(*args): return apply(_loris.PartialIterator_isInRange,args)
    def __init__(self,*args):
        _swig_setattr(self, PartialIterator, 'this', apply(_loris.new_PartialIterator,args))
        _swig_setattr(self, PartialIterator, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_PartialIterator):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C PartialIterator instance at %s>" % (self.this,)

class PartialIteratorPtr(PartialIterator):
    def __init__(self,this):
        _swig_setattr(self, PartialIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PartialIterator, 'thisown', 0)
        _swig_setattr(self, PartialIterator,self.__class__,PartialIterator)
_loris.PartialIterator_swigregister(PartialIteratorPtr)

class Breakpoint(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Breakpoint, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Breakpoint, name)
    def __init__(self,*args):
        _swig_setattr(self, Breakpoint, 'this', apply(_loris.new_Breakpoint,args))
        _swig_setattr(self, Breakpoint, 'thisown', 1)
    def __del__(self, destroy= _loris.delete_Breakpoint):
        try:
            if self.thisown: destroy(self)
        except: pass
    def frequency(*args): return apply(_loris.Breakpoint_frequency,args)
    def amplitude(*args): return apply(_loris.Breakpoint_amplitude,args)
    def bandwidth(*args): return apply(_loris.Breakpoint_bandwidth,args)
    def phase(*args): return apply(_loris.Breakpoint_phase,args)
    def setFrequency(*args): return apply(_loris.Breakpoint_setFrequency,args)
    def setAmplitude(*args): return apply(_loris.Breakpoint_setAmplitude,args)
    def setBandwidth(*args): return apply(_loris.Breakpoint_setBandwidth,args)
    def setPhase(*args): return apply(_loris.Breakpoint_setPhase,args)
    def copy(*args): return apply(_loris.Breakpoint_copy,args)
    def equals(*args): return apply(_loris.Breakpoint_equals,args)
    def __repr__(self):
        return "<C Breakpoint instance at %s>" % (self.this,)

class BreakpointPtr(Breakpoint):
    def __init__(self,this):
        _swig_setattr(self, Breakpoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Breakpoint, 'thisown', 0)
        _swig_setattr(self, Breakpoint,self.__class__,Breakpoint)
_loris.Breakpoint_swigregister(BreakpointPtr)

class BreakpointPosition(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BreakpointPosition, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BreakpointPosition, name)
    def time(*args): return apply(_loris.BreakpointPosition_time,args)
    def breakpoint(*args): return apply(_loris.BreakpointPosition_breakpoint,args)
    def frequency(*args): return apply(_loris.BreakpointPosition_frequency,args)
    def amplitude(*args): return apply(_loris.BreakpointPosition_amplitude,args)
    def bandwidth(*args): return apply(_loris.BreakpointPosition_bandwidth,args)
    def phase(*args): return apply(_loris.BreakpointPosition_phase,args)
    def setFrequency(*args): return apply(_loris.BreakpointPosition_setFrequency,args)
    def setAmplitude(*args): return apply(_loris.BreakpointPosition_setAmplitude,args)
    def setBandwidth(*args): return apply(_loris.BreakpointPosition_setBandwidth,args)
    def setPhase(*args): return apply(_loris.BreakpointPosition_setPhase,args)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C BreakpointPosition instance at %s>" % (self.this,)

class BreakpointPositionPtr(BreakpointPosition):
    def __init__(self,this):
        _swig_setattr(self, BreakpointPosition, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BreakpointPosition, 'thisown', 0)
        _swig_setattr(self, BreakpointPosition,self.__class__,BreakpointPosition)
_loris.BreakpointPosition_swigregister(BreakpointPositionPtr)


