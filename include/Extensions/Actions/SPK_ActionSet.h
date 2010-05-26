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

#ifndef H_SPK_ACTIONSET
#define H_SPK_ACTIONSET

#include <vector>

#include "Core/SPK_Action.h"

namespace SPK
{
	class SPK_PREFIX ActionSet : public Action
	{
	SPK_IMPLEMENT_REGISTERABLE(ActionSet)

	public :

		static inline ActionSet* create();
		
		virtual ~ActionSet();

		void addAction(Action* action);
		void removeAction(Action* action);
		Action* getAction(size_t index) const;
		inline size_t getNbActions() const;

		virtual void apply(Particle& particle) const;

		virtual Nameable* findByName(const std::string& name);

	private :

		inline ActionSet() {}
		ActionSet(const ActionSet& actionSet);

		std::vector<Action*> actions;
	};

	inline ActionSet* ActionSet::create()
	{
		return new ActionSet;
	}

	inline size_t ActionSet::getNbActions() const
	{
		return actions.size();
	}
}

#endif
