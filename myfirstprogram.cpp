#include <iostream>
#include <cstdlib>
#include "crow.h"
#include <vector>
using namespace std;

map<string,string> patientDetailsInput(){

    string name  = "";
    string address = "";
    map<string,string>patientDetails;
    cout<<"Enter your name: \n";
    getline(cin,name);
    cout<<"Enter your address: \n";
    getline(cin,address);
    patientDetails[address] = name;

   return patientDetails;
}
//class ? 
struct patient{
    string address;
    string name;

     crow::json::wvalue to_json() const {
        crow::json::wvalue j;
        j["name"] = name;
        j["address"] = address;
        return j;
    }
};

int main(){

    /*
    Once the details are registered, the unique id is set as a key, which becomes the key 
    in the second hashmapand further operations can be handled with the id
    hashmap called info is just a temp hashmap
    */
    map<string,string> info = patientDetailsInput();//temporary hashmap
   
    crow::SimpleApp app;
    vector<int> result;
   
    //Server initialization
    CROW_ROUTE(app, "/")([info](){
            
        patient patientObject;
        map<int,patient>container; // map should contain the struct as a value
        crow::json::wvalue jsonResult;

        int  uniqueId = rand()%88;//key
        for(auto i : info){
           
            patientObject.address = i.first;
            patientObject.name  =i.second;
            container[uniqueId] = patientObject;
            cout<<uniqueId<<" \n";
        }

        jsonResult[std::to_string(uniqueId)] = patientObject.to_json();

        return jsonResult;
        
    });

    app.port(3333).run();
    return 0;
}