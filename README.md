# Automated Health Clinic Management System (AHCM)

## Project Scope / Project Objectives
The local community health clinic is transitioning from a paper-based system to a
digital one. They need a system where patients can register with their details (name,
address, medical history) and receive a unique patient ID. Once registered, patients
should be able to book appointments using their patient ID.
Therefore we created a simple program in C++, designed to streamline patient care and clinic management through automation.
Features include, a registration process, managing patients info, a prescription assignment and payment process.


## Technical Requirements:

To run the Automated Health Clinic, ensure your system meets the following requirements:

- Operating System: Windows, macOS, or Linux

- Docker

## Technolgy used:

- C++ programming language

- Crow (C++ Web Framework) Clone the git -> https://github.com/CrowCpp/Crow.git
  
-  Database: SQLlite3 / Macs -> brew install sqlite3 / Linux -> sudo apt-get install libsqlite3-dev

## Installation Guidline

- Please install Docker 
- Clone this AHCM repository

## Run the docker image

Once Docker is installed, build the docker image 
```bash
  docker build -t "your-image-name" .
``` 
Then, run the container
```bash
  docker run -p 8080:8080 "your-image-name"
```
Alternatively, if there is a docker-compose.yml file, 
you can build and start all services with a single command:

```bash
docker compose up --build
```

Apply migrations and create a superuser in a terminal  

```bash
docker-compose exec web python manage.py migrate
docker-compose exec web python manage.py createsuperuser
```
    
To stop the continer press Ctrl+C


## How to access the application:

Open your browser and navigate to http://localhost:port for the frontend. Here is an example use for registering a patient -> 

http://0.0.0.0:8080/register?name=Anthony&address=Betis

## Features Implemented:

1, Patient Registration (Example useage)

 + ``` //http://0.0.0.0:8080/register?name=Anthony&address=Betis&insurance=public&purpose=Null&prescription=Pain%20Killer```

2, Appointment Booking 
Check the already booked appointment slot by 
 + ```http://0.0.0.0:8080/checkBookingSlots```
   
Book an appintment
 + ```http://0.0.0.0:8080/book?pid=1&date=12.12.2024&time=10:00&name=Mario```

3,Providing prescription to patients
 + ```http://0.0.0.0:8080/prescribe?pid=1&treatment=Paracetamol```

3, Handling Insuarance
 + 

4, Inventory Management 

To add medicines to Inventory
+ ```"http://0.0.0.0:8080/inventory/add_item/<string>/<int>/<int>"```
  
To get inventory
+ ```"http://0.0.0.0:8080/inventory/all"```

To check any item that is running low 

+ ```"http://0.0.0.0:8080/inventory/low_supplies"```

To add inventory

+ ```""http://0.0.0.0:8080/inventory/add_supply/<string>/<int>"```

  


