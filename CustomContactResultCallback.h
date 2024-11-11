#pragma once
#include "include/btBulletCollisionCommon.h"
#include "include/btBulletDynamicsCommon.h"
#include <iostream>

class CustomContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override
    {
        // 충돌이 감지되면 콘솔에 출력
        btVector3 ptA = cp.getPositionWorldOnA();
        btVector3 ptB = cp.getPositionWorldOnB();
        std::cout << "Collision detected at: "
            << "Point A(" << ptA.getX() << ", " << ptA.getY() << ", " << ptA.getZ() << ") - "
            << "Point B(" << ptB.getX() << ", " << ptB.getY() << ", " << ptB.getZ() << ")" << std::endl;

        return 0;
    }
};