#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "crow.h"
#include <nlohmann/json.hpp>
#include <sqlite3.h>


//Mix of database and json file 
using namespace std;
nlohmann::json jsonData;
nlohmann::json j;

class Patient{
    public:
    string name;
    string medicalHistory;
    string insuranceCoverage;
    string address;
    string appointment = "";
    string visitPurpose = "";
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
            jsonData[std::to_string(pid)] = prescription;
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

           map<string, string> container = jsonData[std::to_string(pid)].get<map<string ,string>>();
           return container;
        }
};


class Bills{
    
    Patient patientObj;//to get a pid and insurance coverage status
    PatientPrescription prescriptionDetails;//if a patient receives medication, they need to pay the amount

    public:
        //Bills the insurance company of pid depending on the status
        string bills(int id,string option){

            if(option == "Doctor Visit"){
                //Update the databasae for this 
                return  "No payment / Billing the insurance company";
            }
            if (option == "NULL"){
                return "NULL";
            }else{
                return "Pay the amount";
            }

        }
};

class Insurance{
    //Insurance claim logic also comes here 
    //

};


class Inventory {
private:
    map<string, int> supplies;
    map<string, int> threshold;
    string inventoryFile = "inventory.json";

    void addtoinventory() {
        ifstream file(inventoryFile);
        if (file.is_open()) {
            //json j;
            file >> j;
            supplies = j["supplies"].get<map<string, int>>();
            threshold = j["threshold"].get<map<string, int>>();
            file.close();
        }
    }

    void saveinventory() {
        ofstream file(inventoryFile);
        if (file.is_open()) {
            //json j;
            j["supplies"] = supplies;
            j["threshold"] = threshold;
            file << j.dump(4); 
            file.close();
        }
    }

    void resupply(const string &item) {
        if (threshold.find(item) != threshold.end()) {
            supplies[item] = threshold[item];
            saveinventory();
        }
    }

public:
    Inventory() {
        addtoinventory();
    }

    void addsupply(const string &item, int quantity) {
        supplies[item] += quantity;
        saveinventory();
    }

    void setthreshold(const string &item,  int thresholds) {
        threshold[item] = thresholds;
        saveinventory();
    }

    int checkSupply(const string &item) const {
        auto it = supplies.find(item);
        if (it != supplies.end()) {
            return it->second;
        }
        return -1;
    }

    bool removeSupply(const string &item, int quantity) {
        auto it = supplies.find(item);
        if (it != supplies.end() && it->second >= quantity) {
            supplies[item] -= quantity;
            saveinventory();

            if (supplies[item] == 0) {
                resupply(item);
            }

            return true;
        }
        return false;
    }

    crow::json::wvalue getInventoryJSON() const {
        crow::json::wvalue json;
        for (const auto &entry : supplies) {
            json[entry.first] = entry.second;
        }
        return json;
    }

    crow::json::wvalue checkthreshold() const {
        crow::json::wvalue response;
        bool lowSupplyExists = false;

        for (const auto &entry : supplies) {
            auto thresholdIt = threshold.find(entry.first);
            if (thresholdIt != threshold.end()) {
                int threshold = thresholdIt->second;
                if (entry.second < 0.2 * threshold) {
                    response["low_supplies"] = {{"item", entry.first}, {"current_quantity", entry.second}, {"threshold", threshold}};
                    lowSupplyExists = true;
                }
            }
        }

        if (!lowSupplyExists) {
            response["message"] = "stock is full";
        } else {
            response["status"] = "warning";
            response["message"] = "Some items are running low.";
        }

        return response;
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


static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

//Used only single time for creating patient database
int createTable(){

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
        
    }

    /*const char* dropTable = "DROP TABLE IF EXISTS patient;";

    // Execute the SQL command
    rc = sqlite3_exec(db, dropTable, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Table dropped successfully" << std::endl;
    }*/
    

    //Patient table
    const char* sql = "CREATE TABLE IF NOT EXISTS patient ("
                  "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                  "NAME TEXT NOT NULL, "
                  "ADDRESS TEXT, "
                  "INSURANCE TEXT, "
                  "PURPOSE TEXT, "
                  "APPOINTMENT TEXT);";
    

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);//Executing the sql command
   
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table Ok\n");
    }

    return 0;
}

//Initial   tration  of a patient 
int registerPatient(string name,string address,string insurance,string purpose,string appo){//Parameter id, name and address at least for a basic information 

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
        
    }

    string query = "INSERT INTO patient (NAME, ADDRESS, INSURANCE, PURPOSE, APPOINTMENT) VALUES ('" +name + "', " +
               (address.empty() ? "NULL" : "'" + address + "'") + ", " +
               (insurance.empty() ? "NULL" : "'" + insurance + "'") + ", " +
               (purpose.empty() ? "NULL" : "'" + purpose + "'") + ", " +
               (appo.empty() ? "NULL" : "'" + appo + "'") + ");";


    const char* insertSql = query.c_str(); // Convert to const char*

    /* Execute SQL statement for inserting the records*/
    rc = sqlite3_exec(db, insertSql, callback, 0, &zErrMsg);
   
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error inserting records: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return 404;
    } else {
      fprintf(stdout, "Records inserted successfully\n");
      return 200;
    }
    sqlite3_close(db);
}

string retrievePatientTreatment(){
return "";
    
}

//Retrieve patients records 
string retrieveData(int pid,string option){// for example... option == PURPOSE


    sqlite3 *db;
    sqlite3_stmt* stmt;
    char *zErrMsg = 0;
    int rc;

    crow::json::wvalue response; 

    /* Open database */
   rc = sqlite3_open("test.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully / retrieve \n");
   }

    /* Create SQL statement for retrieving data*/
   const char* data = "Callback function called";
   string sql = "SELECT * from patient WHERE ID = " +std::to_string(pid)+";";

   /* Execute SQL statement */
   sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error preparing statement for retrieving: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      sqlite3_close(db); 
    }else{
        cout<<"Good SQL"<<"\n";
    }

   
    string name = "";
    string address = "";
    string insurance = "";
    string purpose = "";
    string appointment = "";

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int id = sqlite3_column_int(stmt, 0);
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        insurance = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        purpose = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        appointment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while iterating rows: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if(option == "PURPOSE"){
        return purpose;
    }
     if(option == "INSURANCE"){
        return insurance;
    }
     if(option == "APPOINTMENT"){
        return appointment;
    }
     if(option == "ADDRESS"){
        return address;
    }

    return "";
}

int main() {
    
    crow::SimpleApp app;//Used to initialise CROW server
    map<int, Patient> patients;
   
    //map<int, vector<Appointment>> appointments;  
    map<int,DoctorClass>doctorsMap;  
    
    //int doctorCounter = 0; 

    createTable();
    
    //To register a patient 
    // address:port/register?name=whatever whatever&street= 12312313 anywhere
    //The patient can not have an appointment at the time of the registration
    CROW_ROUTE(app, "/register").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("name") || !req.url_params.get("address")) {
            return crow::response(404, "Missing 'name' or 'address' query parameter.");
        }

        string name = req.url_params.get("name");
        string address = req.url_params.get("address");
        crow::json::wvalue jsonRes;
       
        int status = registerPatient(name,address,"Public","Doctor Visit","Null");//storing to a database
        
        if(status == 200){
            jsonRes[name] = "Registered"; 
        }else{
            jsonRes[name] = "Please enter valid details";
        }
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
                patientBookAppo.visitPurpose = "Doctor Visit";
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
    http://0.0.0.0:3333/prescribe?pid=9&name=Anthony&treatment=Paracetamol 
    */

    CROW_ROUTE(app, "/prescribe").methods("GET"_method)([&](const crow::request& req) {
        
        int id =  atoi(req.url_params.get("pid"));
        string treatment = req.url_params.get("treatment");
        string name = req.url_params.get("name");
        crow::json::wvalue response; 
        PatientPrescription prescribe;

        //patient prescription database 
        sqlite3 *db;
        char *zErrMsg = 0;
        int rc;

        rc = sqlite3_open("test.db", &db);
        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stderr, "Opened database successfully ph yeah\n");
        }
       const char* sql = "CREATE TABLE IF NOT EXISTS prescription (" "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, " "NAME TEXT NOT NULL, " "PRESCRIPTION TEXT);";

       rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);//Executing the sql command
   
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error creating prescription table: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Prescription table ok\n");
        }

        string query = "INSERT INTO prescription (NAME, PRESCRIPTION) VALUES ('" + name + "','"+ treatment +"');";
        const char* insertSql = query.c_str(); // Convert to const char*
        sqlite3_stmt *stmt;
        /* Execute SQL statement for inserting the records*/
        rc = sqlite3_prepare_v2(db, insertSql, -1,&stmt,nullptr);
    
        if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error inserting records: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Records inserted successfully\n");
       
        }
        sqlite3_close(db);

        prescribe.handlePrescription(id,treatment);
        response[name] = "Prescribed";

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
        response[std::to_string(id)] = getTreatment;
        return crow::response(response);

    });

    /*
    ===========Task3=========
    Handling billing and insurance claims
    */

    string purpose = "test ";

    CROW_ROUTE(app, "/bill").methods("GET"_method)([&purpose](const crow::request& req) {

        crow::json::wvalue response; 
        Insurance insuranceCompany;
        Bills bill;
        int id =  atoi(req.url_params.get("pid"));
        //For example If the purpose of visit is simply visiting a doctor, no payment
        //But if you get prescription, pay the amount 
        string option = retrieveData(id,"PURPOSE");
        purpose = option;
        cout<<purpose;

        response[std::to_string(id)] = bill.bills(id,option);
        
        return crow::response(response);

    });

    http://0.0.0.0:3333/insurance_claim?pid=2 

    CROW_ROUTE(app, "/insurance_claim").methods("GET"_method)([&purpose](const crow::request& req) {

        int id =  atoi(req.url_params.get("pid"));

        cout<<purpose;//Debugging 
        crow::json::wvalue response; 
        string coverage = retrieveData(id,"INSURANCE");
        string status;
        if (purpose == "Doctor Visit" && coverage == "Public") {
            status = "Covered";
        }
        response[std::to_string(id)] = status;
        return crow::response(response);

    });

    /* ==========Task4==========
    Inventory Management
    Jalal 
    */
    Inventory inventory;
    CROW_ROUTE(app,"/inventory/set_threshold/<string>/<int>")
    ([&inventory](const string &item, int threshold){
        inventory.setthreshold(item,threshold);
        return crow::json::wvalue{{"status","succes"},{"message","Threshold is set for "+ item}};

    });
    CROW_ROUTE(app,"/inventory/low_supplies")
        ([&inventory]() {
        auto lowSupplies = inventory.checkthreshold();
        return crow::response(lowSupplies);
    });
    CROW_ROUTE(app, "/inventory/all")
    ([&inventory]() {
        return inventory.getInventoryJSON();
    });
    CROW_ROUTE(app, "/inventory/add_supply/<string>/<int>")
    ([&inventory](const string &item, int quantity) {
        
        inventory.addsupply(item, quantity);
        return crow::json::wvalue{{"status", "success"}, {"message", item + " updated with " + to_string(quantity) + " units"}};
    });

    app.port(3333).run();
    return 0;
}


