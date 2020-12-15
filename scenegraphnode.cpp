#include "scenegraphnode.h"

objectType SceneGraphNode::getType() const
{
    return type;
}

SceneGraphNode::SceneGraphNode()
{
    this->type = objectType::UNDEF;
    this->Drawable = false;
    this->Collidable = false;
}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent, objectType type)
{
    this->parent = parent;
    this->type = type;
    switch (type) {
    case objectType::UNDEF:
    default:
        this->Drawable = false;
        this->Collidable = false;
        break;
    case objectType::SPHERE:
        this->Drawable = true;
        this->Collidable = true;
        break;
    case objectType::CUBE:
        this->Drawable = true;
        this->Collidable = true;
        break;
    }
}

void SceneGraphNode::setTransform(Transform t)
{
    transform=t;
}

void SceneGraphNode::addTransform(Transform t)
{
    transform.combineWith(t);
}

Transform SceneGraphNode::getTransform()
{
    return transform;
}

SceneGraphNode *SceneGraphNode::getParent()
{
    return parent;
}

bool SceneGraphNode::isDrawable() const{
    return Drawable;
}

bool SceneGraphNode::isCollidable() const{
    return Collidable;
}


