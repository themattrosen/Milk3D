// Register events here with any primitive data parcelled inside like this:
// REGISTER_EVENT(Name, int id, float data, void* etc)
// note no semicolon, and "Name" will have the term Event appended to it in code
REGISTER_EVENT(EngineStart)
REGISTER_EVENT(EngineStop)

// System Events
REGISTER_EVENT(SystemInit)
REGISTER_EVENT(SystemUpdate, float dt)
REGISTER_EVENT(SystemExit)
REGISTER_EVENT(SystemRender)

// GameObject Events
REGISTER_EVENT(GameObjectUpdate, float dt)
REGISTER_EVENT(GameObjectRender)
REGISTER_EVENT(GetGameObject, Milk3D::GameObjectID id, Milk3D::GameObject* setObj)
REGISTER_EVENT(CreateGameObject, Milk3D::GameObjectID parentID, Milk3D::GameObject* obj, Milk3D::GameObjectID* setID)
REGISTER_EVENT(DeleteGameObject, Milk3D::GameObjectID id)
REGISTER_EVENT(ParentGameObject, Milk3D::GameObjectID id, Milk3D::GameObjectID newParent)

// Physics Events
REGISTER_EVENT(AddToPhysics, Milk3D::GameObject* parent, Milk3D::Body* setBody)
REGISTER_EVENT(RemoveFromPhysics, Milk3D::GameObject* parent)
REGISTER_EVENT(SetGravity, Milk3D::Vec3 direction, bool mode)

// Game Events
REGISTER_EVENT(GameSave, std::string path)
REGISTER_EVENT(GameLoad, std::string path)
