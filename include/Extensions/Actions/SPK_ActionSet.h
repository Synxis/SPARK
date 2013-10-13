//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 - Julien Fryer - julienfryer@gmail.com				//
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

namespace SPK
{
	class SPK_PREFIX ActionSet : public Action
	{
	public :

		static  Ref<ActionSet> create();
		
		virtual ~ActionSet();

		void addAction(const Ref<Action>& action);
		void removeAction(const Ref<Action>& action);
		const Ref<Action>& getAction(size_t index) const;
		size_t getNbActions() const;
		void clearActions();

		virtual void apply(Particle& particle) const;

		virtual Ref<SPKObject> findByName(const std::string& name);

	public :
		spark_description(ActionSet, Action)
		(
			spk_array(Ref<Action>, actions, addAction, removeAction, clearActions, getAction, getNbActions);
		);

	private :

		ActionSet() {}
		ActionSet(const ActionSet& actionSet);

		std::vector<Ref<Action> > actions;
	};

	inline Ref<ActionSet> ActionSet::create()
	{
		return SPK_NEW(ActionSet);
	}

	inline size_t ActionSet::getNbActions() const
	{
		return actions.size();
	}
}

#endif
