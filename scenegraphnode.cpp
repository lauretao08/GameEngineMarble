#include "scenegraphnode.h"

SceneGraphNode::SceneGraphNode(){
    this->type = objectType::UNDEF;
    this->Drawable = false;
    this->Collidable = false;
    this->mobile = false;
}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent, objectType type){
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


objectType SceneGraphNode::getType() const{return type;}
bool SceneGraphNode::isDrawable() const {return Drawable;}
bool SceneGraphNode::isCollidable() const {return Collidable;}


Transform SceneGraphNode::getTransform(){
    return transform;
}

SceneGraphNode *SceneGraphNode::getParent(){
    return parent;
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
