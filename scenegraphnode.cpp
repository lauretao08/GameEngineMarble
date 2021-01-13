#include "scenegraphnode.h"

SceneGraphNode::SceneGraphNode(){
    this->type = objectType::UNDEF;
    this->Drawable = false;
    this->Collidable = false;
    this->Static = true;
}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent, objectType type){
    this->parent = parent;
    this->type = type;
    switch (type) {
    case objectType::UNDEF:
    default:
        this->Drawable = false;
        this->Collidable = false;
        this->Static = true;
        break;
    case objectType::SPHERE:
        this->Drawable = true;
        this->Collidable = true;
        this->Static = true;
        break;
    case objectType::CUBE:
        this->Drawable = true;
        this->Collidable = true;
        this->Static = true;
        break;
    }
}


objectType SceneGraphNode::getType() const{return type;}
bool SceneGraphNode::isDrawable() const {return Drawable;}
bool SceneGraphNode::isCollidable() const {return Collidable;}
bool SceneGraphNode::isStatic() const{return Static;}

void SceneGraphNode::setStatic(bool s)
{
    Static = s;
}


Transform SceneGraphNode::getTransform(){
    return transform;
}

SceneGraphNode *SceneGraphNode::getParent(){
    return parent;
}

QVector3D SceneGraphNode::getVelocity()
{
    return velocity;
}


void SceneGraphNode::setTransform(Transform t){
    transform=t;
}

void SceneGraphNode::addTransform(Transform t){
    transform.combineWith(t);
}
void SceneGraphNode::addTranslate(Translation t){
    transform.addTranslation(t);
}
