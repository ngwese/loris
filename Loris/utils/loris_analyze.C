#include <algorithm>
#include <cstdio> // for scanf
#include <cstdlib>
#include <iostream>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "AiffFile.h"
#include "Analyzer.h"
#include "Channelizer.h"
#include "Distiller.h"
#include "FrequencyReference.h"
#include "Resampler.h"
#include "SdifFile.h"

using std::cout;
using std::endl;
using std::string;

class Command; 
typedef std::stack< std::string > Arguments;
typedef std::map< std::string, Command * > CmdDictionary;

// ----------------------------------------------------------------
//	Abstract Command class
// ----------------------------------------------------------------

class Command
{
public:
	virtual ~Command( void ) {};
	virtual void execute( Arguments & args ) const = 0;
};

// ----------------------------------------------------------------
//	global program state
// ----------------------------------------------------------------
string gInFileName, gOutFileName = "partials.sdif", gTestFileName;
Loris::Analyzer * gAnalyzer = 0;
bool gCollate = false;
double gDistill = 0;
double gResample = 0;
bool gVerbose = false;
double gRate = 44100;

// ----------------------------------------------------------------
//	argument parsing
// ----------------------------------------------------------------
static bool argIsNumber( const string & s, double * ret= 0 )
{
	const char * cstr = s.c_str();
	char * endptr = 0;
	double x = std::strtod( cstr, &endptr );
	if ( endptr == cstr )
	{
		return false;
	}
	else
	{
		if ( ret != 0 )
		{
			*ret = x;
		}
		return true;
	}
}

static bool argIsFlag( const string & s )
{
	return s[0] == '-' && ! argIsNumber(s);
}

static string lowercaseArg( string s )
{
	for ( string::iterator it = s.begin(); it != s.end(); ++it )
	{
		*it = std::tolower(*it);
	}
	return s;
}

// ----------------------------------------------------------------
//	Concrete Commands
// ----------------------------------------------------------------

class InfileCommand : public Command
{
public:
	//	set the global input filename
	void execute( Arguments & args ) const 
	{
		//	requires a string specifying the filename
		if ( args.empty() || argIsFlag( args.top() ) )
		{
			throw std::invalid_argument("input file specification "
									   "requires a filename");
		}
		
		gInFileName = args.top();
		cout << "* using input (AIFF) file: " << gInFileName << endl;

		args.pop();
	}
};

class OutfileCommand : public Command
{
public:
	//	set the global output filename
	void execute( Arguments & args ) const 
	{
		//	requires a string specifying the filename
		if ( args.empty() || argIsFlag( args.top() ) )
		{
			throw std::invalid_argument("output file specification "
									   "requires a filename");
		}
		
		gOutFileName = args.top();
		cout << "* using output (SDIF) file: " << gOutFileName << endl;

		args.pop();
	}
};

class TestfileCommand : public Command
{
public:
	//	set the global filename for a test render
	void execute( Arguments & args ) const 
	{
		//	accepts a string specifying the filename, 
		//	default is test.aiff
		if ( args.empty() || argIsFlag( args.top() ) )
		{
			gTestFileName = "test.aiff";
		}
		else
		{
			gTestFileName = args.top();
			args.pop();
		}
		
		cout << "* rendering to (AIFF) file: " << gTestFileName << endl;
	}
};

class CollateCommand : public Command
{
public:
	//	set the global flag indicating that the
	//	Partials should be collated before export
	void execute( Arguments &  ) const 
	{
		gCollate = true;
		cout << "* will collate partials" << endl;

		//	collation overrides distillation
		if ( gCollate )
		{
			cout << "* collate specification overrides "
			        "distillation specification" << endl;			       
			gDistill = 0;
		}
	}
};

class DistillCommand : public Command
{
public:
	//	set the global flag indicating that the
	//	Partials should be distilled before export
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("distillation specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("distillation specification "
									   "must be positive");
		}

		gDistill = x;
		cout << "* will distill partials assuming a fundamental of approximately " 
			 << gDistill << " Hz" << endl;
		
		args.pop();

		//	distillation overrides collation
		if ( gCollate )
		{
			cout << "* distillation specification overrides "
			        "collation specification" << endl;			       
			gCollate = false;
		}
	}
};

class ResampleCommand : public Command
{
public:
	//	set the global flag indicating that the
	//	Partials should be resampled before export
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("resample specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("resample specification "
									   "must be positive");
		}

		gResample = x;
		cout << "* will resample partials every " 
			 << gResample << " s" << endl;

		args.pop();
	}
};

class SetHopTimeCommand : public Command
{
public:
	//	set the hopTime parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("hop time specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("hop time specification "
									   "must be positive");
		}
		
		gAnalyzer->setHopTime( x );
		cout << "* setting analysis hop time to: ";
		cout << gAnalyzer->hopTime() << " s" << endl;

		args.pop();
	}
};
	
class SetCropTimeCommand : public Command
{
public:
	//	set the cropTime parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("crop time specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("crop time specification "
									  "must be positive");
		}

		gAnalyzer->setCropTime( x );
		cout << "* setting analysis crop time to: ";
		cout << gAnalyzer->cropTime() << " s" << endl;

		args.pop();
	}
};
	
class SetRegionWidthCommand : public Command
{
public:
	//	set the BWRegionWidth parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("BW region width specification "
									   "requires a number");
		}
		
		if ( x < 0 )
		{
			throw std::invalid_argument("BW region width specification "
									  "must be non-negative");
		}
		
		gAnalyzer->setBwRegionWidth( x );
		cout << "* setting analysis BW region width to: ";
		cout << gAnalyzer->bwRegionWidth() << " Hz" << endl;

		args.pop();
	}
};
	
class SetDriftCommand : public Command
{
public:
	//	set the frequencyDrift parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("frequency drift specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("frequency drift specification "
									  "must be positive");
		}
		
		gAnalyzer->setFreqDrift( x );
		cout << "* setting analysis frequency drift to: ";
		cout << gAnalyzer->freqDrift() << " Hz" << endl;

		args.pop();
	}
};
	
class SetAmpFloorCommand : public Command
{
public:
	//	set the ampFloor parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("amplitude floor specification "
									   "requires a number");
		}
		
		if ( x >= 0 )
		{
			throw std::invalid_argument("amplitude floor specification "
									  "must be negative");
		}
		
		gAnalyzer->setAmpFloor( x );
		cout << "* setting analysis amplitude floor to: ";
		cout << gAnalyzer->ampFloor() << " dB" << endl;

		args.pop();
	}
};
	
class SetFreqFloorCommand : public Command
{
public:
	//	set the freqFloor parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("frequency floor specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("frequency floor specification "
									  "must be positive");
		}
		
		gAnalyzer->setFreqFloor( x );
		cout << "* setting analysis frequency floor to: ";
		cout << gAnalyzer->freqFloor() << " Hz" << endl;

		args.pop();
	}
};
	
class SetAttenuationCommand : public Command
{
public:
	//	set the sidelobe attenuation parameter of the global
	//	Loris Analyzer
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("sidelobe attenuation specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("sidelobe attenuation specification "
									  "must be positive");
		}
		
		gAnalyzer->setSidelobeLevel( x );
		cout << "* setting analysis sidelobe attenuation to: "; 
		cout << gAnalyzer->sidelobeLevel() << " dB" << endl;

		args.pop();
	}
};

class SetResolutionCommand : public Command
{
public:
	//	set the frequency resolution parameter of the global
	//	Loris Analyzer (after configuring)
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("frequency resolution specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("frequency resolution specification "
									  "must be positive");
		}
		
		gAnalyzer->setFreqResolution( x );
		cout << "* setting analysis frequency resolution to: "; 
		cout << gAnalyzer->freqResolution() << " Hz" << endl;

		args.pop();
	}
};

class SetWindowCommand : public Command
{
public:
	//	set the window width parameter of the global
	//	Loris Analyzer (after configuring)
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("window width specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("window width specification "
									  "must be positive");
		}
		
		gAnalyzer->setWindowWidth( x );
		cout << "* setting analysis window width to: "; 
		cout << gAnalyzer->windowWidth() << " Hz" << endl;

		args.pop();
	}
};

class SetSampleRateCommand : public Command
{
public:
	//	set the sample rate for samples read from standard
	//	input, and for rendering partials.
	void execute( Arguments & args ) const 
	{
		//	requires a numeric parameter
		double x;
		if ( args.empty() || !argIsNumber( args.top(), &x ) )
		{
			throw std::invalid_argument("sample rate specification "
									   "requires a number");
		}
		
		if ( x <= 0 )
		{
			throw std::invalid_argument("sample rate specification "
									  "must be positive");
		}
		
		gRate = x;
		cout << "* setting sample rate to: "; 
		cout << gRate << " Hz" << endl;

		args.pop();
	}
};
		
class VerboseCommand : public Command
{
public:
	//	set the verbosity flag to spew out more information.
	void execute( Arguments & args ) const 
	{
		gVerbose = true;
		cout << "* being even more verbose than usual" << endl;
	}
};

// ----------------------------------------------------------------
//	parseArguments
// ----------------------------------------------------------------
//	Process each command line argument in sequence, and execute
//	the command associated with each one. Throw 
//	invalid_argument if an unrecognized argument is encountered,
//	and allow any exceptions generated by executing the command
//	to propogate.
//
static void parseArguments( Arguments & args, const CmdDictionary & commands )
{
	if ( args.size() < 2 )
	{
		throw std::invalid_argument("insufficient arguments");
	}
	
	//	the first argument is the program name
	cout << "running " << args.top() << endl;
	args.pop();
	
	//	the second argument specifies the resolution at
	//	which to configure the Analyzer
	double resolution;
	if ( ! argIsNumber( args.top(), &resolution ) )
	{
		throw std::invalid_argument("first argument must be analyzer resolution");
	}
	args.pop();
	
	//	if the next argument is not an flag and it is a
	//	number, then it is the window width at which to 
	//	configure the Analyzer. If not specified, the
	//	window width is twice the resolution.
	//
	//	if the next argument is not a flag and is not a 
	//	number, then it is the input filename. If not specified
	//	here, then either the '-i' flag will specify it, or
	//	samples will be read from standard input:
	double winwidth = 2*resolution;
	if ( !args.empty() && argIsNumber( args.top(), &winwidth ) )
	{
		args.pop();
	}
	cout << "* configuring Loris Analyzer with resolution " << resolution;
	cout << " and window width " << winwidth << endl;
	gAnalyzer = new Loris::Analyzer( resolution, winwidth );
	
	//	if the next argument is not a flag and is not a 
	//	number, then it is the input filename. If not specified
	//	here, then either the '-i' flag will specify it, or
	//	samples will be read from standard input:
	if ( !args.empty() && !argIsFlag( args.top() ) && !argIsNumber( args.top() ) )
	{
		InfileCommand cmd;
		cmd.execute( args );
	}
	
	//	invariant: there are more command line arguments
	//	to be processed
	while ( !args.empty() )
	{
		string cmd = lowercaseArg( args.top() );
		CmdDictionary::const_iterator it = commands.find( cmd );
		if ( it == commands.end() )
		{
			throw std::invalid_argument( "unrecognized argument " + args.top() );
		}
		
		args.pop();
		it->second->execute( args );	
	}
}

// ----------------------------------------------------------------
//	fill_buffer
// ----------------------------------------------------------------
//	Read samples from standard input into a buffer, return the number
//	of samples successfully read into the buffer (should be equal to
//	howmany unless the read fails). 
//
static int fill_buffer( double * buffer, int howmany )
{
	using std::scanf;
	
	//	invariant:
	//	j samples have been read into the buffer from
	//	satndard input.
	int j = 0;
	while ( j < howmany && scanf("%lf", buffer+j ) == 1 )
	{
		++j;
	}

	//	invariant:
	//	k samples in the buffer have been overwritten
	//	with either samples from standard input, or
	//	with zeros.
	for ( int k = j; k < howmany; ++k )
    {
		buffer[k] = 0.;
	}

	return  j;
}

// ----------------------------------------------------------------
//	main
// ----------------------------------------------------------------

int main( int argc, char * argv[] )
{
	//	build a dictionary of commands
	CmdDictionary commands;
	commands["-i"] = commands["-in"] = commands["-ifile"] = commands["-infile"] = 
		new InfileCommand();
	commands["-o"] = commands["-out"] = commands["-ofile"] = commands["-outfile"] =
		new OutfileCommand();
	commands["-render"] = commands["-synth"] = new TestfileCommand();
	commands["-collate"] = new CollateCommand();
	commands["-distill"] = commands["-dist"] = new DistillCommand();
	commands["-resample"] = commands["-resamp"] = new ResampleCommand();
	commands["-hop"] = commands["-hoptime"] = new SetHopTimeCommand();
	commands["-crop"] = commands["-croptime"] = new SetCropTimeCommand();
	commands["-bw"] = commands["-bwregionwidth"] = new SetRegionWidthCommand();
	commands["-drift"] = commands["-freqdrift"] = new SetDriftCommand();
	commands["-ampfloor"] = new SetAmpFloorCommand();
	commands["-freqfloor"] = new SetFreqFloorCommand();
	commands["-sidelobes"] = commands["-attenutation"] = commands["-sidelobelevel"] =
		new SetAttenuationCommand();
	commands["-rate"] = commands["-samplerate"] = commands["-sr"] = 
		new SetSampleRateCommand();
	commands["-resolution"] = commands["-res"] = commands["-freqres"] = 
		commands["-freqresolution"] = new SetResolutionCommand();
	commands["-width"] = commands["-winwidth"] = commands["-windowwidth"] = 
		new SetWindowCommand();
	commands["-v"] = commands["-verbose"] = new VerboseCommand();
	
	// 	build an argument stack, pushing the arguments
	//	in reverse order.
	//	invariant: argc command line arguments remain to be pushed
	Arguments args;
	while (argc-- > 0)
	{
		args.push( argv[argc] );
	}
	
	try
	{
		parseArguments( args, commands );
	}
	catch ( std::logic_error & ex )
	{
		cout << "Error parsing arguments: \n\t" << ex.what() << endl;
		cout << "usage: " << argv[0] << " resolution ";
		cout << "[windowWidth] [infilename.aiff] [flags]" << endl;
		return 1;
	}
	
	//	if verbose, spew out the Analyzer state:
	if ( gVerbose )
	{
		cout << "* Loris Analyzer configuration:" << endl;
		cout << "*\tfrequency resolution: " << gAnalyzer->freqResolution() << " Hz\n";
		cout << "*\tanalysis window width: " << gAnalyzer->windowWidth() << " Hz\n";
		cout << "*\tanalysis window sidelobe attenuation: "
			 << gAnalyzer->sidelobeLevel() << " dB\n";
		cout << "*\tspectral amplitude floor: " << gAnalyzer->ampFloor() << " dB\n";
		cout << "*\tminimum partial frequecy: " << gAnalyzer->freqFloor() << " Hz\n";
		cout << "*\thop time: " << 1000*gAnalyzer->hopTime() << " ms\n";
		cout << "*\tmaximum partial frequency drift: " << gAnalyzer->freqDrift() 
			 << " Hz\n";
		cout << "*\tcrop time: " << 1000*gAnalyzer->cropTime() << " ms\n";
		cout << "*\tbandwidth association region width: " 
			 << gAnalyzer->bwRegionWidth() << " Hz\n";
		cout << endl;
	}
	
	//	run the analysis
	try
	{
		Loris::AiffFile::samples_type samples;
		Loris::AiffFile::markers_type markers;
		double analysisRate = gRate;
		if ( !gInFileName.empty() )
		{
			cout << "* reading samples from " << gInFileName << endl;
			Loris::AiffFile infile( gInFileName );
			samples = infile.samples();
			analysisRate = infile.sampleRate();
			markers = infile.markers();
		}
		else
		{
			cout << "reading samples from standard input at " 
				 << gRate << " Hz sample rate" << endl;
			analysisRate = gRate;
			const int bufsize = 1024;
			double buffer[ bufsize ];
			int sampsread = 0;
			do
			{
				sampsread = fill_buffer( buffer, bufsize );
				samples.insert( samples.end(), buffer, buffer + sampsread );
			} while ( sampsread == bufsize );
			cout << "read " << samples.size() << " samples" << endl;
		}
		
		cout << "* performing analysis" << endl;
		gAnalyzer->analyze( samples, analysisRate );
		cout << "* analysis complete" << endl;	
		
		if ( gDistill > 0 )
		{
			cout << "* extracting frequency reference envelope" << endl;
			Loris::FrequencyReference ref( gAnalyzer->partials().begin(), 
										 gAnalyzer->partials().end(),
										 0.8 * gDistill, 1.2 * gDistill );
			Loris::Channelizer chan( ref, 1 );
			cout << "* channelizing " << gAnalyzer->partials().size() 
				 << " partials" << endl;
			chan.channelize( gAnalyzer->partials().begin(), 
							gAnalyzer->partials().end() );
			Loris::Distiller dist;
			cout << "* distilling " << gAnalyzer->partials().size() 
				 << " partials" << endl;
			dist.distill( gAnalyzer->partials() );
		}
		else if ( gCollate )
		{
			cout << "* collating " << gAnalyzer->partials().size();
			cout << " partials" << endl;
			Loris::Distiller dist;
			dist.distill( gAnalyzer->partials() );
		}
		
		if ( gResample > 0 )
		{
			Loris::Resampler resamp( gResample );
			cout << "* resampling " << gAnalyzer->partials().size() 
				 << " partials at " << 1000*gResample << " ms intervals" << endl;
			resamp.resample( gAnalyzer->partials().begin(), 
							gAnalyzer->partials().end() );
		}
			
		cout << "* exporting " << gAnalyzer->partials().size(); 
		cout << " partials to " << gOutFileName << endl;
		Loris::SdifFile outfile( gAnalyzer->partials().begin(), 
								gAnalyzer->partials().end() );
		outfile.markers() = markers;
		outfile.write( gOutFileName );
		
		if ( ! gTestFileName.empty() )
		{
			cout << "* exporting rendered partials to " << gTestFileName << endl;
			Loris::AiffFile testfile( gAnalyzer->partials().begin(), 
									gAnalyzer->partials().end(), gRate );
			testfile.markers() = markers;
			testfile.write( gTestFileName );
		}
		
		cout << "* Done." << endl;
	}
	catch ( std::exception & ex )
	{
		cout << "Error running analysis: " << ex.what() << endl;
		return 1;
	}
	
	return 0;
} 