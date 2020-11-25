#include "scenegraphnode.h"

objectType SceneGraphNode::getType() const
{
    return type;
}

SceneGraphNode::SceneGraphNode()
{
    this->type = objectType::UNDEF;
}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent, objectType type)
{
    this->parent = parent;
    this->type = type;
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



