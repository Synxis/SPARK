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

#ifndef H_SPK_ACTION
#define H_SPK_ACTION

#include "Core/SPK_DEF.h"
#include "Core/SPK_Registerable.h"

namespace SPK
{
	class Particle;

	class Action : public Registerable
	{
	public :

		virtual inline ~Action() {}

		inline void setActive(bool active);
		inline bool isActive() const;

		virtual void apply(Particle& particle) const = 0;

	protected :

		inline Action();

	private :

		bool active;
	};

	inline Action::Action() :
		active(true)
	{}

	inline void Action::setActive(bool active)
	{
		this->active = active;
	}

	inline bool Action::isActive() const
	{
		return active;
	}
}

#endif
