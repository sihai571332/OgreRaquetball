#include "Plain.h"

Plain::Plain(SceneManager* mSceneMgr, Vector3 normal, Vector3 up_vector, float x, float y, float offset, String name, String node_name, String path){
	Plane plane(normal, offset);
	MeshManager::getSingleton().createPlane(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, x, y, 20, 20, true, 1, 5, 5, up_vector);	



    
    entGround = mSceneMgr->createEntity(name);
    entGround->setMaterialName(path);
    groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(node_name);
    groundNode->attachObject(entGround);

}

Plain::Plain(SceneManager* mSceneMgr, Vector3 normal, Vector3 up_vector, float x, float y, float offset, String name, String node_name, String path, String tname){
    Plane plane(normal, offset);
    MeshManager::getSingleton().createPlane(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, x, y, 20, 20, true, 1, 5, 5, up_vector);    

    
    entGround = mSceneMgr->createEntity(name);
    entGround->setMaterialName(path);
    translationNode = mSceneMgr->getSceneNode(tname);
    groundNode = mSceneMgr->getSceneNode(tname)->createChildSceneNode(node_name);
    groundNode->attachObject(entGround);

}



void Plain::set_origin(btVector3 origin){
    groundTransform.setIdentity();
    groundTransform.setOrigin(origin);
    this->set_bounding_box();
}

void Plain::set_bounding_box(){
    AxisAlignedBox boundingB = entGround->getBoundingBox();
    Vector3 size = Vector3::ZERO;
    size = boundingB.getSize();
    groundShape = new btBoxShape(btVector3(size.x, size.y, size.z));

    /********************************************************************
      Old code
      Since we aren't using a goal anymore, 
      we don't need to have the rigid body's be
      the exact size of the plane, so they can 
      overlap and close any gaps that the ball was getting out through
      ******************************************************************/

    // size = boundingB.getSize()*0.95f;
	// groundShape = new btBoxShape(btVector3(size.x*0.5f, size.y*0.5f, size.z*0.5f));
}	

void Plain::create_bounding_box(Simulator* simulator, btScalar resititution){
    simulator->getCollisionShapes()->push_back(groundShape);
    groundShape->setUserPointer(groundNode);

    /*The code commented out below was causing problems in some of our other
      classes, so I went ahead and disabled it here as well, nothing*/

    // groundObject = new btCollisionObject();
    // groundObject->setCollisionShape(groundShape);
    // groundObject->setWorldTransform(groundTransform);
    // groundObject->forceActivationState(DISABLE_DEACTIVATION);
    // simulator->getDynamicsWorld()->addCollisionObject(groundObject);
    
	btScalar groundMass(0.0);
    btVector3 localGroundInertia(0,0,0);
    groundMotionState = new OgreMotionState(groundTransform, groundNode);

    groundShape->calculateLocalInertia(groundMass, localGroundInertia);
    btRigidBody::btRigidBodyConstructionInfo groundRBInfo(groundMass, groundMotionState, groundShape, localGroundInertia);
    groundBody = new btRigidBody(groundRBInfo);
    groundBody->setRestitution(resititution);
    groundBody->setFriction(0.0);
    groundBody->setCollisionFlags(groundBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    groundBody->setActivationState(DISABLE_DEACTIVATION);
    simulator->getDynamicsWorld()->addRigidBody(groundBody);
    groundBody->setUserPointer(groundNode);
}

void Plain::updateTransform() {
    Ogre::Vector3 pos = translationNode->_getDerivedPosition();
    groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    Ogre::Quaternion qt = groundNode->getOrientation();
    groundTransform.setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));
    if(groundMotionState)
        groundMotionState->updateTransform(groundTransform);
}

Plain::~Plain(){}