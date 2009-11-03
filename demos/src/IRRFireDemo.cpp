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

float angleY = 10.0f;
float angleX = -45.0f;
float camPosZ = 5.0f;

int deltaTime = 0;

const float PI = 3.14159265358979323846f;

const string STR_NB_PARTICLES = "NB PARTICLES : ";
const string STR_FPS = "FPS : ";

string strNbParticles = STR_NB_PARTICLES;
string strFps = STR_FPS;

Group* fireGroup = NULL;
Group* smokeGroup = NULL;
Emitter* smokeEmitter = NULL;
System* particleSystem = NULL;

IrrlichtDevice* device = NULL;
scene::ICameraSceneNode* cam = NULL;

bool smokeEnabled = true;

// Callback function to create smoke when a fire particle dies (1 chance out of 4)
void createSmoke(Particle& particle)
{
	if (random(0,4) == 0)
		smokeGroup->addParticles(1,particle.position(),smokeEmitter);
}

// Input Receiver
class MyEventReceiver : public IEventReceiver
{
    public:
		MyEventReceiver() :
		  IEventReceiver(),
		  init(false)
		  {}

        virtual bool OnEvent(const SEvent& event)
        {
            if(event.EventType==EET_KEY_INPUT_EVENT)
            {
				if(event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown==false)
				{
					device->closeDevice();
				}
                if(event.KeyInput.Key == KEY_SPACE && event.KeyInput.PressedDown==false)
                {
                    smokeEnabled=!smokeEnabled;
                    if(!smokeEnabled)
                        fireGroup->setCustomDeath(NULL);
                    else
                        fireGroup->setCustomDeath(&createSmoke);
                    return true;
                }
                if(event.KeyInput.Key == KEY_DELETE && event.KeyInput.PressedDown==false)
                {
                    particleSystem->empty();
                    return true;
                }
            }
            else if(event.EventType == EET_MOUSE_INPUT_EVENT)
            {
				if (init)
				{
					angleX += (oldMouseX - event.MouseInput.X)*0.1f;
					angleY += (oldMouseY - event.MouseInput.Y)*0.1f;
                    
					if(angleY < 1.0f) angleY=1.0f;
					if(angleY > 70.0f) angleY=70.0f;
					camPosZ -= event.MouseInput.Wheel;
					if(camPosZ < 3.0f) camPosZ=3.0f;
					if(camPosZ > 18.0f) camPosZ=18.0f;

					cam->setPosition(core::vector3df(camPosZ*sinf(angleX*core::DEGTORAD)*sinf((90.0f-angleY)*core::DEGTORAD),
						camPosZ*cosf((90.0f-angleY)*core::DEGTORAD),camPosZ*cosf(angleX*core::DEGTORAD)*sinf((90.0f-angleY)*core::DEGTORAD)));
					cam->setTarget(core::vector3df());
				}

				init = true;
				oldMouseX=event.MouseInput.X;
				oldMouseY=event.MouseInput.Y;

                return true;
            }
            return false;
        }

        int oldMouseX,oldMouseY;

	private :
	
		bool init;
};

// Main function
int main(int argc, char *argv[])
{
	//!IRRLICHT
    video::E_DRIVER_TYPE chosenDriver = video::EDT_OPENGL;
#ifdef _WIN32
	if(MessageBoxA(0,"Do you want to use DirectX 9 ?","SPARK Fire Demo using Irrlicht",MB_YESNO) == 6)
        chosenDriver = video::EDT_DIRECT3D9;
#endif

    //!IRRLICHT
	MyEventReceiver* evtrcv = new MyEventReceiver;
	device = createDevice(chosenDriver,
		core::dimension2d<u32>(640,480),
		32,
		false,
		false,
		false,
		evtrcv);

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();
    gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

    device->setWindowCaption(L"SPARK Fire Demo using Irrlicht");
	device->getCursorControl()->setVisible(false);
    cam = smgr->addCameraSceneNode(0,core::vector3df(camPosZ*sinf(angleX*core::DEGTORAD)*sinf((90.0f-angleY)*core::DEGTORAD),
        camPosZ*cosf((90.0f-angleY)*core::DEGTORAD),camPosZ*cosf(angleX*core::DEGTORAD)*sinf((90.0f-angleY)*core::DEGTORAD)),
        core::vector3df());
    cam->setNearValue(0.05f);

	// plane
    core::array<video::S3DVertex2TCoords> pVertices;
    pVertices.set_used(4);
    core::array<u16> pIndices;
    pIndices.set_used(6);
    for(int y=0; y<2; y++){
    for(int x=0; x<2; x++){
        pVertices[x+2*y].Pos = core::vector3df((x*2-1)*5.0f,-1.2f,(y*2-1)*5.0f);
        pVertices[x+2*y].Normal = core::vector3df(0,1,0);
        pVertices[x+2*y].TCoords = core::vector2df((x*2-1)*5.0f,(y*2-1)*5.0f);
        pVertices[x+2*y].TCoords2 = core::vector2df((f32)x,(f32)y);
        pVertices[x+2*y].Color = video::SColor(255,255,255,255);
    }
    }
    pIndices[0] = 0;
    pIndices[1] = 2;
    pIndices[2] = 1;
    pIndices[3] = 2;
    pIndices[4] = 3;
    pIndices[5] = 1;
    scene::IMeshBuffer* buff = new scene::CMeshBuffer<video::S3DVertex2TCoords>;
    buff->append(pVertices.pointer(),4,pIndices.pointer(),6);
    buff->getMaterial().MaterialType = video::EMT_LIGHTMAP;
    buff->getMaterial().TextureLayer[0].Texture = driver->getTexture("res\\grass.bmp");
    buff->getMaterial().TextureLayer[1].Texture = driver->getTexture("res\\lightmap3.bmp");
    buff->getMaterial().Lighting = false;
    scene::SMesh* mesh = new scene::SMesh; mesh->addMeshBuffer(buff);
    scene::IMeshSceneNode* plane = smgr->addMeshSceneNode(mesh);

	// random seed
	randomSeed = device->getTimer()->getRealTime();
	// Sets the update step
	System::setClampStep(true,0.1f);			// clamp the step to 100 ms
	System::useAdaptiveStep(0.001f,0.01f);		// use an adaptive step from 1ms to 10ms (1000fps to 100fps)

	// Inits Particle Engine
	// Those vectors are inverted gravity so that the fire and smoke will rise up
	Vector3D fireForce(0.0f,0.8f,0.0f);
	Vector3D smokeForce(0.0f,0.4f,0.0f);

	// Renderers
	IRRQuadRenderer* fireRenderer = IRRQuadRenderer::create(device);
	fireRenderer->setScale(0.3f,0.3f);
	fireRenderer->setTexture(driver->getTexture("res\\fire.bmp"));
	fireRenderer->setTexturingMode(TEXTURE_2D);
	fireRenderer->setBlending(BLENDING_ADD);
	fireRenderer->enableRenderingHint(DEPTH_WRITE,false);

	IRRQuadRenderer* smokeRenderer = IRRQuadRenderer::create(device);
	smokeRenderer->setScale(0.3f,0.3f);
	smokeRenderer->setTexture(driver->getTexture("res\\smoke.bmp"));
	smokeRenderer->setTexturingMode(TEXTURE_2D);
	smokeRenderer->setBlending(BLENDING_ALPHA);
	smokeRenderer->enableRenderingHint(DEPTH_WRITE,false);

	// Models
	Model* fireModel = Model::create(FLAG_ALPHA | FLAG_SIZE | FLAG_ANGLE,
		FLAG_ALPHA | FLAG_ANGLE,
		FLAG_SIZE | FLAG_ANGLE);
	fireModel->setParam(PARAM_ALPHA,0.4f,0.0f);
	fireModel->setParam(PARAM_SIZE,0.4f,1.2f);
	fireModel->setParam(PARAM_ANGLE,0.0f,2.0f * PI,0.0f,2.0f * PI); // the texture is rotated over time
	fireModel->setLifeTime(0.2f,2.0f);

	Model* smokeModel = Model::create(FLAG_ALPHA | FLAG_SIZE | FLAG_ANGLE,
		FLAG_ALPHA | FLAG_SIZE,
		FLAG_SIZE | FLAG_ANGLE);
	smokeModel->setParam(PARAM_ALPHA,0.075f,0.0f);
	smokeModel->setParam(PARAM_SIZE,0.2f,0.8f,4.8f,4.8f);
	smokeModel->setParam(PARAM_ANGLE,0.0f,2.0f * PI);
	smokeModel->setLifeTime(3.0f,6.0f);

	// Emitters
	// The emitters are arranged so that the fire looks realistic
	StraightEmitter* fireEmitter1 = StraightEmitter::create(Vector3D(1.0f,0.2f,0.0f));
	fireEmitter1->setZone(Sphere::create(Vector3D(0.15f,-1.2f,0.075f),0.1f));
	fireEmitter1->setFlow(100);
	fireEmitter1->setForce(0.1f,0.5f);

	StraightEmitter* fireEmitter2 = StraightEmitter::create(Vector3D(0.0f,1.0f,0.0f));
	fireEmitter2->setZone(Sphere::create(Vector3D(0.0f,-1.0f,0.0f),0.5f));
	fireEmitter2->setFlow(500);
	fireEmitter2->setForce(0.1f,0.8f);

	StraightEmitter* fireEmitter3 = StraightEmitter::create(Vector3D(-0.6f,0.4f,-0.8f));
	fireEmitter3->setZone(Sphere::create(Vector3D(-0.375f,-1.15f,-0.375f),0.3f));
	fireEmitter3->setFlow(300);
	fireEmitter3->setForce(0.1f,0.6f);

	StraightEmitter* fireEmitter4 = StraightEmitter::create(Vector3D(-0.8f,0.2f,0.2f));
	fireEmitter4->setZone(Sphere::create(Vector3D(-0.255f,-1.2f,0.225f),0.2f));
	fireEmitter4->setFlow(100);
	fireEmitter4->setForce(0.1f,0.5f);

	StraightEmitter* fireEmitter5 = StraightEmitter::create(Vector3D(0.1f,0.2f,-1.0f));
	fireEmitter5->setZone(Sphere::create(Vector3D(-0.075f,-1.2f,-0.3f),0.2f));
	fireEmitter5->setFlow(100);
	fireEmitter5->setForce(0.1f,0.5f);

	smokeEmitter = SphericEmitter::create(Vector3D(0.0f,1.0f,0.0f),0.0f,0.5f * PI);
	smokeEmitter->setForce(0.5f,1.0f);

	// Groups
	fireGroup = Group::create(fireModel,1400);
	fireGroup->addEmitter(fireEmitter2);
	fireGroup->addEmitter(fireEmitter1);
	fireGroup->addEmitter(fireEmitter3);
	fireGroup->addEmitter(fireEmitter4);
	fireGroup->addEmitter(fireEmitter5);
	fireGroup->setRenderer(fireRenderer);
	fireGroup->setCustomDeath(&createSmoke);
	fireGroup->setGravity(fireForce);
	fireGroup->enableAABBComputing(true);

	smokeGroup = Group::create(smokeModel,1800);
	smokeGroup->setRenderer(smokeRenderer);
	smokeGroup->setGravity(smokeForce);
	smokeGroup->enableAABBComputing(true);
	
	// System
	particleSystem = IRRSystem::create(smgr->getRootSceneNode(),smgr);
	particleSystem->addGroup(smokeGroup);
	particleSystem->addGroup(fireGroup);
	particleSystem->enableAABBComputing(true);

	// setup some useful variables
	float time=(f32)device->getTimer()->getTime() / 1000.0f,oldtime,deltaTime;
	float step = 0.0f;
	float lastLightTime = 0;

	cout << "\nSPARK FACTORY AFTER INIT :" << endl;
	SPKFactory::getInstance().traceAll();

	while(device->run())
	{
		oldtime = time;
        time = (f32)device->getTimer()->getTime() / 1000.0f;
        deltaTime = time - oldtime;

		// lightmap effect
        if(time - lastLightTime >= 0.05f)
        {
            float decal = SPK::random(0.95f,1.05f);
            for(int y=0; y<2; y++)
                for(int x=0; x<2; x++)
                    ((video::S3DVertex2TCoords*)(buff->getVertices()))[x+2*y].TCoords2 = core::vector2df((x-0.5f)*decal+0.5f,(y-0.5f)*decal+0.5f);
            lastLightTime = time;
        }

		driver->beginScene(true, true, video::SColor(0,0,0,0));

		// Renders scene
		smgr->drawAll();

		core::stringw infos; infos+="FPS: "; infos+=driver->getFPS(); infos+=" - Nb Particles: "; infos+=particleSystem->getNbParticles();
        guienv->getBuiltInFont()->draw(infos.c_str(),core::rect<s32>(0,0,170,20),video::SColor(255,255,255,255));

		driver->endScene();

		// clip mouse
        if(device->getCursorControl()->getPosition().X < 20)
        {
            device->getCursorControl()->setPosition(620,device->getCursorControl()->getPosition().Y);
            evtrcv->oldMouseX = 620;
        }
        if(device->getCursorControl()->getPosition().X > 620)
        {
            device->getCursorControl()->setPosition(20,device->getCursorControl()->getPosition().Y);
            evtrcv->oldMouseX = 20;
        }
        if(device->getCursorControl()->getPosition().Y < 20)
        {
            device->getCursorControl()->setPosition(device->getCursorControl()->getPosition().X,460);
            evtrcv->oldMouseY = 460;
        }
        if(device->getCursorControl()->getPosition().Y > 460)
        {
            device->getCursorControl()->setPosition(device->getCursorControl()->getPosition().X,20);
            evtrcv->oldMouseY = 20;
        }
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