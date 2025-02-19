# Automated Health Clinic Management System 

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

- C++ and C++ Compiler: GCC 9.3 or higher / g++
  
- Databsese SQLlite3

## Libraries used:

Make sure the following libraries are installed

- Crow (C++ Web Framework) Clone the git -> https://github.com/CrowCpp/Crow.git
  
-  Database: SQLlite3 / Macs -> brew install sqlite3 / Linux -> sudo apt-get install libsqlite3-dev

## How to run the application:

After navigating to the correct folder where the dowloaded git file is located, run the application 
by specifying the type of your chosen compiler and the file name followed by the command for running the libraries -> 

g++ -o myfirstprogram myfirstprogram.cpp -std=c++17 -pthread -l sqlite3 

## How to access the application:

Open your browser and navigate to http://localhost:port for the frontend. Here is an example use for registering a patient -> 

http://0.0.0.0:3333/register?name=Anthony&address=Betis

## Manual Guide
  


