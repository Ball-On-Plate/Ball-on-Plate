#ifndef ROTATE_H
#define ROTATE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <irrlicht.h>
#include <irrString.h>
#include "driverChoice.h"


using namespace std;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

void signalHandler(int signo);

float scale(float valueIn, float baseMin, float baseMax, float limitMin, float limitMax);

// the line is defined by axis direction passing through the pivot
// 3rd argument "point" is the external point
core::vector3df getClosestPointOnLine(const core::vector3df& axis, const core::vector3df& pivot, const core::vector3df& point);

//both axis and pivot are in world space
void revolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot);

void rotateNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis);

void rotateAround(core::vector3df &point, core::vector3df center, core::vector3df rotation);

void setActiveCamera(scene::ICameraSceneNode* newActive, IrrlichtDevice * Device);

#endif
