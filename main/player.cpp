#include <iostream>
#include <vector>
#include "../src/common.h"
#include <boost/asio.hpp>
#include "../src/scene.h"
#include "../src/utility.inl"
#include "../src/language.h"


Scene mygame;

using boost::asio::ip::tcp;
void send_match_request(tcp::socket& socket){
    std::string request = "match_request\n";
    boost::asio::write(socket, boost::asio::buffer(request));
}

void send_game_completed(tcp::socket& socket) {
    std::string message = "game_completed\n";
    boost::asio::write(socket, boost::asio::buffer(message));
}




int main(){
    mygame.generate();
    std::string server_ip = "127.0.0.1";
    unsigned short server_port = 114514;

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
        mygame.SetMode(KeyMode::NORMAL);
    }

    send_msg(I18n::Instance().getKey(I18n::Key::GET_INTO_MATCH));

    c = _getch();
    if(c != 's' && c != 'S')
        return 0;
    //接收地图信息

    try{
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(server_ip), server_port);

        //连接到服务器
        //若当前没有开服务器就会连接失败，有没有什么办法能一直等待直到成功
        socket.connect(endpoint);

        send_match_request(socket);
        std::cout << "waiting for another player to match: " << std::endl;

        //接收数据
        package pack_;
        boost::asio::read(socket,boost::asio::buffer(&pack_,sizeof(package)));

        mygame.load(pack_);
        std::cout << "player name: " << pack_.player_name << std::endl;


        mygame.play();
        if(mygame.isComplete()){
            send_game_completed(socket);
        }

        //获取结果
        boost::asio::streambuf result;
        boost::asio::read_until(socket, result, '\n');

        std::istream is(&result);
        std::string msg;
        std::getline(is, msg);

        std::cout << msg << std::endl;
        }
    catch(std::exception& e){
        std::cerr << "Exception in receive_package: " << e.what() << std::endl;
    }

    return 0;
}