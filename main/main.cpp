#include "scene.h"
#include <iostream>
#include "common.h"
#include "utility.inl"
#include "language.h"


int main(){
    Scene game;
    char c = '\0';

    std::cout << "1 English, 2 简体中文" << std::endl;
    c = _getch();
    if(c == '1'){
        I18n::Instance().setLanguage(Language::ENGLISH);
    }
    else
        I18n::Instance().setLanguage(Language::CHINESE);

    send_msg(I18n::Instance().getKey(I18n::Key::ASK_KEY_MAP));
    c = _getch();
    if(c == '1'){
        game.SetMode(KeyMode::NORMAL);
    }

    game.generate();

    //加载存档
    send_msg(I18n::Instance().getKey(I18n::Key::ASK_LOAD));
    char c;
    c = getchar();

    if(c == 'y' || c == 'Y')
        game.load();

    std::cin.get();



    game.play();
}