#ifndef GUARD_Ptr_h
#define GUARD_Ptr_h
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
 * COWPtr.h
 *
 * Template Ptr class from chapter 14 of Accelerated C++, by Koenig and 
 * Moo, with very minor modifications. This smart pointer class supports
 * copy-on-write, and was selected for that reason, hence the name.
 *
 * Kelly Fitz, 22 May 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <cstddef>
#include <stdexcept>

template <class T> 
class COWPtr 
{
public:
	// define reference count type:
#ifdef _MSC_VER
	typedef size_t refcount_type;
#else
	typedef std::size_t refcount_type;
#endif

	// new member to copy the object conditionally when needed
	void make_unique() {
		if (*refptr != 1) {
			--*refptr;
			refptr = new refcount_type(1);
			p = p? clone(p): 0;
		}
	}

	// the rest of the class looks like `Ref_handle' except for its name
	COWPtr(): refptr(new refcount_type(1)), p(0) { }
	COWPtr(T* t): refptr(new refcount_type(1)), p(t) { }
	COWPtr(const COWPtr& h): refptr(h.refptr), p(h.p) { ++*refptr; }

	COWPtr& operator=(const COWPtr&);    // implemented analogously to 14.2/261
	~COWPtr();                        // implemented analogously to 14.2/262
	operator bool() const { return p; }
	T& operator*() const;          // implemented analogously to 14.2/261
	T* operator->() const;         // implemented analogously to 14.2/261

private:
	T* p;
	refcount_type* refptr;
};

template <class T> T* clone(const T* tp)
{
	return tp->clone();
}



template<class T>
T& COWPtr<T>::operator*() const { if (p) return *p; throw std::runtime_error("unbound Ptr"); }

template<class T>
T* COWPtr<T>::operator->() const { if (p) return p; throw std::runtime_error("unbound Ptr"); }


template<class T>
COWPtr<T>& 
COWPtr<T>::operator=(const COWPtr& rhs)
{
        ++*rhs.refptr;
        // free the lhs, destroying pointers if appropriate
        if (--*refptr == 0) 
		{
                delete refptr;
                delete p;
        }

        // copy in values from the right-hand side\
        refptr = rhs.refptr;
        p = rhs.p;
        return *this;
}

template<class T> 
COWPtr<T>::~COWPtr()
{
        if (--*refptr == 0) 
		{
                delete refptr;
                delete p;
        }
}


#endif
