#ifndef INCLUDE_PARTIALLIST_H
#define INCLUDE_PARTIALLIST_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2011 by Kelly Fitz and Lippold Haken
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
 *	PartialList.h
 *
 *	Type definition of Loris::PartialList, which is just a name
 *	for std::list< Loris::Partial >.
 *
 * Kelly Fitz, 6 March 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
//	Seems like we shouldn't need to include Partial.h, but 
//	without it, I can't instantiate a PartialList. I need
//	a definition of Partial for PartialList to be unambiguous.
#include "Partial.h"

#include <functional>
#include <list>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	class PartialList
//
//	PartialList is a typedef for a std::list<> of Loris Partials. The
//	oscciated bidirectional iterators are also defined as
//	PartialListIterator and PartialListConstIterator. Since these are
//	simply typedefs, they classes have identical interfaces to std::list,
//	std::list::iterator, and std::list::const_iterator, respectively.
//

class PartialList
{ 
private: 

    std::list< Loris::Partial > mList;
    
    
public:
    typedef std::list< Partial >::size_type size_type;
    typedef std::list< Partial >::iterator iterator;
    typedef std::list< Partial >::const_iterator const_iterator;
    typedef std::list< Partial >::reference reference;
    typedef std::list< Partial >::const_reference const_reference;
    

    //  construct an empty list
    PartialList( void ) 
    {
    }
    
#if ! defined(NO_TEMPLATE_MEMBERS)
    template<class InIt>
    PartialList( InIt b, InIt e ) :
#else
    PartialList( iterator b, iterator e ) :
#endif
        mList( b, e )
    {
    }
    
    
    //  default copy/assign/destroy are OK
    
    //  iterator access
    iterator begin( void ) { return mList.begin(); }
    iterator end( void ) { return mList.end(); }
    
    const_iterator begin( void ) const { return mList.begin(); }
    const_iterator end( void ) const { return mList.end(); }
    
    
    //  container interface
    Partial & front( void ) { return mList.front(); }
    const Partial & front( void ) const { return mList.front(); }

    Partial & back( void ) { return mList.back(); }
    const Partial & back( void ) const { return mList.back(); }
    
    void push_back( const Partial & val ) { mList.push_back( val ); }
    void push_front( const Partial & val ) { mList.push_front( val ); }
    
    iterator insert(iterator where, const Partial & val ) { return mList.insert( where, val ); }
    
#if ! defined(NO_TEMPLATE_MEMBERS)
    template<class InIt>
    void insert( iterator where, InIt first, InIt last )
    { 
        return mList.insert( where, first, last );
    }
#else
    void insert( iterator where, const_iterator first, const_iterator last )
    { 
        return mList.insert( where, first, last );
    }
    void insert( iterator where, iterator first, iterator last )
    { 
        return mList.insert( where, first, last );
    }
#endif

    size_type size( void ) const { return mList.size(); }
    
    iterator erase( iterator where )
    { 
        return mList.erase( where );
    }
    
    iterator erase( iterator first, iterator last )
    {
        return mList.erase( first, last );
    }
    
    bool empty( void ) const
    {
        return mList.empty();
    }
    
    void clear( void ) { mList.clear(); }
    
    //  sort
#if ! defined(NO_TEMPLATE_MEMBERS)
    template<class Comparitor>
    void sort( Comparitor c )
    {
        mList.sort( c );
    }
#else
    //  define this type in Partial.h as PartialComparitor? nah, not useful enough
    void sort( bool ( * c )( const Partial &, const Partial & ) )
    {
        mList.sort( c );
    }
#endif
    
    
    //  splice - 
    //  these are operations that occur in Loris that are implemented using 
    //  std::list::splice, but I don't want to actually support splice, I 
    //  prefer the more explicit operations
    //
    //  extract
    //  one operation is to extract is range of Partials from a List and 
    //  create a new List
    //
    //  this is icky, because begin and end _must_ be iterators on this List!
    PartialList extract( iterator begin, iterator end )   // removes that range from this list, returns it in a new list
    {
        PartialList ret;
        ret.mList.splice( ret.begin(), mList, begin, end );        
        return ret;
    }
    
    //  absorb
    //  the other operation is to transfer the Partials from one List 
    //  to another List -- use in the procedural interface to return 
    //  Partials generated by an operation like analysis or morphing.
    //  In this case, probably the operation we want is just to return 
    //  a newly created List, but this would not be desirable in C
    //  (don't want to allocate a List).
    //
    //  Also used in the Distiller to construct a List of distilled Partials
    //  with unlabeled Partials at the end of the List. Instead of this, could
    //  extract (above) the unlabeled ones, and adopt them into the distilled
    //  List (at the end) as the last step in the distill operation.
    void absorb( iterator pos, PartialList & adoptThese ) // or transfer
    {
        mList.splice( pos, adoptThese.mList );
    }
    
    
    //  a variant of this operation only transfers a single Partial, but
    //  this could probably be performed with insert and erase -- used in 
    //  the procedural interface to implement extractIf and extractLabeled,
    //  and only due to an error in std::stable_partition (on some platform)
    //  that caused the operation to hang! ach!
    void absorb( iterator ins_pos, PartialList & fromHere, iterator thisOne ) // or transfer
    {
        mList.splice( ins_pos, fromHere.mList, thisOne );
    }


    //
    //

};  //   end of class PartialList

typedef PartialList::iterator PartialListIterator;
typedef PartialList::const_iterator PartialListConstIterator;

/*
//  old way
typedef std::list< Loris::Partial > PartialList;
typedef std::list< Loris::Partial >::iterator PartialListIterator;
typedef std::list< Loris::Partial >::const_iterator PartialListConstIterator;
*/

}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALLIST_H */
