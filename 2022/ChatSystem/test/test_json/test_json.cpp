/*================================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：test_json.cpp
 *   创 建 者：wudu
 *   创建日期：2022年07月22日
 *   描    述：
 *
 ================================================================*/

#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <jsoncpp/json/json.h>


using namespace std;

#if 0
int main(){
    Json::Value js;
    js["nickname"] = "wudu";
    js["school"] = "bite";
    js["telnum"] = "123";
    js["passwd"] = "1";

    cout << js << endl;

    Json::Value js1;
    js1["nickname"] = "gaobo";
    js1["school"] = "bite";
    js1["telnum"] = "234";
    js1["passwd"] = "2";

    cout << js1 << endl;

    Json::Value arr_js;
    arr_js.append(js);
    arr_js.append(js1);

    cout << arr_js << endl;

    cout << "1111111111111111111111111111111111111111111111111111111111111111111111" << endl;

    for(int i = 0; i < (int)arr_js.size(); i++){
        cout << arr_js[i]["nickname"].asString() << endl;
    }




    return 0;
}
#endif

static bool Serialize(const Json::Value& value, std::string* body) {
    Json::StreamWriterBuilder swb;
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    std::stringstream ss;
    int ret = sw->write(value, &ss);
    if (ret != 0) {
        return false;
    }
    *body = ss.str();
    return true;
}

static bool UnSerialize(const std::string& body, Json::Value* value) {
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

    std::string err;
    bool ret = cr->parse(body.c_str(), body.c_str() + body.size(), value, &err);
    if (ret == false) {                                                                                                                                                                  
        return false;
    }
    return true;
}


int main(){
    Json::Value js;
    js["nickname"] = "wudu";
    js["school"] = "bite";
    js["telnum"] = "123";
    js["passwd"] = "1";

    //kcout << js << endl;

    Json::Value js1;
    js1["nickname"] = "gaobo";
    js1["school"] = "bite";
    js1["telnum"] = "234";
    js1["passwd"] = "2";

    //cout << js1 << endl;

    Json::Value arr_js;
    arr_js.append(js);
    arr_js.append(js1);

    //cout << arr_js << endl;


    //序列化和反序列化的过程
#if 0
    Json::FastWriter w;
    std::string msg = w.write(arr_js);
    cout << msg << endl;
#endif

#if 1
    string msg;
    Serialize(arr_js, &msg);
    cout << msg << endl;
#endif

#if 0
    Json::Reader r;
    Json::Value v_tmp;
    r.parse(msg, v_tmp);

    cout << v_tmp[0] << endl;
    cout << "------------------------------------- "<< endl;
    cout << v_tmp[1] << endl;
#endif

    Json::Value v_tmp;
    UnSerialize(msg, &v_tmp);
    cout << v_tmp[0] << endl;
    cout << "------------------------------------- "<< endl;
    cout << v_tmp[1] << endl;
    return 0;
}
