//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

#ifndef H_SPK_IOCONVERTERS
#define H_SPK_IOCONVERTERS

namespace SPK
{
	class SPK_PREFIX Loader
	{
	public :

		virtual Loader* clone() const = 0;

		virtual System* load(std::istream& os) const = 0;
		System* load(const std::string& path) const;

	protected :

		SPKObject* createSerializable(const std::string& name);
	};

	class SPK_PREFIX Saver
	{
	public :

		virtual Saver* clone() const = 0;

		virtual bool save(std::ostream& os,const WeakRef<const System>& system) const = 0;
		bool save(const std::string& path,const WeakRef<const System>& system) const;
	};
}

#endif
