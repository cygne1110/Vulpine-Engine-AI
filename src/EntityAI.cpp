#include <EntityAI.hpp>
#include <Globals.hpp>

template<>
void Component<EntityModel>::ComponentElem::init()
{
    // std::cout << "creating entity model " << entity->toStr();
    globals.getScene()->add(data);
};

template<>
void Component<EntityModel>::ComponentElem::clean()
{
    // std::cout << "deleting entity model " << entity->toStr();

    if(data.get())
        globals.getScene()->remove(data);
    else
        WARNING_MESSAGE("Trying to clean null component from entity " << entity->ids[ENTITY_LIST] << " named " << entity->comp<EntityInfos>().name)
};

template<>
void Component<EntityPosition3D>::ComponentElem::init()
{
    // std::cout << "creating entity position " << entity->toStr();
}

template<>
void Component<EntityPosition3D>::ComponentElem::clean()
{
    // std::cout << "deleting entity position " << entity->toStr();
}

template<>
void Component<EntityDestination3D>::ComponentElem::init()
{
    // std::cout << "creating entity destination " << entity->toStr();
}

template<>
void Component<EntityDestination3D>::ComponentElem::clean()
{
    // std::cout << "deleting entity destination " << entity->toStr();
}

template<>
void Component<EntityPathfinding>::ComponentElem::init()
{
    // std::cout << "creating entity pathfinding " << entity->toStr();

    entity->comp<EntityPathfinding>().path.update(entity->comp<EntityPathfinding>().graph);
}

template<>
void Component<EntityPathfinding>::ComponentElem::clean()
{
    // std::cout << "deleting entity pathfinding " << entity->toStr();
}