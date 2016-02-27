#include "Ball.h"

Ball::Ball(SceneManager* mSceneMgr, String name, String node_name) {
	entBall = mSceneMgr->createEntity(name, "sphere.mesh");
	ballNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(node_name);
	ballNode->setScale(.5, .5, .5);
	ballNode->attachObject(entBall);
}

void Ball::set_origin(btVector3 origin, btQuaternion rotation) {
	ballTransform.setIdentity();
	ballTransform.setRotation(rotation);
	ballTransform.setOrigin(origin);
}

void Ball::set_bounding_box(btScalar radius) {
	ballShape = new btSphereShape(radius);
}

void Ball::create_bounding_box(Simulator* simulator, btScalar mass, btVector3 inertia, btScalar restitution) {
	simulator->getCollisionShapes().push_back(ballShape);
	ballShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* ballMotionState = new btDefaultMotionState(ballTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, ballMotionState, ballShape, inertia);
	ballRB = new btRigidBody(rbInfo);
	ballRB->activate(true);
	ballRB->setRestitution(restitution);
	ballRB->setUserPointer(ballNode);

	simulator->getDynamicsWorld()->addRigidBody(ballRB);
}

void Ball::reset(SceneManager* mSceneMgr, Ball* ball, Simulator* simulator) {
    btVector3 origin = btVector3(btScalar(0), btScalar(500), btScalar(0));
    btQuaternion rotation = btQuaternion(1.0f, 1.0f, 1.0f, 0);
    ball->set_origin(origin, rotation);
    ball->set_bounding_box(50.0f);
    btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);
    btScalar restitution = .95f;
    ball->create_bounding_box(simulator, .2f, inertia, restitution);
    ball->get_rigidbody()->applyCentralForce(btVector3(btScalar(rand()%5000 - 2500), btScalar(rand()%5000 - 2500), btScalar(rand()%5000 - 2500)));
}

btRigidBody* Ball::get_rigidbody() {
	return ballRB;
}