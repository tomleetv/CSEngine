//
// Created by ounols on 19. 6. 6.
//

#include "AssetMgr.h"
#include "../MacroDef.h"
#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include <iostream>

#ifdef __linux__

#include <sys/types.h>
#include <dirent.h>


#elif WIN32
#include <windows.h>
#endif
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <Manager/ResMgr.h>
#endif

AssetMgr::AssetMgr() {

}

AssetMgr::~AssetMgr() {
    Exterminate();
}


void AssetMgr::Exterminate() {
    for (auto asset : m_assets) {
        SAFE_DELETE(asset);
    }

#ifdef __ANDROID__
    SAFE_DELETE(m_assetManager);
#endif
}

void AssetMgr::LoadAssets(bool isPacked) {

    if (!isPacked) {
        ReadDirectory(CSE::AssetsPath());

        SetType();
    }
}

AssetMgr::AssetReference* AssetMgr::GetAsset(std::string name) const {
    for(auto asset : m_assets) {
        if(asset->name == name) return asset;
        if(asset->path == name) return asset;
        if(asset->name_full == name) return asset;
    }

    return nullptr;
}

void AssetMgr::ReadDirectory(std::string path) {
#ifdef __linux__ //======================================

    DIR* dirp = opendir(path.c_str());
    struct dirent* dp;
    while ((dp = readdir(dirp)) != nullptr) {

        std::string name = dp->d_name;

        if (dp->d_type == DT_DIR) {
            if (name == "." || name == "..") continue;

            ReadDirectory(path + name + '/');
            continue;
        }

        AssetReference* asset = new AssetReference();
        asset->path = path + name;
        asset->name_full = name;

        auto name_strs = split(name, '.');
        asset->extension = name_strs[name_strs.size() - 1];
        asset->name = name.substr(0, name.rfind('.'));

        m_assets.push_back(asset);
        std::cout << "[pkg] " << asset->name << " (" << asset->extension << ")\n";
    }
    closedir(dirp);

    return;

#endif //================================================
#ifdef WIN32 //==========================================
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
#endif //================================================
}

void AssetMgr::SetType() {

    for (auto asset : m_assets) {
        std::string type_str = asset->extension;
        make_lower(type_str);

        //차후 확장자 리스트를 제작하여 최적화
        //texture data
        if (type_str == "jpg" || type_str == "png" || type_str == "dds" || type_str == "hdr") {
            asset->type = TEX_2D;
            asset->name += ".texture";
            continue;
        }

        //cube map texture data
        if (type_str == "cbmap") {
            asset->type = TEX_CUBEMAP;
            asset->name += ".textureCubeMap";
            continue;
        }

        //material data
        if (type_str == "mat") {
            asset->type = MATERIAL;
            asset->name += ".material";
            continue;
        }

        //DAE data
        if (type_str == "dae") {
            asset->type = DAE;
            asset->name += ".dae";
            continue;
        }

        //script data
        if (type_str == "nut") {
            asset->type = SCRIPT;
            asset->name += ".script";
            continue;
        }

        //shader part data
        if (type_str == "vert" || type_str == "vs") {
            asset->type = SHADER;
            asset->name += ".vert";
            continue;
        }
        if (type_str == "frag" || type_str == "fs") {
            asset->type = SHADER;
            asset->name += ".frag";
            continue;
        }

        //shader part data
        if (type_str == "shader") {
            asset->type = SHADER_HANDLE;
            asset->name += ".shader";
            continue;
        }

        //prefab data
        if (type_str == "prefab") {
            asset->type = PREFAB;
            asset->name += ".prefab";
            continue;
        }

        //scene data
        if (type_str == "scene") {
            asset->type = SCENE;
            asset->name += ".scene";
            continue;
        }

        //ini data
        if (type_str == "ini") {
            asset->type = INI;
            asset->name += ".ini";
            continue;
        }

        //txt data
        asset->type = TXT;
        asset->name += ".text";
    }

}


#ifdef __ANDROID__

void AssetMgr::SetAssetManager(AAssetManager* obj) {
    m_assetManager = obj;
}


AAssetManager * AssetMgr::GetAssetManager() {
    return m_assetManager;
}

void AssetMgr::SetEnv(JNIEnv *obj) {
    m_env = obj;
}

JNIEnv *AssetMgr::GetEnv() {
    return m_env;
}

#endif