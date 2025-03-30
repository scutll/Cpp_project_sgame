#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include "../src/common.h"
#include "../src/save_package.h"
#include "../src/scene.h"


//包的序列化，将package转化成字节形式
std::vector<char> serialize_package(const package& pack_){
    std::vector<char> buffer(sizeof(package));
    std::memcpy(buffer.data(), &pack_, sizeof(package));
    return buffer;
}

//反序列化
package deserialze_package(const std::vector<char>& buffer){
    package pack_;
    std::memcpy(&pack_, buffer.data(), sizeof(package));
    return pack_;
}


using boost::asio::ip::tcp;

std::queue<tcp::socket *> match_queue;
std::mutex queue_mutex;

void handle_battle(tcp::socket *player1, tcp::socket *player2);

void handle_client(tcp::socket* socket){
    std::cout << "1 player connected to the server! " << std::endl;

    try{

        //处理匹配请求
        boost::asio::streambuf request;
        boost::asio::read_until(*socket,request,'\n');

        //request相当于文件一样读写
        std::istream is(&request);
        std::string message;
        std::getline(is, message);  //写入到message

        if(message == "match_request"){
            std::lock_guard<std::mutex> lock(queue_mutex);
            //锁定使得只有当前线程可以访问match_queue
            match_queue.push(socket);
        }

        if(match_queue.size() >= 2){
            //匹配玩家
            std::lock_guard<std::mutex> lock(queue_mutex);
            tcp::socket *player1 = match_queue.front();
            match_queue.pop();
            tcp::socket *player2 = match_queue.front();
            match_queue.pop();

            //生成地图
            Scene game;
            game.generate();

            package _pack = game.package_();
            //序列化地图
            std::vector<char> buffer = serialize_package(_pack);
            //发送地图到玩家端
            boost::asio::write(*player1, boost::asio::buffer(&_pack,sizeof(package)));
            boost::asio::write(*player2, boost::asio::buffer(&_pack,sizeof(package)));

            //加入线程开始对战
            std::thread battle_therad(handle_battle,player1,player2);
            //放到后台，不影响其他玩家匹配
            battle_therad.detach();
        }
    }
    catch(std::exception& e){
        std::cerr << "Exception in handle_client: " << e.what() << std::endl;
        delete socket;
    }
}

void handle_battle(tcp::socket* player1,tcp::socket* player2){
    try{
        //处理玩家信息
        boost::asio::streambuf response1, response2;
        boost::asio::read_until(*player1, response1, '\n');
        boost::asio::read_until(*player2, response2, '\n');

        //接收player消息
        std::istream is1(&response1), is2(&response2);
        std::string msg1, msg2;
        std::getline(is1, msg1);
        std::getline(is2, msg2);

        if(msg1 == "game_completed" && !msg2.size()){
            boost::asio::write(*player1, boost::asio::buffer("you win!\n"));
            boost::asio::write(*player2, boost::asio::buffer("you lose\n"));
        }
        else if(msg2 == "game_completed" && !msg1.size()){
            boost::asio::write(*player2, boost::asio::buffer("you win!\n"));
            boost::asio::write(*player1, boost::asio::buffer("you lose\n"));
        }
        else{
            boost::asio::write(*player1, boost::asio::buffer("you draw the game\n"));
            boost::asio::write(*player2, boost::asio::buffer("you draw the game\n"));
        }

        delete player1;
        delete player2;
    }catch(std::exception& e){
        std::cerr << "Exception in handle_battle: " << e.what() << std::endl;
        delete player1;
        delete player2;
    }
}


int main(){
    try{
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 114514));

        while (true)
        {
            tcp::socket *socket = new tcp::socket(io_context);
            acceptor.accept(*socket);

            std::thread client_thread(handle_client, socket);
            client_thread.detach();
        }
        
    }
    catch(std::exception& e){
        std::cerr << "Exception in main: " << e.what() << std::endl;
    }


    return 0;
}

