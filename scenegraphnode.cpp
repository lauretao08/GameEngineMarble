#include "scenegraphnode.h"

objectType SceneGraphNode::getType() const
{
    return type;
}

SceneGraphNode::SceneGraphNode()
{
    this->type = DEFAULT;
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

Transform SceneGraphNode::getTransform()
{
    return transform;
}

SceneGraphNode *SceneGraphNode::getParent()
{
    return parent;
}



