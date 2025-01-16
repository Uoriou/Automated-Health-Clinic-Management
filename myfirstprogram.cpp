#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "crow.h"
#include <nlohmann/json.hpp>


using namespace std;
nlohmann::json jsonData;

class Patient{
    public:
    string name;
    string medicalHistory;
    string address;
    string appointment = "";
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
    vector<string>availableTimeSlots = {"9:00","9:30","10:00","10:30","11:00","11:30","12:30"};
};

class PatientPrescription{

    public:
        Patient patientObj;
        vector<Patient> patientRecords;
        map<string, string>prescription;// maps the date they visited and the treatment they got 
        int offset; 
        string line;

        void handlePrescription(int pid, string treatment){
             
            prescription["03/12/24"] = treatment;
            //prescription["04/12/24"] = "Aspirin";
            //prescription["05/12/24"] = "Ibuprofen";
            jsonData[to_string(pid)] = prescription;
            std::ofstream file("prescription.json");
            //Saving data to prescription.json
            if (file.is_open()) {
                
                
                file << jsonData.dump(4);
                file.close();
                std::cout << "Data saved" << std::endl;
            } 
        }

        map<string,string> getMedicalRecords(int pid){ 

            std::ifstream file("prescription.json");
            if (file.is_open()) {
                file >> jsonData;  // Read data into jsonData
                file.close();
            } else {
                std::cerr << "Error opening prescription.json" << std::endl;
            }

           map<string, string> container = jsonData[to_string(pid)].get<map<string ,string>>();
           return container;
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
    
    crow::SimpleApp app;//Used to initialise CROW server
    map<int, Patient> patients;
   
    //map<int, vector<Appointment>> appointments;  
    map<int,DoctorClass>doctorsMap;  
    
    int doctorCounter = 0; 
    // register a patient 
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

        return crow::response(jsonRes);
    });
    
    
    // to book an appointment 
    // address:port/bookappointment?pid = patient id(int)& date = 12.12.2024(format of the not defined yet is just a string)&time=hour:minute 
    //http://0.0.0.0:3333/bookappontment?pid=1&date=12.12.2024&time=9:00
    CROW_ROUTE(app, "/bookappointment").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("pid") || !req.url_params.get("date") || !req.url_params.get("time")) {
            return crow::response(404, "Missing 'pid', 'date', or 'time' query parameter.");
        }

        int pid = atoi(req.url_params.get("pid"));
        string date = req.url_params.get("date");
        string time = req.url_params.get("time");

        //Loop through the doctors map and associate docId and pid, 
        //then associate pid, docId with the timeslot, removing the already-taken timeslot from  the vector  
        AppointmentClass book;
        DoctorClass doctorObj;
        Patient patientBookAppo;//booking
        crow::json::wvalue response; 
        int tempContainer;//Doctor id holder temporary
        for(const auto & doctor:doctorObj.doctorsMap){
            tempContainer = doctor.first;
            book.patientToDocMap[pid] = tempContainer;
           
        } 
        //Booking an appointment 
        try{   
            if(find(book.availableTimeSlots.begin(), book.availableTimeSlots.end(), time) != book.availableTimeSlots.end()){
                cout<<"Time slot found: "<<time;
                patientBookAppo.appointment = time;
                doctorObj.slots[tempContainer] = patientBookAppo.appointment;
                if(patientBookAppo.hasAppointment[pid] == false){
                    patientBookAppo.hasAppointment[pid] = true; 
                    cout<<"BOOKED";
                }
            }
            else{
                throw("Error finding the slot");
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

    /*
    ==========Task 2========== 
    Providing prescription to patients
    */

    CROW_ROUTE(app, "/prescribe").methods("GET"_method)([&](const crow::request& req) {
        
        int id =  atoi(req.url_params.get("pid"));
        string treatment = req.url_params.get("treatment");
        crow::json::wvalue response; 
        PatientPrescription prescribe;

        prescribe.handlePrescription(id,treatment);
        response[to_string(id)] = "Prescribed";

        return crow::response(response);

    });

    CROW_ROUTE(app, "/getMedicalRecords").methods("GET"_method)([&](const crow::request& req) {

        int id =  atoi(req.url_params.get("pid"));
        
        crow::json::wvalue response; 
        PatientPrescription prescribe;
        string getDate = "";
        string getTreatment = "";
        for(auto const& [key, value]:prescribe.getMedicalRecords(id)){
            cout<<key <<":"<<value<<::endl;
            getDate = key;
            getTreatment = value;
        }
        response[to_string(id)] = getTreatment;
        return crow::response(response);

    });

    /*
    ===========Task3==========
    Handling billing and insurance claims
    */
    


    app.port(3333).run();
    return 0;
}
