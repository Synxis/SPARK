//////////////////////////////////////////////////////////////////////////////////
// SPARK Irrlicht Rendering library												//
// Copyright (C) 2009															//
// Thibault Lescoat -  info-tibo <at> orange <dot> fr							//
// Julien Fryer - julienfryer@gmail.com											//
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


// external libs
#include <irrlicht.h>

// SPARK lib
#include <SPK.h>
#include <SPK_IRR.h>

// windows only
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace irr;
using namespace SPK;
using namespace SPK::IRR;

float angleY = 0.0f;
float angleX = 45.0f;
float camPosZ = 5.0f;

const float PI = 3.14159265358979323846f;

System* particleSystem = NULL;
IrrlichtDevice* device = NULL;

// Calls back function to have particle bounce on the floor
bool bounceOnFloor(Particle& particle,float deltaTime)
{
	if (particle.position().y < 0.015f)
	{
		particle.position().y = 0.015f;
		particle.velocity().y = -particle.velocity().y * 0.6f;
	}
	return false;
}

// Input Receiver
class MyEventReceiver : public IEventReceiver
{
    public:
        virtual bool OnEvent(const SEvent& event)
        {
            if(event.EventType==EET_KEY_INPUT_EVENT)
            {
				if(event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown==false)
				{
					device->closeDevice();
				}
                if(event.KeyInput.Key == KEY_DELETE && event.KeyInput.PressedDown==false)
                {
                    particleSystem->empty();
                    return true;
                }
            }
            return false;
        }

        int oldMouseX,oldMouseY;
};

// Main function
int main(int argc, char *argv[])
{
	//!IRRLICHT
    video::E_DRIVER_TYPE chosenDriver = video::EDT_OPENGL;
#ifdef _WIN32
	if(MessageBoxA(0,"Do you want to use DirectX 9 ?","SPARK Basic Demo using Irrlicht",MB_YESNO) == 6)
        chosenDriver = video::EDT_DIRECT3D9;
#endif

    //!IRRLICHT
	device = createDevice(chosenDriver,
		core::dimension2d<u32>(640,480),
		32,
		false,
		false,
		false,
		new MyEventReceiver);

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();
    gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

    device->setWindowCaption(L"SPARK Basic Demo using Irrlicht");
	device->getCursorControl()->setVisible(false);
    irr::scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS(smgr->getRootSceneNode(),100.0f,0.0005f);
    cam->setPosition(irr::core::vector3df(0.0f,2.5f,2.0f));
    cam->setTarget(irr::core::vector3df(0.0f,0.0f,0.3f));
    cam->setNearValue(0.05f);

	// Inits Particle Engine
	randomSeed = device->getTimer()->getRealTime();
	//Vector3D gravity(0.0f,-0.8f,0.0f);

	IRRQuadRenderer* particleRenderer = IRRQuadRenderer::create(device);
	particleRenderer->setTexture(driver->getTexture("res\\triangle2.png"));
	particleRenderer->setTexturingMode(TEXTURE_2D);
	particleRenderer->setScale(0.05f,0.05f);
	particleRenderer->setBlending(BLENDING_ADD);
	particleRenderer->setOrientation(LOOK_AXIS,UP_DIRECTION,LOCK_UP);
	particleRenderer->lookVector.set(0.0f,1.0f,0.0f);
	particleRenderer->enableRenderingHint(DEPTH_WRITE,false);

	// Model
	Model* particleModel = Model::create(FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_ALPHA,FLAG_ALPHA);
	particleModel->setParam(PARAM_ALPHA,1.0f,0.0f);
	particleModel->setLifeTime(4.0f,5.0f);

	// Emitter
	Sphere* sphere = Sphere::create(Vector3D(),0.2f);
	RandomEmitter* particleEmitter = RandomEmitter::create();
	particleEmitter->setZone(sphere,false);
	particleEmitter->setFlow(200);
	particleEmitter->setForce(0.2f,0.4f);

	// Group
	Group* particleGroup = Group::create(particleModel,2100);
	particleGroup->addEmitter(particleEmitter);
	particleGroup->addModifier(PointMass::create(NULL,ALWAYS,1.0f,0.2f));
	particleGroup->setRenderer(particleRenderer);
	//particleGroup->setCustomUpdate(&bounceOnFloor);
	//particleGroup->setGravity(gravity);
	particleGroup->enableAABBComputing(true);
	
	particleSystem = IRRSystem::create(smgr->getRootSceneNode(),smgr);
	particleSystem->addGroup(particleGroup);
	particleSystem->enableAABBComputing(true);

	// setup some useful variables
	float time=(irr::f32)device->getTimer()->getTime() / 1000.0f,oldtime,deltaTime;
	float step = 0.0f;

	cout << "\nSPARK FACTORY AFTER INIT :" << endl;
	SPKFactory::getInstance().traceAll();

	//SDL_Delay(3000);
	//while (SDL_PollEvent(&event)){}

	while(device->run())
	{
		oldtime = time;
        time = (irr::f32)device->getTimer()->getTime() / 1000.0f;
        deltaTime = time - oldtime;

		// Changes the color of the model over time
		//step += deltaTime * 0.5f;
		//particleModel->setParam(PARAM_RED,0.6f + 0.4f * sin(step));
		//particleModel->setParam(PARAM_GREEN,0.6f + 0.4f * sin(step + PI * 2.0f / 3.0f));
		//particleModel->setParam(PARAM_BLUE,0.6f + 0.4f * sin(step + PI * 4.0f / 3.0f));

		driver->beginScene(true, true, irr::video::SColor(0,0,0,0));

		// Renders scene
		smgr->drawAll();

		irr::core::stringw infos; infos+="FPS: "; infos+=driver->getFPS(); infos+=" - Nb Particles: "; infos+=particleSystem->getNbParticles();
        guienv->getBuiltInFont()->draw(infos.c_str(),irr::core::rect<irr::s32>(0,0,170,20),irr::video::SColor(255,255,255,255));

		driver->endScene();
	}

	cout << "\nSPARK FACTORY BEFORE DESTRUCTION :" << endl;
	SPKFactory::getInstance().traceAll();
	SPKFactory::getInstance().destroyAll();
	cout << "\nSPARK FACTORY AFTER DESTRUCTION :" << endl;
	SPKFactory::getInstance().traceAll();
	device->drop();

	cout << endl;
	system("pause"); // Waits for the user to close the console

	return 0;
}