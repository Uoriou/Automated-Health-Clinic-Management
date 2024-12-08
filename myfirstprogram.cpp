#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "crow.h"

using namespace std;


class Test{
    public:
    int num = 10;//Just testing 
};

class Patient{
    public:
    string name;
    string medicalHistory;
    string address;
    bool hasAppointment;
    
    //Patient(string name, string address){
        //this->name = name;
        //this->address = address;
    
    //}

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

struct Appointment {
    int pid;
    string date; // Day Month Year
    string time; // Hours: Minutes
    

    crow::json::wvalue to_json() const {
        crow::json::wvalue j;
        j["pid"] = pid;
        j["date"] = date;
        j["time"] = time;
        // j["doctor"] = doctor;
        return j;
    }
};

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
    map<int, vector<Appointment>> appointments; 
    map<int, Doctor> doctors;                  
    int patientCounter = 0;
    int doctorCounter = 0;  

    // to register patient 
    // address:port/register?name=whatever whatever&street= 12312313 anywhere
    //  patientCounter automatically increases by 1 when new patients are created
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
        //Save patients data to a JSON file 

        crow::json::wvalue jsonRes;

        patients[patientCounter] = patientObject;

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

        Doctor doctorObject{name, specialization};
        crow::json::wvalue jsonRes;

        doctors[doctorCounter] = doctorObject; // Add doctor
        jsonRes[std::to_string(doctorCounter)] = doctorObject.to_json();
        doctorCounter++; // Increment doctor ID

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

        if (patients.find(pid) == patients.end()) {
            return crow::response(404, "Patient not found");
        }

        Appointment appointment{pid, date, time};
        appointments[pid].push_back(appointment);

        crow::json::wvalue response;
        response["message"] = "Appointment booked successfully";
        response["appointment"] = appointment.to_json();
        return crow::response(response);
    });

    // address:port/getappointment/pid will get all the appointments that patient has
    CROW_ROUTE(app, "/getappointments/<int>")([&](int pid) {
        if (patients.find(pid) == patients.end()) {
            return crow::response(404, "Patient ID is incorrect or there is no such patient");
        }

        crow::json::wvalue response;
        vector<crow::json::wvalue> appointmentList;

        for (const auto& appointment : appointments[pid]) {
            appointmentList.push_back(appointment.to_json());
        }

        response["appointments"] = std::move(appointmentList);
        return crow::response(response);
    });

    // list of all doctors address:port/doctors
    CROW_ROUTE(app, "/doctors").methods("GET"_method)([&]() {
        crow::json::wvalue response;
        vector<crow::json::wvalue> doctorList;

        for (const auto& doctor : doctors) {
            doctorList.push_back(doctor.second.to_json());
        }

        response["doctors"] = std::move(doctorList);
        return crow::response(response);
    });

    // list of all patients address:port/patients
    CROW_ROUTE(app, "/patients").methods("GET"_method)([&]() {
        crow::json::wvalue response;
        vector<crow::json::wvalue> patientList;

        for (const auto& patient : patients) {
            patientList.push_back(patient.second.to_json());
        }

        response["patients"] = std::move(patientList);
        return crow::response(response);
    });

    app.port(3333).run();
    return 0;
}
