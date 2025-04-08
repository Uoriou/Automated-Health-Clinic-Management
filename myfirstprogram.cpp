#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "crow.h"
#include <sqlite3.h>
#include <unordered_set>


using namespace std;
using namespace crow;


/*
A class to represent a patient
any information about the patient can be stored here before passing it to SQL
*/
class Patient{
    private:
        string name = "";
        string medicalHistory = "";
        string insuranceCoverage;
        string address;
        string appointment = "";
        string purpose = "";
        string prescription = "";
    public:

        Patient(string name,string address,string insuranceCoverage,string purpose,string appointment,string prescription){
            this->name = name;
            this->address = address;
            this->insuranceCoverage = insuranceCoverage;
            this->purpose = purpose;
            this->appointment = appointment;
            this->prescription = prescription;
        }

        string getPatientName(){
            return name;
        }

        string getPatientAddress(){
            return address;
        }

        string getInsuranceCoverage(){
            return insuranceCoverage;
        }

        string getAppointment(){
            return appointment;
        }

        string getPurpose(){
            return purpose;
        }

        static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
            int i;
            for(i = 0; i<argc; i++) {
               printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            }
            printf("\n");
            return 0;
         }
         
        // I forgot to register the prescription  here thats why it was causing a segmentation fault error 
        int registerPatient(string name,string address,string insurance,string purpose,string appo,string prescription){

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

            sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
        
            string query = "INSERT INTO patient (NAME, ADDRESS, INSURANCE, PURPOSE, APPOINTMENT,PRESCRIPTION) VALUES ('" +name + "', " +
                       (address.empty() ? "NULL" : "'" + address + "'") + ", " +
                       (insurance.empty() ? "NULL" : "'" + insurance + "'") + ", " +
                       (purpose.empty() ? "NULL" : "'" + purpose + "'") + ", " +
                       (appo.empty() ? "NULL" : "'" + appo + "'")+ ", " +
                       (prescription.empty() ? "NULL" : "'" + prescription + "'")+ ");";
        
        
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
    //Initialise doctors here 
};

class Appointment{

    //To book an appointment, map pid and docId
    //In the doctorclass, the following timeslot is mapped to docID 
    public:
        map<int,int>patientToDocMap;
        vector<string>availableTimeSlots = {"9:00","9:30","10:00","10:30","11:00","11:30","12:30"};
       
};

class Prescription{

    private:
        string treatment;
        
    public:

        Prescription(string treatment){
            this->treatment = treatment;
        }

        string getTreatment(){
            return treatment;
        }

};


class Bills{
    
    Patient patientObj;//to get a pid and insurance coverage status
    //PatientPrescription prescriptionDetails;//if a patient receives medication, they need to pay the amount

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
        map<string, int> supplies; // quantity of medical supplies
        map<string, int> threshold;
        string item = ""; // The name of the medical supplies, syringes, bandages, etc.
        

        // here call the createInventoryTable() 
    public:
        //Getter
        string getItem(){
            return item;
        }

        map<string, int> getSupplies() const {
            return supplies;
        }
        map<string, int> getThreshold() const {
            return threshold;
        }
        //Constructor in C++ 
        Inventory(string item, const map<string, int> &supplies, const map<string, int> &threshold) {
            this->item = item;
            this->supplies = supplies;
            this->threshold = threshold;
            //addtoinventory();
        }
        
        void addsupply(const string &item, int quantity) {


            sqlite3 *db;
            char *zErrMsg = 0;
            int rc;
        
            rc = sqlite3_open("test.db", &db);
        
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                return;

            } else {
                fprintf(stderr, "Opened database successfully\n");
                
            }

            sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
        
            string query = "INSERT INTO inventory (ITEM, QUANTITY) VALUES ('" +item + "', " +
                       
                       (supplies.empty() ? "NULL" : "'" + supplies + "'")+ ");";
        
        
                const char* insertSql = query.c_str(); // Convert to const char*
            
                /* Execute SQL statement for inserting the records*/
                rc = sqlite3_exec(db, insertSql, callback, 0, &zErrMsg);
            
                if( rc != SQLITE_OK ){
                    fprintf(stderr, "SQL error inserting records: %s\n", zErrMsg);
                    sqlite3_free(zErrMsg);
                
                } else {
                    fprintf(stdout, "Records inserted successfully\n");
                
                }
                sqlite3_close(db);
            //supplies[item] += quantity;
            //saveinventory();
        }

        void setthreshold(const string &item,  int thresholds) {
            threshold[item] = thresholds;
            
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
               

                if (supplies[item] == 0) {
                    //resupply(item);
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


//Printing the entries in the database
//Helper functions for printing the entries in the database 
//This function is used for appointment booking 
vector<string>container;
vector<string>okAppos;

vector<string> appointmentsHandler(vector<string>& container, string id,string date, bool isAddMode){
    
    if(isAddMode){
        container.push_back(date);
        cout<<"Added to the container "<<date<<" \n";
        return container;
    }else{
        return container;
    }
}
//This function is also used for appointment booking 
static int callback(void *userData, int argc, char **argv, char **azColName) {
    int i;
    string key = "";
    map<string,string>patientData;
    
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //printf("%s",argv[i]);// i think this is returning the filed info

        if (string(azColName[i]) == "ID"){
            key = argv[i];
            // ID is the key
            patientData[key] = "";
        }   
        if (string(azColName[i]) == "APPOINTMENT"){
            string value = argv[i];// just to verify that argv[i] is string 
            //Update the appointment field associated with the ID
            patientData[key] = value;
            if(strcmp(argv[i], "Null") != 0){
                //use emptyappos to store the appointments
                cout<<"appointment exist"<<"\n";
                okAppos.push_back(argv[i]);
            }
            vector<string> appointmentLists = appointmentsHandler(container,key,value,true);//id and date
        }
    }
    for(auto const& [key, val] : patientData)
    {
        std::cout << key        
                  << ':'  
                  << val        
                  << std::endl;
    }
   printf("\n");
   return 0;
}

//Use this when only needed 
int deleteTable(char *zErrMsg,int rc){// idk if the pointer initilisation is correct 

    sqlite3 *db;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(-1);
    } else {
        fprintf(stderr, "Opened database successfully\n");
        
    }
    const char* dropTable = "DROP TABLE IF EXISTS prescription;";
    //Execute the SQL command
    rc = sqlite3_exec(db, dropTable, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Table dropped successfully" << std::endl;
    }

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
    
    //Patient table initialised
    const char* sql = "CREATE TABLE IF NOT EXISTS patient ("
                  "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                  "NAME TEXT NOT NULL, "
                  "ADDRESS TEXT, "
                  "INSURANCE TEXT, "
                  "PURPOSE TEXT, "
                  "APPOINTMENT TEXT,"
                  "PRESCRIPTION TEXT);";
    

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);//Executing the sql command
   
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table Ok\n");
    }

    return 0;
}

int createInventoryTable(){

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
    
    //Inventory table initialised
    const char* sql = "CREATE TABLE IF NOT EXISTS inventory ("
                  "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                  "ITEM TEXT NOT NULL, "
                  "QUANTITY INTEGER NOT NULL);";
    

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);//Executing the sql command
   
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table Ok\n");
    }

    return 0;
}


/*
A function to book an appointment
The original patient database is updated because when a patient registers for the first time,
the appointment field is null 
*/ 
int updatePatient(string name, int pid,string date,string time){

    //These can be inside the main method and passed as parameters
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    string appointment = time + date;
    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
        
    }

    string query = "UPDATE patient SET APPOINTMENT = " +
    (appointment.empty() ? "NULL" : "'" + appointment + "'") + 
    " WHERE ID = " + std::to_string(pid) + ";";

    const char* insertSql = query.c_str(); // Convert to const char*

    /* Execute SQL statement for inserting the records*/
    rc = sqlite3_exec(db, insertSql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error updating one or more records: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 500;
    } else {
        fprintf(stdout, "Record updated successfully\n");
        return 202;
    }

    sqlite3_close(db);
}

//Retrieve patients records, i might not need this..... 
string retrieveData(int pid,string dbName){// for example... option == PURPOSE

    crow::json::wvalue response; 
    sqlite3 *db;
    sqlite3_stmt* stmt;
    char *zErrMsg = 0;
    int rc;
    string name = "";
    string address = "";
    string insurance = "";
    string purpose = "";
    string appointment = "";
    string prescription = "";

    /* Open database */
    rc = sqlite3_open("test.db", &db);
    
    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully / retrieve \n");
    }
    
    /* Create SQL statement for retrieving data*/
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

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int id = sqlite3_column_int(stmt, 0);
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        insurance = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        purpose = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        appointment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        prescription = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while iterating rows: " << sqlite3_errmsg(db) << std::endl;

    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    response[to_string(pid)] = {{"name", name}, {"address", address}, {"insurance", insurance}, {"purpose", purpose}, {"appointment", appointment}};
    return response.dump();
   
        
}

//Retrieve patients records for other operation purposes
string retrieveOnePatientRecord(int pid,string option){// for example... option == PURPOSE

    sqlite3 *db;
    sqlite3_stmt* stmt;

    int rc;
    char *zErrMsg = 0;

    string name = "";
    string address = "";
    string insurance = "";
    string purpose = "";
    string appointment = "";
    string prescription = "";

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
   //Why it gets executed when id doesnt exisit in the table ???/
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

    // Check if any rows were actually returned
    bool rowFound = false;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        
        int id = sqlite3_column_int(stmt, 0);
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        insurance = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        purpose = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        appointment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        prescription  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        rowFound = true;
    }
    if (rc != SQLITE_DONE) {
        std::cerr << "Error while iterating rows: " << sqlite3_errmsg(db) << std::endl;
    }


    if (!rowFound) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "NF"; 
    }
    
    if (option == "NAME"){
        return name;
    }
    if (option == "ADDRESS"){
        return address;
    }
    if (option == "INSURANCE"){
        return insurance;
    }
    if (option == "PURPOSE"){
        return purpose;
    }
    if(option == "APPOINTMENT"){
        return appointment;
    }
    if(option == "PRESCRIPTION"){
        return prescription;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return "nullptr";
}

//Customised option filed to get a specific information from the database
//I am now using it for appointment logic to use the call back function
// but should be used for other purposes
string executeCallBack(string option){// for example... option == PURPOSE

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
   string sql = "SELECT * from patient;";

   /* Execute SQL statement */ // this was the original one 
   //sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error preparing statement for retrieving: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      sqlite3_close(db); 
    }else{
        cout<<"Good SQL"<<"\n";
    }
    if (rc != SQLITE_DONE) {
        std::cerr << "Error while iterating rows: " << sqlite3_errmsg(db) << std::endl;
    }

    //sqlite3_finalize(stmt);
    sqlite3_close(db);
    return "OK";
}
//This is another callback function to retrieve the appointment slots
vector<string>appointmentSlots(vector<string>& okAppos){
    string status = executeCallBack("APPOINTMENT");
    return okAppos;
}


int main() {

    char *zErrMsg = 0;
    int rc;
    crow::SimpleApp app;//Used to initialise CROW server 
    createInventoryTable(); 
    

    /*
    ==========Task 1========== 
    Registration and booking an appointment 
    */
    //http://0.0.0.0:3333/register?name=Anthony&address=Betis&insurance=public&purpose=Null&prescription=Pain%20Killer
    //The patient can not have an appointment at the time of the registration
    CROW_ROUTE(app, "/register").methods("GET"_method)([&](const crow::request& req) {
        if (!req.url_params.get("name") || !req.url_params.get("address")) {
            return crow::response(404, "Missing 'name' or 'address' query parameter.");
        }else{
            
            // Get parameters safely, pointing to the memory addresses to avoid segmentation fault
            const char* nameStr = req.url_params.get("name");
            const char* addressStr = req.url_params.get("address");
            const char* insuranceStr = req.url_params.get("insurance");
            const char* purposeStr = req.url_params.get("purpose");
            const char* prescriptionStr = req.url_params.get("prescription");

            if ( !nameStr || !addressStr || !insuranceStr || !purposeStr || !prescriptionStr) {
                return crow::response(400, "One or more details missing");
            }

            
            //string pid = pidStr;// wait i dont need this ?  
            string name = nameStr;
            string address = addressStr;
            string insurance = insuranceStr;
            string purpose = purposeStr;
            string prescription = prescriptionStr;
            Patient patient{name,address,insurance,purpose,"NULL",prescription};

            //int id = std::stoi(pid); 
            crow::json::wvalue jsonRes;
            //Here ive changed becaue i put it inside the class 
            int status = patient.registerPatient(patient.getPatientName(),address,insurance,purpose,"Null",prescription);//storing to a database
            if(status == 200){
                //Here it should return a id so that a patient can book an appointment
                jsonRes[name + " Registered"] = "You are now registered"; 
            }else{
                jsonRes[name] = "Something went wrong with the registration";
            }return crow::response(jsonRes);
        }
    });

    CROW_ROUTE(app, "/checkBookingSlots").methods("GET"_method)([&](const crow::request& req) {
        //just display the available slots
        Appointment patientAppointment;
        vector<string> takenSlots= appointmentSlots(okAppos);// this is actually taken appointments
        crow::json::wvalue json({{"message", "Booking status"}});

        vector<crow::json::wvalue> slots;
        unordered_set<string> set(takenSlots.begin(), takenSlots.end());
        //Use set to filter the hours 
        for (const auto& slot : set) {
            slots.push_back(crow::json::wvalue(slot));
        }
        json["The following slots are taken"] = std::move(slots);
        return json;
    });
    
    // To book an appointment 
    // address:port/bookappointment?pid = patient id(int)& date = 12.12.2024(format of the not defined yet is just a string)&time=hour:minute 
    //http://0.0.0.0:3333/book?pid=1&date=12.12.2024&time=10:00&name=Mario
    CROW_ROUTE(app, "/book").methods("GET"_method)([&](const crow::request& req) {
        
        if (!req.url_params.get("pid") || !req.url_params.get("date") || !req.url_params.get("time")) {
            return crow::response(500, "Please enter your full information / id, date, time and name");
        }else{
            try{
                // Get parameters safely to avoid segmentation fault
                const char* pidStr = req.url_params.get("pid");
                const char* dateStr = req.url_params.get("date");
                const char* timeStr = req.url_params.get("time");
                const char* nameStr = req.url_params.get("name");

                if (!pidStr || !dateStr || !timeStr || !nameStr) {
                    return crow::response(400, "One or more details missing");
                }

                //get the timetable, operating hours of the clinic from PatientAppointment class
                Appointment patientAppointment;
                for(int i = 0; i < patientAppointment.availableTimeSlots.size(); i++){
                    //if (std::find(patientAppointment.availableTimeSlots.begin(),
                        //patientAppointment.availableTimeSlots.end(),time)!=patientAppointment.availableTimeSlots.end()){
                        //cout<<"Time slot is valid "<<"\n";
                   // }
                    cout<<patientAppointment.availableTimeSlots[i]<<"\n";
                }
                //Database retrieve the appointment slots
                //Using callback function
                string test = executeCallBack("APPOINTMENT");
                
                std::string pid = pidStr;
                std::string date = dateStr;
                std::string time = timeStr;
                std::string name = nameStr;
                //Access the map in callback to get the appointment slots
                //and check if the time table is taken or not / database 
                vector<string> appointmentLists = appointmentsHandler(container,pid,date,false);
                string appointmentDateInput  = time+date;
                cout << "Size of appointmentLists: " << appointmentLists.size() << endl;

                cout<<appointmentDateInput<<"\n";
                for(int i = 0; i< appointmentLists.size();i++){
                   //Might be better to display available slots
                    if(appointmentDateInput == appointmentLists[i]){
                        return crow::response("Sorry the slot: " +time + " on " + date + " is taken");
                    }
                }
                //If the slot is not taken, book the appointment
                int id = std::stoi(pid);  
                int status = updatePatient(name,id,date,time);

                if (status == 202){
                    return crow::response(name + " you are booked in: " +time + " on " + date);
                }
                return crow::response(505,"Something went wrong with the appointment booking"); 
                
            }catch(exception e){
                return crow::response(505,"Something went wrong with the appointment booking");
            }
        }

        return crow::response("Booking page");
    });

    /*
    ==========Task 2========== 
    Providing prescription to patients
    http://0.0.0.0:3333/prescribe?pid=1&treatment=Paracetamol 
    
    1 )Implementing a procedure to record prescriptions associated with a patient ID
    2 )Updating medical records (database) after each patient visit
    3 )Searching to retrieve patient’s medical history
    */

    CROW_ROUTE(app, "/prescribe").methods("GET"_method)([&](const crow::request& req) {
    
        if (!req.url_params.get("pid") || !req.url_params.get("treatment")) {
            return crow::response(500, "One or more details missing");
        }else{
            const char* pidStr = req.url_params.get("pid");
            const char*prescriptionPtr = req.url_params.get("treatment");

            if(pidStr == nullptr || prescriptionPtr == nullptr){
                return crow::response(404, "One or more details missing");
            }
            
            string treatment = prescriptionPtr;
            Prescription prescriptionObj{treatment};
            string pid = pidStr;

            sqlite3 *db;
            char *zErrMsg = 0;
            sqlite3_stmt *stmt;
            int rc;

            rc = sqlite3_open("test.db", &db);
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            } else {
                fprintf(stderr, "Opened database successfully\n");
            }
    
            string query = "UPDATE patient SET PRESCRIPTION  = ('"+ prescriptionObj.getTreatment() +"') WHERE ID = "+pid+";";
            cout << "Patient ID: " << pid << ", Treatment: " << prescriptionObj.getTreatment() << endl;
            const char* insertSql = query.c_str(); // Convert to const char*

            /* Execute SQL statement for inserting the records*/
            rc = sqlite3_exec(db, insertSql, callback, 0, &zErrMsg);

            if( rc != SQLITE_OK ){
                fprintf(stderr, "SQL error updating one or more records: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
               
            } else {
                fprintf(stdout, "Record updated successfully\n");
                
            }

            sqlite3_close(db);
            
            return crow::response("Prescribed OK");
        }
    });

    //Problem here is that the data is not being retrieved from the database
    CROW_ROUTE(app, "/getMedicalRecords").methods("GET"_method)([&](const crow::request& req) {
        //Call a db function to retrieve the medical records
        if (!req.url_params.get("pid")) {
            return crow::response(404, "Patient ID missing, please enter the patient ID");
        }

        try{
            sqlite3 *db;
            sqlite3_stmt* stmt;
            char *zErrMsg = 0;
            int rc;
            int pid = std::stoi(req.url_params.get("pid"));
            string name = "";
            string prescription = "";
            auto response = retrieveOnePatientRecord(pid,"PRESCRIPTION");
            cout<<response; // OK now i can get the records 
            
            // The problem might be because of the execution of the sql statement 
            /* Open database */
            rc = sqlite3_open("test.db", &db);
            
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            } else {
                fprintf(stderr, "Opened database successfully / retrieve \n");
            }
            /*string query = "SELECT patient.NAME, prescription.PRESCRIPTION FROM patient "
            "INNER JOIN prescription ON patient.ID = prescription.PATIENT_ID "
            "WHERE patient.ID = " + std::to_string(pid) + ";";
            rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, pid);
            
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                prescription = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                cout << "Patient: " << name << ", Prescription: " << prescription << endl;
            }*/

            string query = "SELECT * FROM patient WHERE ID =  " + std::to_string(pid) + ";";
            rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
   
            if( rc != SQLITE_OK ) {
               fprintf(stderr, "SQL error: %s\n", zErrMsg);
               sqlite3_free(zErrMsg);
            } else {
               fprintf(stdout, "Operation done successfully\n");
            }
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            sqlite3_close(db);

    
            crow::json::wvalue jsonResponse;
            jsonResponse["message"]["id"] = std::to_string(pid);
            jsonResponse["message"]["prescription"] = response;
            return crow::response(jsonResponse);
        }catch(exception e){
            return crow::response(404, "Something went wrong, please try again with correct details");
        }
        
    });

    /*
    ===========Task3=========
    Handling billing and insurance claims
    */

    CROW_ROUTE(app, "/bill").methods("GET"_method)([&](const crow::request& req) {

        crow::json::wvalue payment; 
        Insurance insuranceCompany;
        //Bills bill;
        if (!req.url_params.get("pid")) {
            return crow::response(404, "Patient ID missing, please enter the patient ID");
        }
        try{
            //Use char* idPtr = req.url_params.get("pid"); to avoid segmentation fault
            //but let me see if this works
            int id =  atoi(req.url_params.get("pid"));
            // Now it is using the purpose but eventually also use prescription
            auto purpose = retrieveOnePatientRecord(id,"PURPOSE"); 
            auto prescription = retrieveOnePatientRecord(id,"PRESCRIPTION");
            if(purpose == "Doctor Visit"){ 
                payment[std::to_string(id)] = "No payment / Billing the insurance company";
                return crow::response(payment);            
            }
            if (purpose == "Null"){
                payment[std::to_string(id)] = "Null";
                return response(payment);
            }
            if(purpose == "NF"){
                return crow::response(404, "Patient not found");
            }
            else{
                payment[std::to_string(id)] = "Pay the amount";
                return response(payment);
            }

        }
        catch(exception e){
            return crow::response(404, "Something went wrong");
        }

    });

    //http://0.0.0.0:3333/insurance_claim?pid=2 

    CROW_ROUTE(app, "/insurance_claim").methods("GET"_method)([&](const crow::request& req) {

        int id =  atoi(req.url_params.get("pid"));
 
        crow::json::wvalue response; 
       
        string purpose = retrieveOnePatientRecord(id,"PURPOSE"); 
        //Something is wrong with this --  >
        //string prescription = retrieveOnePatientRecord(id,"PRESCRIPTION");
        //cout<<prescription;
        cout<<purpose;
        string status;
        //if (purpose == "Doctor Visit" && coverage == "Public") {
            //status = "Covered";
        //}
        response[std::to_string(id)] = status;
        return crow::response(response);

    });

    
    /* 
    ==========Task4==========
    Inventory Management
   
    1 ) Implement a procedure to record and update the quantity of medical supplies
    2 ) Alert a member of the medical staff when the quantity of a supply falls below a certain threshold
    */

    /*
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
    */

    app.port(8080).run();
    return 0;
}


