#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "crow.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

using namespace std;

class Patient{
    public:
    string name;
    string medicalHistory;
    string address;
    string appointment = ""; //Does this work for c++ ?
    map<int,bool>hasAppointment;
   
    crow::json::wvalue to_json() const {
        crow::json::wvalue j;
        j["name"] = name;
        j["address"] = address;

        return j;
    }

};


class DoctorClass{
    public: 
    int docId;
    bool isAvailable;
    //docId is mapped to the timeslots 
    map<int,string>slots;
    map<int,DoctorClass>doctorsMap; 

    crow::json::wvalue to_json() const {
        crow::json::wvalue j;
        j["ID"] = docId;
        return j;
    } 
};

class AppointmentClass{

    //To book an appointment, map pid and docId
    //In the doctorclass, the following timeslot is mapped to docID 
    public:
    map<int,int>patientToDocMap;
    vector<string>availableTimeSlots = {"9:00","9:30","10,00,","10:30,","11:00","11:30","12:30"};
};


class PatientPrescription{

    public:
    Patient patientObj;
    vector<Patient> patientRecords;
    map<int, string>prescription; //Pid and the names of the medicines

    void handlePrescription(int pid, string treatment,map<int, string>prescription){

        prescription[pid]  = treatment;
    }

    int getMedicalHistory(map<int, string>prescription){ //map<int, string>
        int id = 0;
        string nameOfTheTreatment = "";
        for(map<int, string>::iterator it = prescription.begin(); it != prescription.end(); ++it){
            id = it->first;
            nameOfTheTreatment = it->second;
            cout << it->first << ": " << it->second << "\n";
        }

        return 0;
    }

};


void initialiseDoctors(crow::SimpleApp app){

    int doctorCounter = 0; 
   // to register a doctor 
    // address:port/registerdoctor?name=ayya&specialization=smth same stuff with doctorcounter
    CROW_ROUTE(app, "/registerdoctor").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("name") || !req.url_params.get("specialization")) {
            return crow::response(404, "Missing 'name' or 'specialization' query parameter.");
        }

        string name = req.url_params.get("name");
        string specialization = req.url_params.get("specialization");
        //Doctor doctorObject{name, specialization};//struct

        DoctorClass doctor;
        doctor.docId = doctorCounter++;
        doctor.isAvailable = true;//Initially they are free
        crow::json::wvalue jsonRes;
        // Add doctor to the map and save this to a JSON file
        doctor.doctorsMap[doctor.docId] = doctor; 
        jsonRes[std::to_string(doctorCounter)] = doctor.to_json();
        //doctorCounter++; // Increment doctor ID

        return crow::response(jsonRes);
    }); 
}

int main() {
    
    crow::SimpleApp app;
    map<int, Patient> patients;
    Patient patientBookAppo;//booking               
    //map<int, vector<Appointment>> appointments;  
    map<int,DoctorClass>doctorsMap;  
    PatientPrescription test;
    int doctorCounter = 0; 
    //Fix this code for the task2 -- > 
    int io = 0;
    string name = "bejfnbje";
    test.handlePrescription(io,name,test.prescription);
    cout<<test.getMedicalHistory(test.prescription);

    //Declare a JSON file here 
   
    //jsonData["test"] = 5;
    std::string fileName = "data.json";
    

    //Can this be in a class ?
    // to register a patient 
    // address:port/register?name=whatever whatever&street= 12312313 anywhere
    //  patientCounter automatically increases by 1 when new patients are created
    //The patient can not have an appointment at the time of the registration
    CROW_ROUTE(app, "/register").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("name") || !req.url_params.get("address")) {
            return crow::response(404, "Missing 'name' or 'address' query parameter.");
        }

        int patientCounter = 0;
        string name = req.url_params.get("name");
        string address = req.url_params.get("address");
        Patient patientObject;
        patientObject.name = name;
        patientObject.address = address;
        //Save patients data to a JSON file for now  

        crow::json::wvalue jsonRes;

        patients[patientCounter] = patientObject;
        patientObject.hasAppointment[patientCounter] = false;//No appointment 
        jsonRes[std::to_string(patientCounter)] = patientObject.to_json(); 
        patientCounter++;
        //std::ofstream file(fileName);
        // Write JSON data to a file
        

        return crow::response(jsonRes);
    });
    std::ofstream file("data.json", std::ios::app); // Open in append mod
    json jsonArray = json::array({{"happy", true},{"pi", 3.141}});

    if (file.is_open()) {     
        file << jsonArray.dump(4); // Pretty print with 4 spaces indentation
        file.close();
        cout << "Data saved successfully to " << fileName << std::endl;
    } else {
        cerr << "Failed to open the file." << std::endl;
    }
    //Should this be a method inside Appointment class ? and should be called inside crow ? 
    // to book an appointment 
    // address:port/bookappoinment?pid = patient id(int)& date = 12.12.2024(format of the not defined yet is just a string)&time=hour:minute 
    CROW_ROUTE(app, "/bookappointment").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("pid") || !req.url_params.get("date") || !req.url_params.get("time")) {
            return crow::response(404, "Missing 'pid', 'date', or 'time' query parameter.");
        }

        int pid = atoi(req.url_params.get("pid"));
        string date = req.url_params.get("date");
        string time = req.url_params.get("time");

        //if (patients.find(pid) == patients.end()) {
            //return crow::response(404, "Patient not found");
        //}

        //Loop through the doctors map and associate docId and pid, 
        //then associate pid, docId with the timeslot, removing the already-taken timeslot from  the vector  
        AppointmentClass book;
        DoctorClass doctorObj;
        crow::json::wvalue response; 
        int tempContainer;//Doctor id holder temporary
        for(const auto & doctor:doctorObj.doctorsMap){
            tempContainer = doctor.first;
            book.patientToDocMap[pid] = tempContainer;
           
        } 
        //Booking an appointment 
        try{
            for(int i = 0; i < book.availableTimeSlots.size();i++){
            
                if(time == book.availableTimeSlots[i]){
                    patientBookAppo.appointment = book.availableTimeSlots[i];
                    doctorObj.slots[tempContainer] = patientBookAppo.appointment;
                    if(patientBookAppo.hasAppointment[pid] == false){
                        patientBookAppo.hasAppointment[pid] = true; 
                        //When a patient has an appointment already
                        cout<<"YEAHHHHHH BOOKED";
                    }else if(patientBookAppo.hasAppointment[pid] == true){
                       response["message"] = "You have already booked an appointment"; 
                    }
                }else{
                    throw("?");
                }
            }
        }
        catch(...){
            response["message"] = "Sorry, please try again";
        }
        
        if(patientBookAppo.hasAppointment[pid]){
            response["message"] = "Appointment booked successfully";
            return crow::response(response);
        }else{
           response["message"] = "The selected time is unavailable";
           return crow::response(response); 
        }
        
    });

    // address:port/getappointment/pid will get all the appointments that patient has
    //CROW_ROUTE(app, "/getappointments/<int>")([&](int pid) {
       // if (patients.find(pid) == patients.end()) {
            //return crow::response(404, "Patient ID is incorrect or there is no such patient");
        //}

        //crow::json::wvalue response;
        //vector<crow::json::wvalue> appointmentList;

        //for (const auto& appointment : appointments[pid]) {
            //appointmentList.push_back(appointment.to_json());
        //}

        //response["appointments"] = std::move(appointmentList);
        //return crow::response(response);
    //});
    app.port(3333).run();
    return 0;
}
