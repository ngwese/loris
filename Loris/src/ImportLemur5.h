#ifndef __Loris_import_Lemur5__
#define __Loris_import_Lemur5__
// ===========================================================================
//	ImportLemur5.h
//	
//	Class definition for Loris::ImportLemur5, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================
#include "LorisTypes.h"
#include "ImportPartials.h"

#include <list>
using std::list;

#include <string>
using std::string;

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif


class BinaryFile;

// ---------------------------------------------------------------------------
//	class ImportLemur5
//
//	copy? assignment? dehr?
//
class ImportLemur5: public Import
{
//	-- instance variables --
	BinaryFile & _file;
	double _bweCutoff;	//	Lemur's bandwidth enhancement cutoff frequency
						//	(used to remove undesirable low frequency bandwidth
						//	association in Lemur analyses)
	int _counter;		//	counts tracks to detect completion
	
//	-- public interface --
public:
//	construction:
	ImportLemur5( BinaryFile & lemrFile );
	~ImportLemur5( void );
	
//	access/mutation:
	double bwEnhancementCutoff( void ) const { return _bweCutoff; }
	void setBwEnhancementCutoff( double x ) { _bweCutoff = x; }

//	-- primitve operations --
	//	check that Lemur file is valid:
	virtual void verifySource( void );
	
	//	prepare to import:
	virtual void beginImport( void );
	
	//	derived classes must provide a mean of determining
	//	when the import loop should terminate:
	virtual bool done( void );
	
	//	derived classes must provide a means of reading 
	//	in a Partial and adding it to the list mPartials.
	virtual void getPartial( void );	

	//	clean up after import:
	virtual void endImport( void );
	
//	-- types and ids --
private:	
	enum { 
			FORM_ID = 'FORM', 
			LEMR_ID = 'LEMR', 
			AnalysisParamsID = 'LMAN', 
			TrackDataID = 'TRKS',
			FormatNumber = 4962 };
	
	//	for reading and writing files, the exact sizes and
	//	alignments are critical.
	struct CkHeader {
		Int_32 id;
		Int_32 size;
	};
	
	struct AnalysisParamsCk
	{
		//Int_32 ckID;
		//Int_32 ckSize;
		CkHeader header;
		
		Int_32 formatNumber;
		Int_32 originalFormatNumber;
		
		Int_32 ftLength;			//	samples, transform length
		Float_32 winWidth;			//	Hz, main lobe width
		Float_32 winAtten;			//	dB, sidelobe attenuation
		Int_32	hopSize;			//	samples, frame length
		Float_32 sampleRate;		// 	Hz, from analyzed sample
		
		Float_32 noiseFloor;		//	dB (negative)
		Float_32 peakAmpRange;		//	dB, floating relative amplitde threshold
		Float_32 maskingRolloff;	//	dB/Hz, peak masking curve	
		Float_32 peakSeparation;	//	Hz, minimum separation between peaks
		Float_32 freqDrift;			//	Hz, maximum track freq drift over a frame
	}; 

	struct TrackDataCk
	{
		CkHeader header;
		Uint_32	numberOfTracks;
		Int_32	trackOrder;			// enumerated type
		// track data follows
	};

	struct TrackOnDisk
	{
		Double_64	startTime;		// in milliseconds
		Float_32	initialPhase;
		Uint_32		numPeaks;
		Int_32		label;
	};
	
	struct PeakOnDisk
	{
		Float_32	magnitude;
		Float_32	frequency;
		Float_32	interpolatedFrequency;
		Float_32	bandwidth;
		Double_64	ttn;
	};
	

//	-- import helpers --
private:	
	void readChunkHeader( CkHeader & ck );
	void readParamsChunk( AnalysisParamsCk & ck );
	void readTracksChunk( TrackDataCk & ck );
	
	void readTrackHeader( TrackOnDisk & t );
	void readPeakData( PeakOnDisk & p );
	
//	-- unimplemented --
	ImportLemur5( const ImportLemur5 & other );
	ImportLemur5  & operator = ( const ImportLemur5 & rhs );
};	//	end of class ImportLemur5

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef __Loris_import_Lemur5__

