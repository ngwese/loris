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
#include "ImportPartials.h"
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class BinaryFile;

//	all defined in ImportLemur5.C
struct CkHeader;
struct AnalysisParamsCk; 
struct TrackDataCk;
struct TrackOnDisk;
struct PeakOnDisk;

// ---------------------------------------------------------------------------
//	class ImportLemur5
//
class ImportLemur5: public Import
{
//	-- instance variables --
	std::istream & _file;
	double _bweCutoff;	//	Lemur's bandwidth enhancement cutoff frequency
						//	(used to remove undesirable low frequency bandwidth
						//	association in Lemur analyses)
	int _counter;		//	counts tracks to detect completion
	
//	-- public interface --
public:
//	construction:
	ImportLemur5( std::istream & s );
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

