# This file was created automatically by SWIG.
import lorisc
class PartialList:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,PartialList):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = PartialList.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = PartialList.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def copy(*args): return apply(lorisc.PartialList_copy,args)
    def __init__(self,*args):
        self.this = apply(lorisc.new_PartialList,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_PartialList(self)
    def clear(*args): return apply(lorisc.PartialList_clear,args)
    def size(*args): return apply(lorisc.PartialList_size,args)
    def begin(*args): return apply(lorisc.PartialList_begin,args)
    def end(*args): return apply(lorisc.PartialList_end,args)
    def insert(*args): return apply(lorisc.PartialList_insert,args)
    def erase(*args): return apply(lorisc.PartialList_erase,args)
    def splice(*args): return apply(lorisc.PartialList_splice,args)
    def __repr__(self):
        return "<C PartialList instance at %s>" % (self.this,)

class PartialListPtr(PartialList):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PartialList
lorisc.PartialList_swigregister(PartialListPtr)
class PartialListIterator:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,PartialListIterator):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = PartialListIterator.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = PartialListIterator.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def copy(*args): return apply(lorisc.PartialListIterator_copy,args)
    def next(*args): return apply(lorisc.PartialListIterator_next,args)
    def prev(*args): return apply(lorisc.PartialListIterator_prev,args)
    def partial(*args): return apply(lorisc.PartialListIterator_partial,args)
    def equals(*args): return apply(lorisc.PartialListIterator_equals,args)
    def isInRange(*args): return apply(lorisc.PartialListIterator_isInRange,args)
    def __init__(self,*args):
        self.this = apply(lorisc.new_PartialListIterator,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_PartialListIterator(self)
    def __repr__(self):
        return "<C PartialListIterator instance at %s>" % (self.this,)

class PartialListIteratorPtr(PartialListIterator):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PartialListIterator
lorisc.PartialListIterator_swigregister(PartialListIteratorPtr)
class Partial:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,Partial):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = Partial.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = Partial.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def __init__(self,*args):
        self.this = apply(lorisc.new_Partial,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_Partial(self)
    def label(*args): return apply(lorisc.Partial_label,args)
    def initialPhase(*args): return apply(lorisc.Partial_initialPhase,args)
    def startTime(*args): return apply(lorisc.Partial_startTime,args)
    def endTime(*args): return apply(lorisc.Partial_endTime,args)
    def duration(*args): return apply(lorisc.Partial_duration,args)
    def numBreakpoints(*args): return apply(lorisc.Partial_numBreakpoints,args)
    def setLabel(*args): return apply(lorisc.Partial_setLabel,args)
    def begin(*args): return apply(lorisc.Partial_begin,args)
    def end(*args): return apply(lorisc.Partial_end,args)
    def insert(*args): return apply(lorisc.Partial_insert,args)
    def findAfter(*args): return apply(lorisc.Partial_findAfter,args)
    def findNearest(*args): return apply(lorisc.Partial_findNearest,args)
    def erase(*args): return apply(lorisc.Partial_erase,args)
    def frequencyAt(*args): return apply(lorisc.Partial_frequencyAt,args)
    def amplitudeAt(*args): return apply(lorisc.Partial_amplitudeAt,args)
    def bandwidthAt(*args): return apply(lorisc.Partial_bandwidthAt,args)
    def phaseAt(*args): return apply(lorisc.Partial_phaseAt,args)
    def copy(*args): return apply(lorisc.Partial_copy,args)
    def equals(*args): return apply(lorisc.Partial_equals,args)
    def __repr__(self):
        return "<C Partial instance at %s>" % (self.this,)

class PartialPtr(Partial):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Partial
lorisc.Partial_swigregister(PartialPtr)
class PartialIterator:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,PartialIterator):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = PartialIterator.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = PartialIterator.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def time(*args): return apply(lorisc.PartialIterator_time,args)
    def breakpoint(*args): return apply(lorisc.PartialIterator_breakpoint,args)
    def copy(*args): return apply(lorisc.PartialIterator_copy,args)
    def next(*args): return apply(lorisc.PartialIterator_next,args)
    def prev(*args): return apply(lorisc.PartialIterator_prev,args)
    def equals(*args): return apply(lorisc.PartialIterator_equals,args)
    def isInRange(*args): return apply(lorisc.PartialIterator_isInRange,args)
    def __init__(self,*args):
        self.this = apply(lorisc.new_PartialIterator,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_PartialIterator(self)
    def __repr__(self):
        return "<C PartialIterator instance at %s>" % (self.this,)

class PartialIteratorPtr(PartialIterator):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PartialIterator
lorisc.PartialIterator_swigregister(PartialIteratorPtr)
class Breakpoint:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,Breakpoint):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = Breakpoint.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = Breakpoint.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def __init__(self,*args):
        self.this = apply(lorisc.new_Breakpoint,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_Breakpoint(self)
    def frequency(*args): return apply(lorisc.Breakpoint_frequency,args)
    def amplitude(*args): return apply(lorisc.Breakpoint_amplitude,args)
    def bandwidth(*args): return apply(lorisc.Breakpoint_bandwidth,args)
    def phase(*args): return apply(lorisc.Breakpoint_phase,args)
    def setFrequency(*args): return apply(lorisc.Breakpoint_setFrequency,args)
    def setAmplitude(*args): return apply(lorisc.Breakpoint_setAmplitude,args)
    def setBandwidth(*args): return apply(lorisc.Breakpoint_setBandwidth,args)
    def setPhase(*args): return apply(lorisc.Breakpoint_setPhase,args)
    def copy(*args): return apply(lorisc.Breakpoint_copy,args)
    def equals(*args): return apply(lorisc.Breakpoint_equals,args)
    def __repr__(self):
        return "<C Breakpoint instance at %s>" % (self.this,)

class BreakpointPtr(Breakpoint):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Breakpoint
lorisc.Breakpoint_swigregister(BreakpointPtr)
class Analyzer:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,Analyzer):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = Analyzer.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = Analyzer.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def __init__(self,*args):
        self.this = apply(lorisc.new_Analyzer,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_Analyzer(self)
    def copy(*args): return apply(lorisc.Analyzer_copy,args)
    def analyze(*args): return apply(lorisc.Analyzer_analyze,args)
    def configure(*args): return apply(lorisc.Analyzer_configure,args)
    def freqResolution(*args): return apply(lorisc.Analyzer_freqResolution,args)
    def ampFloor(*args): return apply(lorisc.Analyzer_ampFloor,args)
    def windowWidth(*args): return apply(lorisc.Analyzer_windowWidth,args)
    def freqFloor(*args): return apply(lorisc.Analyzer_freqFloor,args)
    def hopTime(*args): return apply(lorisc.Analyzer_hopTime,args)
    def freqDrift(*args): return apply(lorisc.Analyzer_freqDrift,args)
    def cropTime(*args): return apply(lorisc.Analyzer_cropTime,args)
    def bwRegionWidth(*args): return apply(lorisc.Analyzer_bwRegionWidth,args)
    def setFreqResolution(*args): return apply(lorisc.Analyzer_setFreqResolution,args)
    def setAmpFloor(*args): return apply(lorisc.Analyzer_setAmpFloor,args)
    def setWindowWidth(*args): return apply(lorisc.Analyzer_setWindowWidth,args)
    def setFreqFloor(*args): return apply(lorisc.Analyzer_setFreqFloor,args)
    def setFreqDrift(*args): return apply(lorisc.Analyzer_setFreqDrift,args)
    def setHopTime(*args): return apply(lorisc.Analyzer_setHopTime,args)
    def setCropTime(*args): return apply(lorisc.Analyzer_setCropTime,args)
    def setBwRegionWidth(*args): return apply(lorisc.Analyzer_setBwRegionWidth,args)
    def __repr__(self):
        return "<C Analyzer instance at %s>" % (self.this,)

class AnalyzerPtr(Analyzer):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Analyzer
lorisc.Analyzer_swigregister(AnalyzerPtr)
class BreakpointEnvelope:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,BreakpointEnvelope):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = BreakpointEnvelope.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = BreakpointEnvelope.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def __init__(self,*args):
        self.this = apply(lorisc.new_BreakpointEnvelope,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_BreakpointEnvelope(self)
    def copy(*args): return apply(lorisc.BreakpointEnvelope_copy,args)
    def valueAt(*args): return apply(lorisc.BreakpointEnvelope_valueAt,args)
    def insertBreakpoint(*args): return apply(lorisc.BreakpointEnvelope_insertBreakpoint,args)
    def __repr__(self):
        return "<C BreakpointEnvelope instance at %s>" % (self.this,)

class BreakpointEnvelopePtr(BreakpointEnvelope):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BreakpointEnvelope
lorisc.BreakpointEnvelope_swigregister(BreakpointEnvelopePtr)
class SampleVector:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,SampleVector):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = SampleVector.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = SampleVector.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def __init__(self,*args):
        self.this = apply(lorisc.new_SampleVector,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_SampleVector(self)
    def size(*args): return apply(lorisc.SampleVector_size,args)
    def resize(*args): return apply(lorisc.SampleVector_resize,args)
    def clear(*args): return apply(lorisc.SampleVector_clear,args)
    def copy(*args): return apply(lorisc.SampleVector_copy,args)
    def getAt(*args): return apply(lorisc.SampleVector_getAt,args)
    def setAt(*args): return apply(lorisc.SampleVector_setAt,args)
    def __repr__(self):
        return "<C SampleVector instance at %s>" % (self.this,)

class SampleVectorPtr(SampleVector):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SampleVector
lorisc.SampleVector_swigregister(SampleVectorPtr)
class AiffFile:
    __setmethods__ = {}
    for _s in []: __setmethods__.update(_s.__setmethods__)
    def __setattr__(self,name,value):
        if (name == "this"):
            if isinstance(value,AiffFile):
                self.__dict__[name] = value.this
                if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
                del value.thisown
                return
        method = AiffFile.__setmethods__.get(name,None)
        if method: return method(self,value)
        self.__dict__[name] = value

    __getmethods__ = {}
    for _s in []: __getmethods__.update(_s.__getmethods__)
    def __getattr__(self,name):
        method = AiffFile.__getmethods__.get(name,None)
        if method: return method(self)
        raise AttributeError,name

    def __init__(self,*args):
        self.this = apply(lorisc.new_AiffFile,args)
        self.thisown = 1
    def __del__(self,lorisc=lorisc):
        if getattr(self,'thisown',0):
            lorisc.delete_AiffFile(self)
    def channels(*args): return apply(lorisc.AiffFile_channels,args)
    def sampleFrames(*args): return apply(lorisc.AiffFile_sampleFrames,args)
    def sampleRate(*args): return apply(lorisc.AiffFile_sampleRate,args)
    def sampleSize(*args): return apply(lorisc.AiffFile_sampleSize,args)
    def samples(*args): return apply(lorisc.AiffFile_samples,args)
    def __repr__(self):
        return "<C AiffFile instance at %s>" % (self.this,)

class AiffFilePtr(AiffFile):
    def __init__(self,this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = AiffFile
lorisc.AiffFile_swigregister(AiffFilePtr)
BreakpointEnvelopeWithValue = lorisc.BreakpointEnvelopeWithValue

channelize = lorisc.channelize

dilate = lorisc.dilate

distill = lorisc.distill

exportAiff = lorisc.exportAiff

exportSdif = lorisc.exportSdif

exportSpc = lorisc.exportSpc

importSdif = lorisc.importSdif

importSpc = lorisc.importSpc

morph = lorisc.morph

synthesize = lorisc.synthesize

sift = lorisc.sift

createFreqReference = lorisc.createFreqReference

scaleAmp = lorisc.scaleAmp

scaleNoiseRatio = lorisc.scaleNoiseRatio

shiftPitch = lorisc.shiftPitch


