//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010														//
// Julien Fryer - julienfryer@gmail.com											//
// Thibault Lescoat - info-tibo <at> orange <dot> fr							//
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

#include <SPARK_Core.h>
#include "Rendering/Irrlicht/SPK_IRR_System.h"

namespace SPK
{
namespace IRR
{
	IRRSystem::IRRSystem(
		irr::scene::ISceneNode* parent,
		irr::scene::ISceneManager* mgr,
		bool initialize,
		bool worldTransformed,
		irr::s32 id) :
			irr::scene::ISceneNode(parent,mgr,id),
			System(initialize),
			worldTransformed(worldTransformed),
			autoUpdate(true),
			onlyWhenVisible(false),
			alive(true),
			lastUpdatedTime(0)
    {
		enableAABBComputation(true); // as culling is enabled by default in Irrlicht

		if (parent != NULL && !isInitialized())
			SPK_LOG_WARNING("IRRSystem(ISceneNode*,ISceneManager*,bool,bool) - A uninitialized system may have a NULL parent");
	}

	IRRSystem::IRRSystem(
		const IRRSystem& system,
		irr::scene::ISceneNode* newParent,
		irr::scene::ISceneManager* newManager) :
			irr::scene::ISceneNode(NULL,NULL),
			System(system),
			worldTransformed(system.worldTransformed),
			autoUpdate(system.autoUpdate),
			onlyWhenVisible(system.onlyWhenVisible),
			alive(system.alive),
			lastUpdatedTime(0)
	{
		if (newParent == NULL)
			newParent = system.Parent;
		if (newManager == NULL)
			newManager = system.SceneManager;

		setParent(newParent);
		
		if (getParent() != NULL && !isInitialized())
			SPK_LOG_WARNING("IRRSystem(const IRRSystem&,ISceneNode*,ISceneManager*) - A uninitialized system may have a NULL parent");
		
		cloneMembers(const_cast<IRRSystem*>(&system),newManager);
	}

	IRRSystem* IRRSystem::clone(irr::scene::ISceneNode* newParent,irr::scene::ISceneManager* newManager)
	{
		IRRSystem* nb = new IRRSystem(*this,newParent,newManager);
		if (Parent != NULL)
			nb->drop();
		return nb;
	}

	const irr::core::aabbox3d<irr::f32>& IRRSystem::getBoundingBox() const
    {
		BBox.MaxEdge = spk2irr(getAABBMax());
		BBox.MinEdge = spk2irr(getAABBMin());
        return BBox;
    }

	void IRRSystem::renderParticles() const
	{
        SceneManager->getVideoDriver()->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
        System::renderParticles();
	}

	bool IRRSystem::updateParticles(float deltaTime)
	{
		updateCameraPosition();

		if (!isAABBComputationEnabled() && AutomaticCullingState != irr::scene::EAC_OFF)
		{
			SPK_LOG_INFO("IRRSystem::updateParticles(float) - The culling is activated for the system but not the bounding box computation - BB computation is enabled");
			enableAABBComputation(true);
		}

		alive = System::updateParticles(deltaTime);
		return alive;
	}

	void IRRSystem::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this,irr::scene::ESNRP_TRANSPARENT_EFFECT); // Draws in transparent effect pass (may be optimized)

       ISceneNode::OnRegisterSceneNode();
	}

	void IRRSystem::OnAnimate(irr::u32 timeMs)
	{
		ISceneNode::OnAnimate(timeMs);

		if (lastUpdatedTime == 0)
			lastUpdatedTime = timeMs;

        if (autoUpdate && (!onlyWhenVisible || IsVisible))
			updateParticles((timeMs - lastUpdatedTime) * 0.001f);

        lastUpdatedTime = timeMs;
	}

	void IRRSystem::updateAbsolutePosition()
	{
		ISceneNode::updateAbsolutePosition();

		if (worldTransformed)
		{
			getTransform().set(AbsoluteTransformation.pointer());
			AbsoluteTransformation.makeIdentity();
		}
	}

	void IRRSystem::updateCameraPosition() const
	{
		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			if ((*it)->isDistanceComputationEnabled())
			{
				irr::core::vector3df pos = SceneManager->getActiveCamera()->getAbsolutePosition();
				if (!worldTransformed)
				{
					irr::core::matrix4 invTransform;
					AbsoluteTransformation.getInversePrimitive(invTransform);
					invTransform.transformVect(pos);
				}
				setCameraPosition(irr2spk(pos));
				break;
			}
	}
}}
