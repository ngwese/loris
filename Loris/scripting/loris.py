# This file was created automatically by SWIG.
import lorisc
class PartialList:
    def __init__(self,*args):
        self.this = apply(lorisc.new_PartialList,args)
        self.thisown = 1

    def copy(*args):
        val = apply(lorisc.PartialList_copy,args)
        if val: val = PartialListPtr(val) ; val.thisown = 1
        return val
    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_PartialList(self)
    def clear(*args):
        val = apply(lorisc.PartialList_clear,args)
        return val
    def size(*args):
        val = apply(lorisc.PartialList_size,args)
        return val
    def begin(*args):
        val = apply(lorisc.PartialList_begin,args)
        if val: val = PartialListIteratorPtr(val) ; val.thisown = 1
        return val
    def end(*args):
        val = apply(lorisc.PartialList_end,args)
        if val: val = PartialListIteratorPtr(val) ; val.thisown = 1
        return val
    def insert(*args):
        val = apply(lorisc.PartialList_insert,args)
        if val: val = PartialListIteratorPtr(val) ; val.thisown = 1
        return val
    def erase(*args):
        val = apply(lorisc.PartialList_erase,args)
        return val
    def splice(*args):
        val = apply(lorisc.PartialList_splice,args)
        return val
    def __repr__(self):
        return "<C PartialList instance at %s>" % (self.this,)
class PartialListPtr(PartialList):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = PartialList



class PartialListIterator:
    def __init__(self,this):
        self.this = this

    def copy(*args):
        val = apply(lorisc.PartialListIterator_copy,args)
        if val: val = PartialListIteratorPtr(val) ; val.thisown = 1
        return val
    def next(*args):
        val = apply(lorisc.PartialListIterator_next,args)
        if val: val = PartialListIteratorPtr(val) 
        return val
    def prev(*args):
        val = apply(lorisc.PartialListIterator_prev,args)
        if val: val = PartialListIteratorPtr(val) 
        return val
    def partial(*args):
        val = apply(lorisc.PartialListIterator_partial,args)
        if val: val = PartialPtr(val) 
        return val
    def equals(*args):
        val = apply(lorisc.PartialListIterator_equals,args)
        return val
    def isInRange(*args):
        val = apply(lorisc.PartialListIterator_isInRange,args)
        return val
    def __repr__(self):
        return "<C PartialListIterator instance at %s>" % (self.this,)
class PartialListIteratorPtr(PartialListIterator):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = PartialListIterator



class Partial:
    def __init__(self,*args):
        self.this = apply(lorisc.new_Partial,args)
        self.thisown = 1

    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_Partial(self)
    def label(*args):
        val = apply(lorisc.Partial_label,args)
        return val
    def initialPhase(*args):
        val = apply(lorisc.Partial_initialPhase,args)
        return val
    def startTime(*args):
        val = apply(lorisc.Partial_startTime,args)
        return val
    def endTime(*args):
        val = apply(lorisc.Partial_endTime,args)
        return val
    def duration(*args):
        val = apply(lorisc.Partial_duration,args)
        return val
    def numBreakpoints(*args):
        val = apply(lorisc.Partial_numBreakpoints,args)
        return val
    def setLabel(*args):
        val = apply(lorisc.Partial_setLabel,args)
        return val
    def begin(*args):
        val = apply(lorisc.Partial_begin,args)
        if val: val = PartialIteratorPtr(val) ; val.thisown = 1
        return val
    def end(*args):
        val = apply(lorisc.Partial_end,args)
        if val: val = PartialIteratorPtr(val) ; val.thisown = 1
        return val
    def insert(*args):
        val = apply(lorisc.Partial_insert,args)
        if val: val = PartialIteratorPtr(val) ; val.thisown = 1
        return val
    def findAfter(*args):
        val = apply(lorisc.Partial_findAfter,args)
        if val: val = PartialIteratorPtr(val) ; val.thisown = 1
        return val
    def findNearest(*args):
        val = apply(lorisc.Partial_findNearest,args)
        if val: val = PartialIteratorPtr(val) ; val.thisown = 1
        return val
    def erase(*args):
        val = apply(lorisc.Partial_erase,args)
        return val
    def frequencyAt(*args):
        val = apply(lorisc.Partial_frequencyAt,args)
        return val
    def amplitudeAt(*args):
        val = apply(lorisc.Partial_amplitudeAt,args)
        return val
    def bandwidthAt(*args):
        val = apply(lorisc.Partial_bandwidthAt,args)
        return val
    def phaseAt(*args):
        val = apply(lorisc.Partial_phaseAt,args)
        return val
    def copy(*args):
        val = apply(lorisc.Partial_copy,args)
        if val: val = PartialPtr(val) ; val.thisown = 1
        return val
    def equals(*args):
        val = apply(lorisc.Partial_equals,args)
        return val
    def __repr__(self):
        return "<C Partial instance at %s>" % (self.this,)
class PartialPtr(Partial):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = Partial



class PartialIterator:
    def __init__(self,this):
        self.this = this

    def time(*args):
        val = apply(lorisc.PartialIterator_time,args)
        return val
    def breakpoint(*args):
        val = apply(lorisc.PartialIterator_breakpoint,args)
        if val: val = BreakpointPtr(val) 
        return val
    def copy(*args):
        val = apply(lorisc.PartialIterator_copy,args)
        if val: val = PartialIteratorPtr(val) ; val.thisown = 1
        return val
    def next(*args):
        val = apply(lorisc.PartialIterator_next,args)
        if val: val = PartialIteratorPtr(val) 
        return val
    def prev(*args):
        val = apply(lorisc.PartialIterator_prev,args)
        if val: val = PartialIteratorPtr(val) 
        return val
    def equals(*args):
        val = apply(lorisc.PartialIterator_equals,args)
        return val
    def isInRange(*args):
        val = apply(lorisc.PartialIterator_isInRange,args)
        return val
    def __repr__(self):
        return "<C PartialIterator instance at %s>" % (self.this,)
class PartialIteratorPtr(PartialIterator):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = PartialIterator



class Breakpoint:
    def __init__(self,*args):
        self.this = apply(lorisc.new_Breakpoint,args)
        self.thisown = 1

    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_Breakpoint(self)
    def frequency(*args):
        val = apply(lorisc.Breakpoint_frequency,args)
        return val
    def amplitude(*args):
        val = apply(lorisc.Breakpoint_amplitude,args)
        return val
    def bandwidth(*args):
        val = apply(lorisc.Breakpoint_bandwidth,args)
        return val
    def phase(*args):
        val = apply(lorisc.Breakpoint_phase,args)
        return val
    def setFrequency(*args):
        val = apply(lorisc.Breakpoint_setFrequency,args)
        return val
    def setAmplitude(*args):
        val = apply(lorisc.Breakpoint_setAmplitude,args)
        return val
    def setBandwidth(*args):
        val = apply(lorisc.Breakpoint_setBandwidth,args)
        return val
    def setPhase(*args):
        val = apply(lorisc.Breakpoint_setPhase,args)
        return val
    def copy(*args):
        val = apply(lorisc.Breakpoint_copy,args)
        if val: val = BreakpointPtr(val) ; val.thisown = 1
        return val
    def equals(*args):
        val = apply(lorisc.Breakpoint_equals,args)
        return val
    def __repr__(self):
        return "<C Breakpoint instance at %s>" % (self.this,)
class BreakpointPtr(Breakpoint):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = Breakpoint



class Analyzer:
    def __init__(self,*args):
        self.this = apply(lorisc.new_Analyzer,args)
        self.thisown = 1

    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_Analyzer(self)
    def copy(*args):
        val = apply(lorisc.Analyzer_copy,args)
        if val: val = AnalyzerPtr(val) ; val.thisown = 1
        return val
    def analyze(*args):
        val = apply(lorisc.Analyzer_analyze,args)
        if val: val = PartialListPtr(val) ; val.thisown = 1
        return val
    def configure(*args):
        val = apply(lorisc.Analyzer_configure,args)
        return val
    def freqResolution(*args):
        val = apply(lorisc.Analyzer_freqResolution,args)
        return val
    def ampFloor(*args):
        val = apply(lorisc.Analyzer_ampFloor,args)
        return val
    def windowWidth(*args):
        val = apply(lorisc.Analyzer_windowWidth,args)
        return val
    def freqFloor(*args):
        val = apply(lorisc.Analyzer_freqFloor,args)
        return val
    def hopTime(*args):
        val = apply(lorisc.Analyzer_hopTime,args)
        return val
    def freqDrift(*args):
        val = apply(lorisc.Analyzer_freqDrift,args)
        return val
    def cropTime(*args):
        val = apply(lorisc.Analyzer_cropTime,args)
        return val
    def bwRegionWidth(*args):
        val = apply(lorisc.Analyzer_bwRegionWidth,args)
        return val
    def setFreqResolution(*args):
        val = apply(lorisc.Analyzer_setFreqResolution,args)
        return val
    def setAmpFloor(*args):
        val = apply(lorisc.Analyzer_setAmpFloor,args)
        return val
    def setWindowWidth(*args):
        val = apply(lorisc.Analyzer_setWindowWidth,args)
        return val
    def setFreqFloor(*args):
        val = apply(lorisc.Analyzer_setFreqFloor,args)
        return val
    def setFreqDrift(*args):
        val = apply(lorisc.Analyzer_setFreqDrift,args)
        return val
    def setHopTime(*args):
        val = apply(lorisc.Analyzer_setHopTime,args)
        return val
    def setCropTime(*args):
        val = apply(lorisc.Analyzer_setCropTime,args)
        return val
    def setBwRegionWidth(*args):
        val = apply(lorisc.Analyzer_setBwRegionWidth,args)
        return val
    def __repr__(self):
        return "<C Analyzer instance at %s>" % (self.this,)
class AnalyzerPtr(Analyzer):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = Analyzer



class BreakpointEnvelope:
    def __init__(self,*args):
        self.this = apply(lorisc.new_BreakpointEnvelope,args)
        self.thisown = 1

    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_BreakpointEnvelope(self)
    def copy(*args):
        val = apply(lorisc.BreakpointEnvelope_copy,args)
        if val: val = BreakpointEnvelopePtr(val) ; val.thisown = 1
        return val
    def valueAt(*args):
        val = apply(lorisc.BreakpointEnvelope_valueAt,args)
        return val
    def insertBreakpoint(*args):
        val = apply(lorisc.BreakpointEnvelope_insertBreakpoint,args)
        return val
    def __repr__(self):
        return "<C BreakpointEnvelope instance at %s>" % (self.this,)
class BreakpointEnvelopePtr(BreakpointEnvelope):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = BreakpointEnvelope



class SampleVector:
    def __init__(self,*args):
        self.this = apply(lorisc.new_SampleVector,args)
        self.thisown = 1

    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_SampleVector(self)
    def size(*args):
        val = apply(lorisc.SampleVector_size,args)
        return val
    def resize(*args):
        val = apply(lorisc.SampleVector_resize,args)
        return val
    def clear(*args):
        val = apply(lorisc.SampleVector_clear,args)
        return val
    def copy(*args):
        val = apply(lorisc.SampleVector_copy,args)
        if val: val = SampleVectorPtr(val) ; val.thisown = 1
        return val
    def getAt(*args):
        val = apply(lorisc.SampleVector_getAt,args)
        return val
    def setAt(*args):
        val = apply(lorisc.SampleVector_setAt,args)
        return val
    def __repr__(self):
        return "<C SampleVector instance at %s>" % (self.this,)
class SampleVectorPtr(SampleVector):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = SampleVector



class AiffFile:
    def __init__(self,*args):
        self.this = apply(lorisc.new_AiffFile,args)
        self.thisown = 1

    def __del__(self,lorisc=lorisc):
        if self.thisown == 1 :
            lorisc.delete_AiffFile(self)
    def channels(*args):
        val = apply(lorisc.AiffFile_channels,args)
        return val
    def sampleFrames(*args):
        val = apply(lorisc.AiffFile_sampleFrames,args)
        return val
    def sampleRate(*args):
        val = apply(lorisc.AiffFile_sampleRate,args)
        return val
    def sampleSize(*args):
        val = apply(lorisc.AiffFile_sampleSize,args)
        return val
    def samples(*args):
        val = apply(lorisc.AiffFile_samples,args)
        if val: val = SampleVectorPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C AiffFile instance at %s>" % (self.this,)
class AiffFilePtr(AiffFile):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
        self.__class__ = AiffFile





#-------------- FUNCTION WRAPPERS ------------------

def BreakpointEnvelopeWithValue(*args, **kwargs):
    val = apply(lorisc.BreakpointEnvelopeWithValue,args,kwargs)
    if val: val = BreakpointEnvelopePtr(val); val.thisown = 1
    return val

channelize = lorisc.channelize

dilate = lorisc.dilate

distill = lorisc.distill

exportAiff = lorisc.exportAiff

exportSdif = lorisc.exportSdif

exportSpc = lorisc.exportSpc

def importSdif(*args, **kwargs):
    val = apply(lorisc.importSdif,args,kwargs)
    if val: val = PartialListPtr(val); val.thisown = 1
    return val

def importSpc(*args, **kwargs):
    val = apply(lorisc.importSpc,args,kwargs)
    if val: val = PartialListPtr(val); val.thisown = 1
    return val

def morph(*args, **kwargs):
    val = apply(lorisc.morph,args,kwargs)
    if val: val = PartialListPtr(val); val.thisown = 1
    return val

def synthesize(*args, **kwargs):
    val = apply(lorisc.synthesize,args,kwargs)
    if val: val = SampleVectorPtr(val); val.thisown = 1
    return val

sift = lorisc.sift

def createFreqReference(*args, **kwargs):
    val = apply(lorisc.createFreqReference,args,kwargs)
    if val: val = BreakpointEnvelopePtr(val); val.thisown = 1
    return val

scaleAmp = lorisc.scaleAmp

scaleNoiseRatio = lorisc.scaleNoiseRatio

shiftPitch = lorisc.shiftPitch



#-------------- VARIABLE WRAPPERS ------------------

