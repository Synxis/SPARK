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

#include "Rendering/Irrlicht/CSPKParticleSystemNode.h"
#include "Core/SPK_Group.h"

namespace irr
{
namespace scene
{
	CSPKParticleSystemNode::CSPKParticleSystemNode(
		irr::scene::ISceneNode* parent,
		irr::scene::ISceneManager* mgr,
		bool initialize,
		bool worldTransformed,
		irr::s32 id) :
			ISceneNode(parent,mgr,id),
			SPK::System(initialize),
			worldTransformed(worldTransformed),
			autoUpdate(true),
			onlyWhenVisible(false),
			alive(true),
			lastUpdatedTime(0)
    {}

	CSPKParticleSystemNode::CSPKParticleSystemNode(
		const CSPKParticleSystemNode& system,
		ISceneNode* newParent,
		ISceneManager* newManager) :
			ISceneNode(NULL,NULL),
			SPK::System(system),
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
		cloneMembers(const_cast<CSPKParticleSystemNode*>(&system),newManager);
	}

	CSPKParticleSystemNode* CSPKParticleSystemNode::clone(ISceneNode* newParent,ISceneManager* newManager)
	{
		CSPKParticleSystemNode* nb = new CSPKParticleSystemNode(*this,newParent,newManager);
		if (Parent != NULL)
			nb->drop();
		return nb;
	}

	const irr::core::aabbox3d<irr::f32>& CSPKParticleSystemNode::getBoundingBox() const
    {
		BBox.MaxEdge = SPK::IRR::spk2irr(getAABBMax());
		BBox.MinEdge = SPK::IRR::spk2irr(getAABBMin());
        return BBox;
    }

	void CSPKParticleSystemNode::renderParticles() const
	{
        SceneManager->getVideoDriver()->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
        SPK::System::renderParticles();
	}

	bool CSPKParticleSystemNode::updateParticles(float deltaTime)
	{
		updateCameraPosition();
		alive = SPK::System::updateParticles(deltaTime);
		return alive;
	}

	void CSPKParticleSystemNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this,irr::scene::ESNRP_TRANSPARENT_EFFECT); // Draws in transparent effect pass (may be optimized)

       ISceneNode::OnRegisterSceneNode();
	}

	void CSPKParticleSystemNode::OnAnimate(irr::u32 timeMs)
	{
		ISceneNode::OnAnimate(timeMs);

		if (lastUpdatedTime == 0)
			lastUpdatedTime = timeMs;

        if (autoUpdate && (!onlyWhenVisible || IsVisible))
			updateParticles((timeMs - lastUpdatedTime) * 0.001f);

        lastUpdatedTime = timeMs;
	}

	void CSPKParticleSystemNode::updateAbsolutePosition()
	{
		ISceneNode::updateAbsolutePosition();

		if (worldTransformed)
		{
			this->setTransform(AbsoluteTransformation.pointer());
			updateTransform();
			AbsoluteTransformation.makeIdentity();
		}
	}

	void CSPKParticleSystemNode::updateCameraPosition() const
	{
		for (std::vector<SPK::Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			if ((*it)->isDistanceComputationEnabled())
			{
				irr::core::vector3df pos = SceneManager->getActiveCamera()->getAbsolutePosition();
				if (!worldTransformed)
				{
					irr::core::matrix4 invTransform;
					AbsoluteTransformation.getInversePrimitive(invTransform);
					invTransform.transformVect(pos);
				}
				setCameraPosition(SPK::IRR::irr2spk(pos));
				break;
			}
	}
}}
