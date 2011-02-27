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
 * PartialList.h
 *
 * Definition of class Loris::PartialList class, mostly a wrapper for 
 * std::list< Partial >, to which most operations are forwarded.
 *
 * Kelly Fitz, 15 Feb 2011
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
//	Seems like we shouldn't need to include Partial.h, but 
//	without it, I can't instantiate a PartialList. I need
//	a definition of Partial for PartialList to be unambiguous.
#include "Partial.h"

#include "Notifier.h"

#include "PtrCopyOnWrite.h"

#include <functional>
#include <list>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	clone (non-member)
// ---------------------------------------------------------------------------
//  Cloning operation used by the reference-counting copy-on-write pointer 
//  class that is used to maintain the std::list and avoid unnecessary 
//  copying.
//
template <> 
inline std::list< Partial > * 
clone< std::list< Partial > >( const std::list< Partial > * tp )
{
    debugger << " +++ cloning list of " << tp->size() << " Partials" << endl; 
	return new std::list< Partial >( *tp );
}


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

    typedef std::list< Partial > list_of_Partials_type;
    typedef Ptr< list_of_Partials_type > list_ptr_type;

    list_ptr_type mList;    
    
public:

//  --- types ---

    typedef std::list< Partial >::size_type size_type;
    typedef std::list< Partial >::iterator iterator;
    typedef std::list< Partial >::const_iterator const_iterator;
    typedef std::list< Partial >::reference reference;
    typedef std::list< Partial >::const_reference const_reference;
    
    
//  --- lifecycle ---

    //  construct an empty list
    PartialList( void ); 
    
    
#if ! defined(NO_TEMPLATE_MEMBERS)
    template<class InIt>
    PartialList( InIt b, InIt e ) :
#else
    PartialList( iterator b, iterator e ) :
#endif
        mList( new list_of_Partials_type( b, e ) )
    {
    }
    
    
    PartialList( const PartialList & rhs );
    ~PartialList( void );
    PartialList & operator=( const PartialList & rhs );
    

//  --- std::list interface ---

    //  PartialList implements many members of the std::list interface
    //  by simply forwarding them to mList
    
    //  iterator access
    
    //! See std::list.
    iterator begin( void ) { return mList->begin(); }
    //! See std::list.
    iterator end( void ) { return mList->end(); }
    
    //! See std::list.
    const_iterator begin( void ) const { return mList->begin(); }
    //! See std::list.
    const_iterator end( void ) const { return mList->end(); }
    
    
    //  container access and mutation
    
    //! See std::list.
    Partial & front( void ) 
        { return mList->front(); }
    //! See std::list.
    const Partial & front( void ) const 
        { return mList->front(); }

    //! See std::list.
    Partial & back( void ) 
        { return mList->back(); }
    //! See std::list.
    const Partial & back( void ) const 
        { return mList->back(); }
    
    //! See std::list.
    void push_back( const Partial & val ) 
        { mList->push_back( val ); }
    //! See std::list.
    void push_front( const Partial & val ) 
        { mList->push_front( val ); }
    
    //! See std::list.
    iterator insert(iterator where, const Partial & val ) 
        { return mList->insert( where, val ); }
    
    //! See std::list.
#if ! defined(NO_TEMPLATE_MEMBERS)
    template<class InIt>
    void insert( iterator where, InIt first, InIt last )
        { return mList->insert( where, first, last ); }
#else
    void insert( iterator where, const_iterator first, const_iterator last )
        { return mList->insert( where, first, last ); }
    void insert( iterator where, iterator first, iterator last )
        { return mList->insert( where, first, last ); }
#endif

    
    //! See std::list.
    iterator erase( iterator where )
        { return mList->erase( where ); }
    
    //! See std::list.
    iterator erase( iterator first, iterator last )
        { return mList->erase( first, last ); }
    
    //! See std::list.
    //
    //  HEY opportunity for improvement here, this will trigger a clone operation
    //  in the COW pointer, but there is no reason to copy a bunch of Partials 
    //  just to delete them! Should be able to re-bind the Ptr to an empty list.
    void clear( void ) 
        { 
            //mList->clear(); 
            mList = list_ptr_type( new list_of_Partials_type );
        }

    //  query
    
    //! See std::list.
    bool empty( void ) const
        { return mList->empty();}
    
    //! See std::list.
    size_type size( void ) const 
        { return mList->size(); }
    
    
    //  sorting
    
    //! See std::list.
#if ! defined(NO_TEMPLATE_MEMBERS)
    template<class Comparitor>
    void sort( Comparitor c )
    {
        mList->sort( c );
    }
#else
    void sort( bool ( * c )( const Partial &, const Partial & ) )
    {
        mList->sort( c );
    }
#endif
    
    
    //  --- splice operations ---
    
    //  these are operations that occur in Loris that are implemented using 
    //  std::list::splice, but I don't want to actually support splice, I 
    //  prefer the more explicit operations. In some cases, splicing is 
    //  only used because copying expensive -- might be better to make 
    //  the underlying data shared using smart pointers.
    
    //  extract
    //! Remove a range of Partials from this List and return a new List containing
    //! those Partials.
    //!
    //! \param  b beginning of a range of Partials in this PartialList
    //! \param  e end of a range of Partials in this PartialList
    //! \return a new PartialList containing the Partials in the half-open range [b,e)
    //! \post   Partials in the range [b,e) are removed from this List
    //! \pre    [b,e) must describe a valid range of Partials in this List
    PartialList extract( iterator b, iterator e );
    
            
    //  splice
    //! Transfer the Partials from one List to this List, same as std::list::.splice
    //!
    //! \param  pos is the position in this List at which to insert the absorbed 
    //!         Partials
    //! \param  other is the List of Partials to absorb into this List
    //! \post   other is an empty List, its former contents have been transfered 
    //!         to this List
    //! \pre    pos is a valid position in this List
    //
    void splice( iterator pos, PartialList & adoptThese );    
        //  use in the procedural interface to return 
        //  Partials generated by an operation like analysis or morphing.
        //  In this case, probably the operation we want is just to return 
        //  a newly created List, but this would not be desirable in C
        //  (don't want to allocate a List).
        //
        //  Also used in the Distiller to construct a List of distilled Partials
        //  with unlabeled Partials at the end of the List. Instead of this, could
        //  extract (above) the unlabeled ones, and adopt them into the distilled
        //  List (at the end) as the last step in the distill operation.
    

};  //   end of class PartialList

typedef PartialList::iterator PartialListIterator;
typedef PartialList::const_iterator PartialListConstIterator;


}	//	end of namespace Loris

#endif /* ndef INCLUDE_PARTIALLIST_H */
