#pragma once
#include <map>
#include <string>
#include <cstdint>

enum class Language : uint32_t
{
    ENGLISH,
    CHINESE,
    MAX, // 从0开始计数的话MAX可以用作枚举值的总数比如Language类就有两个枚举值
};

//实现国际化（英语、汉语两种语言分别对应两个键映射）
class I18n{
public:
    //语言键
    enum class Key{
        ASK_KEY_MAP,
        INPUT_ERROR,
        LOAD_ARCHIVE_FAIL,
        SAVE_ARCHIVE_FAIL,
        SAVE_ARCHIVE_SUCCEED,
        ASK_QUIT,
        ASK_SAVE,
        ASK_LOAD,
        ASK_SAVEPATH,
        ASK_LOADPATH,
        UNDOERROR,
        NOT_COMPLETED,
        CONGRATULATION,
    };

    using Dict = std::map<Key, std::string>; //映射

    static I18n &Instance(); //获取实例，保证只有一个I18n对象
    void setLanguage(Language l);
    std::string getKey(Key) const; //获取键对应内容

private:
    I18n();

    Dict *dict_;
};