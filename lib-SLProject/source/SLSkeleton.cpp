
#include <stdafx.h>
#include <SLSkeleton.h>
#include <SLScene.h>
#include <SLAnimationState.h>


SLSkeleton::SLSkeleton()
{
    SLScene::current->animManager().addSkeleton(this);
}

SLSkeleton::~SLSkeleton()
{
    delete _root;

    map<SLstring, SLAnimation*>::iterator it1;
    for (it1 = _animations.begin(); it1 != _animations.end(); it1++)
        delete it1->second;
    
    map<SLstring, SLAnimationState*>::iterator it2;
    for (it2 = _animationStates.begin(); it2 != _animationStates.end(); it2++)
        delete it2->second;
}

SLJoint* SLSkeleton::createJoint(SLuint handle)
{
    ostringstream oss;
    oss << "Bone " << handle;
    return createJoint(oss.str(), handle);
}

SLJoint* SLSkeleton::createJoint(const SLstring& name, SLuint handle)
{
    SLJoint* result = new SLJoint(name, handle, this);
    
    assert((handle >= _jointList.size() || (handle < _jointList.size() && _jointList[handle] == NULL)) && "Trying to create a bone with an already existing handle.");

    if (_jointList.size() <= handle)
        _jointList.resize(handle+1);
    
    _jointList[handle] = result;
    return result;
}


SLAnimationState* SLSkeleton::getAnimationState(const SLstring& name)
{
    if (_animationStates.find(name) != _animationStates.end())
        return _animationStates[name];

    else if (_animations.find(name) != _animations.end())
    {
        _animationStates[name] = new SLAnimationState(_animations[name]);
        return _animationStates[name];
    }

    return NULL;
}

SLJoint* SLSkeleton::getBone(SLuint handle)
{
    assert(handle < _jointList.size() && "Index out of bounds");
    return _jointList[handle];
}

SLJoint* SLSkeleton::getBone(const SLstring& name)
{
    if (!_root) return NULL;

    SLJoint* result = _root->find<SLJoint>(name);
}

void SLSkeleton::getBoneWorldMatrices(SLMat4f* boneWM)
{
    // @todo this is asking for a crash...
    for (SLint i = 0; i < _jointList.size(); i++)
    {
        boneWM[i] = _jointList[i]->updateAndGetWM() * _jointList[i]->offsetMat();
    }
}

void SLSkeleton::root(SLJoint* bone)
{
    if (_root)

    _root = bone;
}

void SLSkeleton::addAnimation(SLAnimation* anim)
{
    _animations[anim->name()] = anim;
}

void SLSkeleton::reset()
{
    for (SLint i = 0; i < _jointList.size(); i++)
        _jointList[i]->resetToInitialState();
}


void SLSkeleton::updateAnimations()
{
    SLScene* scene = SLScene::current;

    // @todo don't do this if we don't have any enabled animations
    //reset();

    map<SLstring, SLAnimationState*>::iterator it;
    for (it = _animationStates.begin(); it != _animationStates.end(); it++)
    {
        SLAnimationState* state = it->second;
        if (state->enabled())
        {reset();
            state->advanceTime(scene->elapsedTimeSec());
            state->parentAnimation()->apply(this, state->localTime(), state->weight());
        }
    }
}