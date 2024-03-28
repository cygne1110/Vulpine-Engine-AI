#pragma once

#define MAX_COMP    64
#define MAX_ENTITY  512

#include <Entity.hpp>
#include <ObjectGroup.hpp>
#include <NavGraph.hpp>

struct EntityModel : public ObjectGroupRef{};

COMPONENT(EntityModel, GRAPHIC, MAX_ENTITY);

template<>
void Component<EntityModel>::ComponentElem::init();

template<>
void Component<EntityModel>::ComponentElem::clean();

struct EntityPosition3D {
    vec3 position;
    float speed;
    vec3 direction;
};

COMPONENT(EntityPosition3D, AI, MAX_ENTITY);

template<>
void Component<EntityPosition3D>::ComponentElem::init();

template<>
void Component<EntityPosition3D>::ComponentElem::clean();

struct EntityDestination3D {
    vec3 destination;
    bool hasDestination = false;
};

COMPONENT(EntityDestination3D, AI, MAX_ENTITY);

template<>
void Component<EntityDestination3D>::ComponentElem::init();

template<>
void Component<EntityDestination3D>::ComponentElem::clean();

struct EntityPathfinding {
    Path path;
};

COMPONENT(EntityPathfinding, AI, MAX_ENTITY);

template<>
void Component<EntityPathfinding>::ComponentElem::init();

template<>
void Component<EntityPathfinding>::ComponentElem::clean();