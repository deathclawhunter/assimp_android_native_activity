#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>

#include "android_native_app_glue.h"

using namespace std;

class Config {
public:
    static Config *GetInstance(struct android_app *app);

    ~Config();

    enum CFGS {
        CFG_USE_SENSOR
    };

    static const bool DEFAULT_CFG_USE_SENSOR = true;

    bool GetbCfg(int Key, bool DefaultValue);
    void SetbCfg(int Key, bool Value);
    int GetiCfg(int Key, int DefaultValue);
    void SetiCfg(int Key, int Value);
    const char *GetsCfg(int Key, const char *DefaultValue);
    void SetsCfg(int Key, const char *Value);
    void FlushCfgs();

private:
    Config(struct android_app *app);
    string CFG_FILE_NAME = "engine.cfg";
    string CFG_TMP_FILE_NAME = "engine.cfg.tmp";
    vector<const char *> m_Cache;
    void LoadCfgs();
    const char *m_CachePath = NULL;
    const char *m_CacheTmpPath = NULL;
    const int MAX_CONFIG_ENTRIES = 128;
};

#endif /* _CONFIG_H_ */
