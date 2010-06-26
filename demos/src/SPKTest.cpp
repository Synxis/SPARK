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

float angleX = -90.0f;
float angleY = 0.0f;
const float CAM_POS_Z = 3.0f;

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

	// inits openGL
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glViewport(0,0,screenWidth,screenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,screenRatio,0.01f,20.0f);
	glEnable(GL_DEPTH_TEST);

	SPK::System::setClampStep(true,0.1f);			// clamp the step to 100 ms
	SPK::System::useAdaptiveStep(0.001f,0.01f);		// use an adaptive step from 1ms to 10ms (1000fps to 100fps)

	SPK::System* system = new SPK::System(true);
	system->enableAABBComputation(true);

	SPK::GL::GLQuadRenderer* quadRenderer = SPK::GL::GLQuadRenderer::create();
	quadRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
	quadRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);

	SPK::GL::GLLineTrailRenderer* lineTrailRenderer = SPK::GL::GLLineTrailRenderer::create();
	//lineTrailRenderer->setNbSamples(32);
	lineTrailRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
	lineTrailRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);

	SPK::RandomEmitter* emitter = SPK::RandomEmitter::create();
	emitter->setZone(SPK::Point::create());
	emitter->setForce(0.4f,0.6f);
	emitter->setFlow(50);

	SPK::Gravity* gravity = SPK::Gravity::create(SPK::Vector3D(0.0f,-0.5f,0.0f));
	SPK::Friction* friction = SPK::Friction::create(0.2f);

	SPK::ColorGraphInterpolator* graphInterpolator = SPK::ColorGraphInterpolator::create();
	graphInterpolator->addEntry(0.0f,0xFF000088);
	graphInterpolator->addEntry(0.5f,0x00FF0088);
	graphInterpolator->addEntry(1.0f,0x0000FF88);

	SPK::Obstacle* obstacle = SPK::Obstacle::create(SPK::Plane::create(SPK::Vector3D(0.0f,-0.8f,0.0f),SPK::Vector3D(0.0f,1.0f,0.0f)));

	SPK::Group* group1 = system->createGroup(100);
	group1->setRadius(0.05f);
	group1->setLifeTime(1.0f,2.0f);
	//group1->setColorInterpolator(SPK::ColorDefaultInitializer::create(0xFFFF2288));
	group1->setColorInterpolator(graphInterpolator);
	group1->setParamInterpolator(SPK::PARAM_SIZE,SPK::FloatRandomInterpolator::create(0.8f,1.2f,0.0f,0.0f));
	group1->setParamInterpolator(SPK::PARAM_ROTATION_SPEED,SPK::FloatRandomInitializer::create(-2.0f,2.0f));
	group1->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInitializer::create(0.0f,2 * 3.14159f));
	group1->addEmitter(emitter);
	group1->addModifier(gravity);
	group1->addModifier(friction);
	group1->addModifier(obstacle);
	group1->addModifier(SPK::Rotator::create());
	group1->setRenderer(quadRenderer);
	group1->setDeathAction(SPK::SpawnParticlesAction::create(5,10,1,emitter));

	SPK::Group* group2 = system->createGroup(1000);
	group2->setRadius(0.0f);
	group2->setLifeTime(1.0f,1.0f);
	group2->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFF000088,0x0000FF00));
	group2->addModifier(gravity);
	group2->addModifier(friction);
	group2->addModifier(obstacle);
	group2->setRenderer(lineTrailRenderer);

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
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_ESCAPE))
				exit = true;

			// if pause is pressed, the system is paused
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_PAUSE))
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
		glTranslatef(0.0f,0.2f,-CAM_POS_Z);
		glRotatef(angleX,1.0f,0.0f,0.0f);
		glRotatef(angleY,0.0f,1.0f,0.0f);

		glColor4f(0.5f,0.0f,0.0f,1.0f);
		glBegin(GL_QUADS);
		glVertex3f(-1.0f,-0.8f,-1.0f);
		glVertex3f(-1.0f,-0.8f,1.0f);
		glVertex3f(1.0f,-0.8f,1.0f);
		glVertex3f(1.0f,-0.8f,-1.0f);
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

	delete system;

	SDL_Quit();
	
	return 0;
}