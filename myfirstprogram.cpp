#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "crow.h"

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

//struct Patient {

    //string name;
    //string address;
    //string illness;
    //string medicalHistory;

    //crow::json::wvalue to_json() const {
        //crow::json::wvalue j;
        //j["name"] = name;
        //j["address"] = address;

        //return j;
    //}
//};

class DoctorClass{
    public: 
    int docId;
    bool isAvailable;
    //docId is mapped to the timeslots 
    map<int,string>slots;

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
    vector<string>availableTimeSlots = {"9:00","9:30","10,00,","10:30"};
    //crow::json::wvalue to_json() const {
        //crow::json::wvalue j;
        //j["pid"] = pid;
        //j["date"] = date;
        //j["time"] = time;
        // j["doctor"] = doctor;
        //return j;
    //}
   
};

//struct Appointment {

   // int pid;
   // string date; // Day Month Year
   //string time; // Hours: Minutes
    

    //crow::json::wvalue to_json() const {
        //crow::json::wvalue j;
        //j["pid"] = pid;
        //j["date"] = date;
        //j["time"] = time;
         //j["doctor"] = doctor;
        //return j;
    //}
//};

struct Doctor {

    
    string name;
    string specialization;

    crow::json::wvalue to_json() const {
        crow::json::wvalue j;
        j["name"] = name;
        j["specialization"] = specialization;
        return j;
    }
};

int main() {
    crow::SimpleApp app;

    map<int, Patient> patients;                
    //map<int, vector<Appointment>> appointments; 
    //map<int, Doctor> doctors; 
    map<int,DoctorClass>doctorsMap;        
    int patientCounter = 0;
    int doctorCounter = 0;  

    // to register a patient 
    // address:port/register?name=whatever whatever&street= 12312313 anywhere
    //  patientCounter automatically increases by 1 when new patients are created
    //The patient can not have an appointment at the time of the registration
    CROW_ROUTE(app, "/register").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("name") || !req.url_params.get("address")) {
            return crow::response(404, "Missing 'name' or 'address' query parameter.");
        }

        string name = req.url_params.get("name");
        string address = req.url_params.get("address");

        Patient patientObject;
        patientObject.name = name;
        patientObject.address = address;

        //Patient patientObject{name, address};
        //Save patients data to a JSON file for now  

        crow::json::wvalue jsonRes;

        patients[patientCounter] = patientObject;
        patientObject.hasAppointment[patientCounter] = false;//No appointment 
        jsonRes[std::to_string(patientCounter)] = patientObject.to_json(); 
        patientCounter++;

        return crow::response(jsonRes);
    });

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
        doctorsMap[doctor.docId] = doctor; 
        jsonRes[std::to_string(doctorCounter)] = doctor.to_json();
        //doctorCounter++; // Increment doctor ID

        return crow::response(jsonRes);
    });

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
        Patient patientBookAppo;
        int tempContainer;//Doctor id holder
        for(const auto & doctor:doctorsMap){
            tempContainer = doctor.first;
            book.patientToDocMap[pid] = tempContainer;
           
        } 
        //cout<<"Doc id";
        //cout<<book.patientToDocMap[pid];
        //Booking an appointment 
        for(int i = 0; i < book.availableTimeSlots.size();i++){

            patientBookAppo.appointment = book.availableTimeSlots[0];//vector needs to be re-initialised
            doctorObj.slots[tempContainer] = patientBookAppo.appointment;
            if(patientBookAppo.hasAppointment[pid] == false){
               patientBookAppo.hasAppointment[pid] = true; 
               cout<<"The patient has an appointment \b";
               cout<<patientBookAppo.appointment;
            }
        }
        

        //Appointment appointment{pid, date, time};
        //appointments[pid].push_back(appointment);

        crow::json::wvalue response;
        //response["message"] = "Appointment booked successfully";
        //response["appointment"] = appointment.to_json();
        response["message"] = "Appointment booked successfully";
        return crow::response(response);
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
