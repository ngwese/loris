#ifndef INCLUDE_IMPORTLEMUR_H
#define INCLUDE_IMPORTLEMUR_H
// ===========================================================================
//	ImportLemur.h
//	
//	Class definition for Loris::ImportLemur, a concrete subclass of 
//	Loris::Import for importing Partials stored in Lemur 5 alpha files.
//
//
//	-kel 10 Sept 99
//
// ===========================================================================
#include "Partial.h"
#include "Exception.h"
#include <iosfwd>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

//	all defined in ImportLemur.C
struct CkHeader;
struct TrackOnDisk;
struct PeakOnDisk;

// ---------------------------------------------------------------------------
//	class ImportLemur
//
class ImportLemur
{
//	-- instance variables --
	PartialList _partials;	//	collect Partials here

	double _bweCutoff;		//	Lemur's bandwidth enhancement cutoff frequency
							//	(used to remove undesirable low frequency bandwidth
							//	association in Lemur analyses)
	
//	-- public interface --
public:
//	construction:
//	(compiler can generate destructor)
	ImportLemur( std::istream & s, double bweCutoff = 1000 );

//	PartialList access:
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	
//	-- import helpers --
private:	
	void importPartials( std::istream & s );
	void getPartial( std::istream & s );	

	void readChunkHeader( std::istream & s, CkHeader & ck );
	void readContainer( std::istream & s );
	void readParamsChunk( std::istream & s );
	long readTracksChunk( std::istream & s );
	
	void readTrackHeader( std::istream & s, TrackOnDisk & t );
	void readPeakData( std::istream & s, PeakOnDisk & p );
	
//	-- unimplemented --
	ImportLemur( const ImportLemur & other );
	ImportLemur  & operator = ( const ImportLemur & rhs );
	
};	//	end of class ImportLemur

// ---------------------------------------------------------------------------
//	class ImportException
//
//	Class of exceptions thrown when there is an error importing
//	Partials.
//
class ImportException : public Exception
{
public: 
	ImportException( const std::string & str, const std::string & where = "" ) : 
		Exception( std::string("Import Error -- ").append( str ), where ) {}		
};

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_IMPORTLEMUR_H

