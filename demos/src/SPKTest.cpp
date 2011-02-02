//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
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

#include <ctime>
#include <deque>
#include <cstdlib>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL.h>
#include <SPARK.h>
#include <SPARK_GL.h>


float angleX = 0.0f;
float angleY = 0.0f;
const float CAM_POS_Z = 3.0f;

// Loads a texture
bool loadTexture(GLuint& index,char* path,GLuint type,GLuint clamp,bool mipmap)
{
	SDL_Surface *particleImg; 
	particleImg = SDL_LoadBMP(path);
	if (particleImg == NULL)
	{
		std::cout << "Unable to load bitmap :" << SDL_GetError() << std::endl;
		return false;
	}

	// converts from BGR to RGB
	if ((type == GL_RGB)||(type == GL_RGBA))
	{
		const int offset = (type == GL_RGB ? 3 : 4);
		unsigned char* iterator = static_cast<unsigned char*>(particleImg->pixels);
		unsigned char *tmp0,*tmp1;
		for (int i = 0; i < particleImg->w * particleImg->h; ++i)
		{
			tmp0 = iterator;
			tmp1 = iterator + 2;
			std::swap(*tmp0,*tmp1);
			iterator += offset;
		}
	}

	glGenTextures(1,&index);
	glBindTexture(GL_TEXTURE_2D,index);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clamp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clamp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if (mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D,
			type,
			particleImg->w,
			particleImg->h,
			type,
			GL_UNSIGNED_BYTE,
			particleImg->pixels);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			type,
			particleImg->w,
			particleImg->h,
			0,
			type,
			GL_UNSIGNED_BYTE,
			particleImg->pixels);
	}

	SDL_FreeSurface(particleImg);

	return true;
}

// Draws the bounding box for a particle system
void drawBoundingBox(const SPK::System& system)
{
	if (!system.isAABBComputationEnabled())
		return;

	SPK::Vector3D AABBMin = system.getAABBMin();
	SPK::Vector3D AABBMax = system.getAABBMax();

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,0.0f);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	
	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);
	glEnd();
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SPARK 2 test",NULL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);	// double buffering

	// vsync
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);

	// AA
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
	
	SDL_SetVideoMode(800,600,32,SDL_OPENGL);

	SDL_Surface& screen = *SDL_GetVideoSurface();
	int screenWidth = screen.w;
	int screenHeight = screen.h;
	float screenRatio = (float)screen.w / (float)screen.h;

	// Loads particle texture
	GLuint textureParticle;
	if (!loadTexture(textureParticle,"res/explosion.bmp",GL_LUMINANCE,GL_CLAMP,true))
	{}//	return 1;

	// inits openGL
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glViewport(0,0,screenWidth,screenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,screenRatio,0.01f,20.0f);
	glEnable(GL_DEPTH_TEST);

	SPK::System::setClampStep(true,0.1f);			// clamp the step to 100 ms
	SPK::System::useAdaptiveStep(0.001f,0.01f);		// use an adaptive step from 1ms to 10ms (1000fps to 100fps)

	{
	SPK::Ref<SPK::System> system = SPK::System::create(true);
	system->setName("Test System");
	
	SPK::Ref<SPK::GL::GLQuadRenderer> renderer = SPK::GL::GLQuadRenderer::create();
	renderer->setBlendMode(SPK::BLEND_MODE_ADD);
	renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
	renderer->setTexture(textureParticle);
	renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	renderer->setAtlasDimensions(2,2);

	SPK::Ref<SPK::SphericEmitter> emitter = SPK::SphericEmitter::create(SPK::Vector3D(0.0f,0.0f,-1.0f),0.0f,3.14159f / 4.0f,SPK::Point::create(),true,-1,100.0f,0.2f,0.5f);

	SPK::Ref<SPK::Group> phantomGroup = system->createGroup(40);
	SPK::Ref<SPK::Group> trailGroup = system->createGroup(1000);

	phantomGroup->setName("Phantom Group");
	phantomGroup->setLifeTime(5.0f,5.0f);
	phantomGroup->setRadius(0.06f);
	phantomGroup->addEmitter(SPK::SphericEmitter::create(SPK::Vector3D(0.0f,1.0f,0.0f),0.0f,3.14159f / 4.0f,SPK::Point::create(SPK::Vector3D(0.0f,-1.0f,0.0f)),true,-1,2.0f,1.2f,2.0f));
	phantomGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-1.0f,0.0f)));
	phantomGroup->addModifier(SPK::Obstacle::create(SPK::Plane::create(SPK::Vector3D(0.0f,-1.0f,0.0f)),0.8f));
	phantomGroup->addModifier(SPK::EmitterAttacher::create(trailGroup,emitter,true));
	
	trailGroup->setName("Trail");
	trailGroup->setLifeTime(0.5f,1.0f);
	trailGroup->setRadius(0.06f);
	trailGroup->setRenderer(renderer);
	trailGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFF802080,0xFF000000));
	trailGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX,SPK::FloatRandomInitializer::create(0.0f,4.0f));
	trailGroup->setParamInterpolator(SPK::PARAM_ROTATION_SPEED,SPK::FloatRandomInitializer::create(-0.1f,1.0f));
	trailGroup->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInitializer::create(0.0f,2.0f * 3.14159f));
	trailGroup->addModifier(SPK::Rotator::create());

	float deltaTime = 0.0f;

	std::deque<clock_t> frameFPS;
	frameFPS.push_back(clock());

	SDL_Event event;
	bool exit = false;
	bool paused = false;
	while(!exit)
	{	
		while (SDL_PollEvent(&event))
		{
			// if esc is pressed, exit
			if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) || event.type == SDL_QUIT)
				exit = true;

			// if pause is pressed, the system is paused
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_PAUSE)
				paused = !paused;

			// Moves the camera with the mouse
			if (event.type == SDL_MOUSEMOTION)
			{
				angleY += event.motion.xrel * 0.5f;
				angleX += event.motion.yrel * 0.5f;
			}
		}

		if (!paused)
			system->updateParticles(deltaTime);
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();  
		glTranslatef(0.0f,0.0f/*0.2f*/,-CAM_POS_Z);
		glRotatef(angleX,1.0f,0.0f,0.0f);
		glRotatef(angleY,0.0f,1.0f,0.0f);

		glColor4f(0.5f,0.0f,0.0f,1.0f);
		glBegin(GL_QUADS);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glEnd();

		drawBoundingBox(*system);
		SPK::GL::GLRenderer::saveGLStates();
		system->renderParticles();
		SPK::GL::GLRenderer::restoreGLStates();

		SDL_GL_SwapBuffers();

		clock_t currentTick = clock();
		deltaTime = (float)(currentTick - frameFPS.back()) / CLOCKS_PER_SEC;
		frameFPS.push_back(currentTick);
		while((frameFPS.back() - frameFPS.front() > 1000)&&(frameFPS.size() > 2))
			frameFPS.pop_front();
	};

	// To be registered automatically
	SPK::IO::IOManager::getInstance().registerObject<SPK::System>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::Group>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::SphericEmitter>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::Point>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::Gravity>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::Obstacle>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::Plane>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::EmitterAttacher>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::ColorSimpleInterpolator>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::FloatRandomInitializer>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::Rotator>();
	SPK::IO::IOManager::getInstance().registerObject<SPK::GL::GLQuadRenderer>();

	SPK::IO::XMLSaver saver;
	saver.save("test.xml",system);

	SPK::IO::XMLLoader loader;
	SPK::Ref<SPK::System> system2 = loader.load("test.xml");

	saver.save("test2.xml",system2); // integrity test. test2.xml must be equivalent to test.xml

	SPK_DUMP_MEMORY
	}
	SPK_DUMP_MEMORY

	SDL_Quit();
	std::system("pause");
	
	return 0;
} 