// Register asset types here with macro
// REGISTER_ASSET(YourType)
// Then add include path to AssetTypeInclude.h
REGISTER_ASSET(TextureAsset, ".dds", ".png")
REGISTER_ASSET(LevelAsset, ".lvl", ".level")
REGISTER_ASSET(MeshAsset, ".obj", ".fbx")
REGISTER_ASSET(MaterialAsset, ".mtl")
REGISTER_ASSET(AudioAsset, ".wav")
REGISTER_ASSET(ShaderAsset, ".hlsl")
