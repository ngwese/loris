#ifndef INCLUDE_NOTIFIER_H
#define INCLUDE_NOTIFIER_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Notifier.h
 *
 *	Declarations of ostreams used for notification throughout
 *	the Loris class library. These streams are used like cout
 *	or cerr, but they buffer their contents in a std::string
 *	until a newline is receieved. Then they post their entire
 *	contents to a notification handler (type NotificationHandler,
 *	defined below). The default handler just prints to stderr. 
 *	Other handlers (of type NotificationHandler) may be assigned
 *	using setNotifierHandler() and setDebuggerHandler(), proto-
 *	typed below. 
 *
 *	The ostream debugger is disabled when the symbol Debug_Loris
 *	is undefined. It cannot be enabled using setDebuggerHandler().
 *
 *	This header may be included in c files, the ostream declarations
 *	and support will be omitted.
 *
 * Kelly Fitz, 28 Feb 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */


/*
 *	stream declaration, C++ only:
 */
#ifdef __cplusplus

#include <iostream>

//	begin namespace
namespace Loris {

std::ostream & getNotifierStream(void);
std::ostream & getDebuggerStream(void);

//	declare streams:
static std::ostream & notifier = getNotifierStream();
static std::ostream & debugger = getDebuggerStream();

//	import endl and ends from std into Loris:
using std::endl;
using std::ends;

}	//	end of namespace Loris

#endif	/* def __cplusplus */

/*
 *	handler assignment, c linkable:
 */

#ifdef __cplusplus
//  begin namespace
namespace Loris {
extern "C" {
#endif	//	def __cplusplus

//	These functions do not throw exceptions.
typedef void(*NotificationHandler)(const char * s);
NotificationHandler setNotifierHandler( NotificationHandler fn );
NotificationHandler setDebuggerHandler( NotificationHandler fn );

#ifdef __cplusplus
}	//	end extern "C"
}	//	end of namespace Loris
#endif	// def __cplusplus


#endif /* ndef INCLUDE_NOTIFIER_H */
