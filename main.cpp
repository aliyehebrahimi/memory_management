/*
    CS 540: Database Management Systems
    Instructor: Dr. Arash Termehchy
    Homework: Homework: Hash Indexing (w7)

    Group 9: Aliyeh Ebrahimi (934634122), Giancarlo Tomasello (932870898)
    emails: ebrahali@oregonstate.edu, tomaselg@oregonstate.edu
*/

#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include "classes.h"

using namespace std;


int main() {

    // Create the index
    HashIndex hashIndex("EmployeeIndex");
    hashIndex.createFromFile("Employee_large.csv");

    // Loop to lookup IDs until user is ready to quit
    std::string searchID;
    std::cout << "Enter the employee ID to find or type exit to terminate: ";
    while (std::cin >> searchID && searchID != "exit") {
        int64_t id = std::stoll(searchID);
        std::string record;
        hashIndex.findAndPrintEmployee(id);
    }

    return 0;
}
