#include "Rotate.h"
#include "rs232.h"

void signalHandler(int signo)
{
	cout << "Close Port" << endl;
	RS232_CloseComport(2);
	exit(0);
}

float scale(float valueIn, float baseMin, float baseMax, float limitMin, float limitMax)
{
	return ((limitMax - limitMin) * (valueIn - baseMin) / (baseMax - baseMin)) + limitMin;
}

// the line is defined by axis direction passing through the pivot
// 3rd argument "point" is the external point
core::vector3df getClosestPointOnLine(const core::vector3df& axis, const core::vector3df& pivot, const core::vector3df& point)
{
	core::vector3df c = point - pivot;
	f32 t = axis.dotProduct(c);
	return pivot + axis*t;
}

void revolveNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis, const core::vector3df& pivot)
{
	node->updateAbsolutePosition();
	core::vector3df p1 = node->getAbsolutePosition();
	core::vector3df p2 = getClosestPointOnLine(axis, pivot, p1);
	core::vector3df vect = p1 - p2;

	core::quaternion q;
	q.fromAngleAxis(degs*core::DEGTORAD, axis);
	q.getMatrix().rotateVect(vect);

	node->setPosition(p2 + vect);
}

void rotateNodeInWorldSpace(scene::ISceneNode* node, f32 degs, const core::vector3df& axis)
{
	core::quaternion q;
	q.fromAngleAxis(degs*core::DEGTORAD, axis);
	core::matrix4 m1 = q.getMatrix();

	node->updateAbsolutePosition();
	core::matrix4 m2 = node->getAbsoluteTransformation();

	core::matrix4 m = m1*m2;
	node->setRotation(m.getRotationDegrees());

}

void rotateAround(core::vector3df &point, core::vector3df center, core::vector3df rotation)
{
	point -= center;
	core::matrix4 m;
	m.setRotationDegrees(rotation);
	m.rotateVect(point);
	point += center;
}

void setActiveCamera(scene::ICameraSceneNode* newActive, IrrlichtDevice * Device)
{
	if (0 == Device)
		return;

	scene::ICameraSceneNode * active = Device->getSceneManager()->getActiveCamera();
	active->setInputReceiverEnabled(false);

	newActive->setInputReceiverEnabled(true);
	Device->getSceneManager()->setActiveCamera(newActive);
}