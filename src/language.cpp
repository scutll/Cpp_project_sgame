#include <cassert>
#include "language.h"

I18n::Dict chinese = {
    {I18n::Key::ASK_KEY_MAP,"选择按键模式,默认1->WASD"},
    {I18n::Key::INPUT_ERROR,"输入错误！"},
    {I18n::Key::LOAD_ARCHIVE_FAIL,"导入存档错误"},
    {I18n::Key::ASK_QUIT,"要退出吗 [Y/N]"},
    {I18n::Key::ASK_SAVE,"是否保存存档 [Y/N]"},
    {I18n::Key::ASK_SAVEPATH,"保存路径: "},
    {I18n::Key::UNDOERROR,"无法撤回"},
    {I18n::Key::NOT_COMPLETED,"还未完成"},
    {I18n::Key::CONGRATULATION,"祝贺你! 成功了"},

};

I18n::Dict english = {
    {I18n::Key::ASK_KEY_MAP,"Choose key mode, default->WASD"},
    {I18n::Key::INPUT_ERROR,"Input error!"},
    {I18n::Key::LOAD_ARCHIVE_FAIL,"Failed to load archive"},
    {I18n::Key::ASK_QUIT,"Do you want to quit [Y/N]"},
    {I18n::Key::ASK_SAVE,"Do you want to save the archive [Y/N]"},
    {I18n::Key::ASK_SAVEPATH,"Save path: "},
    {I18n::Key::UNDOERROR,"Cannot undo"},
    {I18n::Key::NOT_COMPLETED,"Not completed yet"},
    {I18n::Key::CONGRATULATION,"Congratulations! You made it"},
};

I18n::I18n() : dict_(&english){}

I18n& I18n::Instance(){
    static I18n i18n;
    return i18n;
}

void I18n::setLanguage(Language language){
    switch(language){
        case Language::ENGLISH:
            dict_ = &english;
            break;
        case Language::CHINESE:
            dict_ = &chinese;
            break;
        case Language::MAX:
        default:
            assert(false);
    }
}

std::string I18n::getKey(Key key) const {
    assert(dict_->count(key));
    return (*dict_)[key];
}

