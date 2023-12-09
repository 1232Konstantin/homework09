#include <iostream>
#include "async.h"

using namespace std;

char _eof=EOF;

void receive(async::handle_t handle, std::string str)
{
    async::receive(handle, str.data(), str.length());
}


//Тест коннект для контекста 3
void test_1_3()
{
    std::list<std::string> list {"cmd1", "cmd2", "cmd3", "cmd4", "cmd5"};
    auto handler3=async::connect(3);
    //cout<<"h3:"<<handler3<<endl;
    for(auto x: list) receive(handler3, x);
     //cout<<"LOG_MUST: bulk: cmd1, cmd2, cmd3"<<endl;
     async::receive(handler3,&_eof,1);
     //cout<<"LOG_MUST: bulk: cmd4, cmd5"<<endl;

}

//Тест  коннект для контекста 4
void test_1_4()
{
    std::list<std::string> list {"aaa1", "aaa2", "aaa3", "aaa4", "aaa5"};
    auto handler4=async::connect(4);
    //cout<<"h4:"<<handler4<<endl;
    for(auto x: list) receive(handler4, x);
    //cout<<"LOG_MUST: bulk: aaa1, aaa2, aaa3, aaa4"<<endl;
    async::receive(handler4,&_eof,1);
    //cout<<"LOG_MUST: bulk: aaa5"<<endl;

}

//Тест перехода во второе состояние и выхода из него (для контекста 3)
void test_2_3()
{
    auto handler3=async::connect(3);
    std::list<std::string> list {"bbb4", "bbb5", "{","{"};
    for(auto x: list) receive(handler3, x);
    //cout<<"LOG_MUST: bulk: bbb4, bbb5" <<endl;

    std::list<std::string> list2 {"bbb6", "bbb7", "}","}"};
    for(auto x: list2) receive(handler3, x);
    //cout<<"LOG_MUST: bulk: bbb6, bbb7"<<endl;

    std::list<std::string> list3 {"{", "eee6", "eee7"};
    for(auto x: list3) receive(handler3, x);
    async::receive(handler3,&_eof,1);
    //cout<<"LOG_MUST: none"<<endl;

}

//Тест перехода в третье состояние и выхода из него (для контекста 3)
void test_3_3()
{
    std::list<std::string> list1 {"{", "ddd6", "ddd7", "{", "ddd8", "ddd9", "{","ddd10", "ddd11", "}"};
    auto handler3=async::connect(3);
    for(auto x: list1) receive(handler3, x);
    //cout<<"LOG_MUST: none"<<endl;

    std::list<std::string> list2 {"ddd12", "}", "}"};
    for(auto x: list2) receive(handler3, x);
    //cout<<"LOG_MUST: bulk: ddd6, ddd7, ddd8, ddd9, ddd10, ddd11, ddd12" <<endl;

}

//повторный коннект (контекст 3)
void test_5_3()
{
    std::list<std::string> list {"yyy1", "yyy2", "yyy3", "yyy4", "yyy5"};
    auto handler3=async::connect(3);
    //cout<<"h3:"<<handler3<<endl;
    for(auto x: list) receive(handler3, x);
     //cout<<"LOG_MUST: bulk: yyy1, yyy2, yyy3"<<endl;
     async::receive(handler3,&_eof,1);
     //cout<<"LOG_MUST: bulk: yyy4, yyy5"<<endl;

}

//повторный коннект (контекст 4)
void test_2_4()
{
    std::list<std::string> list {"fff1", "fff2", "fff3", "fff4", "fff5"};
    auto handler4=async::connect(4);
    //cout<<"h3:"<<handler3<<endl;
    for(auto x: list) receive(handler4, x);
     //cout<<"LOG_MUST: bulk: fff1, fff2, fff3, fff4"<<endl;
     async::receive(handler4,&_eof,1);
     //cout<<"LOG_MUST: bulk:  fff5"<<endl;

}

//Тест disconnect (для контекста 3)
void test_4_3()
{
    auto handler3=async::connect(3);
    async::disconnect(handler3);
    std::list<std::string> list1 {"{", "ggg1", "ggg7", "{", "ggg8", "ggg9", "{","ggg10", "ggg11", "}", "}", "}"};
    for(auto x: list1) receive(handler3, x);
    //cout<<"LOG_MUST: none"<<endl;

}


int main()
{
    //cout<<"\n >>>>test_1_3............... \n";
    test_1_3();
    //cout<<"\n >>>>test_5_3............... \n";
    test_5_3();

    //cout<<"\n >>>>test_1_4............... \n";
    test_1_4();
    //cout<<"\n >>>>test_2_3()............. \n";
    test_2_3();
    //cout<<"\n >>>>test_3_3()............. \n";
    test_3_3();
    //cout<<"\n >>>>test_2_4()............. \n";
    test_2_4();
    std::chrono::milliseconds timespan(1000); 
    std::this_thread::sleep_for(timespan);
    //cout<<"\n >>>>test_3_4()............. \n";
    test_4_3();
    //cout<<"\n >>>>test_5_3............... \n";
    test_5_3();

    int i;
    cin>>i;

//    int long long i=0;
//    while (i<20000000000000) {i++;}

    return 0;
}
