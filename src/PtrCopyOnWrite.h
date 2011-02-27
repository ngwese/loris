#ifndef INCLUDE_PTRCOPYONWRITE_H
#define INCLUDE_PTRCOPYONWRITE_H
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
 * PtrCopyOnWrite.h
 *
 * Template reference-counting copy-on-write pointer class, adapted from the
 * Ptr template in "Accelerated C++" by Koenig and Moo. 
 *
 * Kelly Fitz, 25 Feb 2011
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#include <cstddef>
#include <stdexcept>


//	begin namespace
namespace Loris {

template <class T> class Ptr {
public:
	// new member to copy the object conditionally when needed
	void make_unique() {
		if (*refptr != 1) {
			--*refptr;
			refptr = new size_t(1);
			p = p? clone(p): 0;
		}
	}

	// the rest of the class looks like `Ref_handle' except for its name
	Ptr(): refptr(new size_t(1)), p(0) { }
	Ptr(T* t): refptr(new size_t(1)), p(t) { }
	Ptr(const Ptr& h): refptr(h.refptr), p(h.p) { ++*refptr; }

	Ptr& operator=(const Ptr&);    // implemented analogously to 14.2/261
	~Ptr();                        // implemented analogously to 14.2/262
	operator bool() const { return p; }
	T& operator*() ;          // implemented analogously to 14.2/261
	T* operator->() ;         // implemented analogously to 14.2/261

    //  kelly added
    const T& operator*() const;          // implemented analogously to 14.2/261
	const T* operator->() const;         // implemented analogously to 14.2/261


private:
	T* p;
#ifdef _MSC_VER
	size_t* refptr;
#else
	std::size_t* refptr;
#endif
};

template <class T> T* clone(const T* tp)
{
	return tp->clone();
}



template<class T>
T& Ptr<T>::operator*() { make_unique(); if (p) return *p; throw std::runtime_error("unbound Ptr"); }

template<class T>
T* Ptr<T>::operator->()  { make_unique(); if (p) return p; throw std::runtime_error("unbound Ptr"); }

template<class T>
const T& Ptr<T>::operator*() const { if (p) return *p; throw std::runtime_error("unbound Ptr"); }

template<class T>
const T* Ptr<T>::operator->() const { if (p) return p; throw std::runtime_error("unbound Ptr"); }

template<class T>
Ptr<T>& Ptr<T>::operator=(const Ptr& rhs)
{
        ++*rhs.refptr;
        // \f2free the lhs, destroying pointers if appropriate\fP
        if (--*refptr == 0) {
                delete refptr;
                delete p;
        }

        // \f2copy in values from the right-hand side\fP
        refptr = rhs.refptr;
        p = rhs.p;
        return *this;
}

template<class T> Ptr<T>::~Ptr()
{
        if (--*refptr == 0) {
                delete refptr;
                delete p;
        }
}

}	//	end of namespace Loris

#endif  /* def INCLUDE_PTRCOPYONWRITE_H */

