/*================================================================
*   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
*   
*   文件名称：test.cpp
*   创 建 者：wudu
*   创建日期：2022年07月22日
*   描    述：
*
================================================================*/


#include "UserManager.hpp"

using namespace std;

int main(){
    UserManager um;
    if(um.InitUserMana() == false){
        return 0;
    }

    int userid;
    int ret = um.DealRegister("zgr","bite","345", "4", &userid);
    if(ret < 0){
        cout << "regi failed" << endl;
    }

    cout << "userid is " << userid << endl;


    ret = um.DealLogin("345", "4", -1);
    if(ret < 0){
        cout << "login failed" << endl;
    }
    cout << "ret : " << ret << endl;


    um.SetFriend(ret, 0);


    return 0;
}
