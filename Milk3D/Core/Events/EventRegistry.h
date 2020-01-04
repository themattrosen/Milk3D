// Register events here with any primitive data parcelled inside like this:
// REGISTER_EVENT(Name, int id, float data, void* etc)
// note no semicolon, and "Name" will have the term Event appended to it in code
REGISTER_EVENT(EngineStart)
REGISTER_EVENT(EngineStop)

REGISTER_EVENT(SystemInit)
REGISTER_EVENT(SystemUpdate, float dt)
REGISTER_EVENT(SystemExit)
REGISTER_EVENT(SystemRender)

