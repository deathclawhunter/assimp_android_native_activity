#define LOG_TAG "Config"

#include "AppLog.h"

#include "Config.h"

#include <android/native_activity.h>
#include <linux/stat.h>
#include <sys/stat.h>
#include <asm/errno.h>
#include <errno.h>
#include "ogldev_util.h"

static Config *s_Config = NULL;

Config::Config(struct android_app *app) {
    ANativeActivity *NativeActivity = app->activity;
    const char *InternalPath = NativeActivity->internalDataPath;
    std::string DataPath;
    DataPath.append(InternalPath);

    std::string ConfigFile = DataPath + "/" + CFG_FILE_NAME;
    std::string TmpConfigFile = DataPath + "/" + CFG_TMP_FILE_NAME;

    struct stat sb;
    m_CachePath = strdup(ConfigFile.c_str());
    m_CacheTmpPath = strdup(TmpConfigFile.c_str());
    int32_t res = stat(DataPath.c_str(), &sb);
    if (0 == res && sb.st_mode & S_IFDIR) {
        LOGI("'files/' dir already in app's internal data storage.");
    } else if (ENOENT == errno) {
        res = mkdir(DataPath.c_str(), 0770);
    }

    if (0 == res) {
        res = stat(ConfigFile.c_str(), &sb);
        if (0 == res && sb.st_mode & S_IFREG) {
            LOGI("Application config file already present");
        } else {
            LOGI("Application config file does not exist. Creating it ...");
            LOGI("Reading config file using the asset manager.\n");

            AAssetManager *AssetManager = NativeActivity->assetManager;
            AAsset *ConfigFileAsset = AAssetManager_open(AssetManager, CFG_FILE_NAME.c_str(),
                                                         AASSET_MODE_BUFFER);
            if (ConfigFileAsset != NULL) {
                const void *ConfigData = AAsset_getBuffer(ConfigFileAsset);
                const off_t ConfigLen = AAsset_getLength(ConfigFileAsset);
                FILE *AppConfigFile = fopen(ConfigFile.c_str(), "w+");
                if (NULL == AppConfigFile) {
                    LOGE("Could not create app configuration file.\n");
                } else {
                    LOGI("App config file created successfully. Writing config data ...\n");
                    res = fwrite(ConfigData, sizeof(char), ConfigLen, AppConfigFile);
                    if (ConfigLen != res) {
                        LOGE("Error generating app configuration file.\n");
                    }
                }
                fclose(AppConfigFile);
                AAsset_close(ConfigFileAsset);
            }
        }
        LoadCfgs();
    }
}

Config::~Config() {
    // TODO: delete configure cache

    if (m_CachePath != NULL) {
        free((void *) m_CachePath);
        m_CachePath = NULL;
    }

    if (m_CacheTmpPath != NULL) {
        free((void *) m_CacheTmpPath);
        m_CacheTmpPath = NULL;
    }
}

void Config::FlushCfgs() {

    if (m_Cache.empty()) {
        return;
    }

    unlink(m_CacheTmpPath); // remove temporary file

    FILE *TmpConfig = fopen(m_CacheTmpPath, "w+");
    if (NULL == TmpConfig) {
        LOGE("Flush configure file fail");
        return;
    }

    for (auto const& ent : m_Cache) {
        fwrite(ent, sizeof(char), strlen(ent), TmpConfig);
        fwrite("\n", sizeof(char), 1, TmpConfig);
    }

    fclose(TmpConfig);

    unlink(m_CachePath);
    rename(m_CacheTmpPath, m_CachePath);
}

void Config::LoadCfgs() {
    char buf[BUFSIZ + 1];
    buf[BUFSIZ] = 0;

    FILE *f1 = fopen(m_CachePath, "rb");
    if (f1 != NULL) {

        m_Cache.resize(MAX_CONFIG_ENTRIES);
        int readStr = 0;
        do {
            memset(buf, 0, BUFSIZ);
            readStr = ReadLine(f1, buf, BUFSIZ);
            int len = strlen(buf);
            if (len > 0) {
                if (len >= 2 && buf[0] == '/' && buf[1] == '/') {
                    continue;
                }
                char *Value = (char *) malloc(len + 1);
                Value[len] = 0;
                memcpy(Value, buf, len);
                m_Cache.push_back(Value);
            }
        } while (readStr > 0);
        fclose(f1);
    }
}

Config *Config::GetInstance(struct android_app *app) {
    if (s_Config == NULL) {
        s_Config = new Config(app);
    }

    return s_Config;
}

const char *Config::GetsCfg(int Key, const char *DefaultValue) {
    if (Key < m_Cache.size()) {
        return m_Cache[Key];
    }

    return DefaultValue;
}

void Config::SetsCfg(int Key, const char *Value) {
    if (Key < m_Cache.size()) {
        m_Cache[Key] = Value;
    }
}

bool Config::GetbCfg(int Key, bool DefaultValue) {
    if (DefaultValue) {
        return strcmp(GetsCfg(Key, "true"), "true") == 0;
    }
    return strcmp(GetsCfg(Key, "false"), "true") == 0;
}


void Config::SetbCfg(int Key, bool Value) {
    if (Value) {
        SetsCfg(Key, "true");
    }
    SetsCfg(Key, "false");
}

int Config::GetiCfg(int Key, int DefaultValue) {
    char a[128];
    sprintf(a, "%d", DefaultValue);

    return atoi(GetsCfg(Key, a));
}

void Config::SetiCfg(int Key, int Value) {
    char a[128];
    sprintf(a, "%d", Value);

    SetsCfg(Key, a);
}

