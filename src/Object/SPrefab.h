//
// Created by ounols on 19. 2. 10.
//

#pragma once
#include "../SObject.h"
#include "SGameObject.h"


class SPrefab : public SObject {
public:
    SPrefab();
    ~SPrefab();

    SGameObject* Clone(vec3 position, SGameObject* parent = nullptr);
    SGameObject* Clone(vec3 position, vec3 scale, Quaternion rotation, SGameObject* parent = nullptr);

    bool SetGameObject(SGameObject* obj);

    void Exterminate() override;

private:
    SGameObject* m_root = nullptr;
};


