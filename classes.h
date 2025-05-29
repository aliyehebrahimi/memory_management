/*
    CS 540: Database Management Systems
    Instructor: Dr. Arash Termehchy
    Homework: Homework: Hash Indexing (w7)

    Group 9: Aliyeh Ebrahimi (934634122), Giancarlo Tomasello (932870898)
    emails: ebrahali@oregonstate.edu, tomaselg@oregonstate.edu
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cmath>
#include <cstdint>

using namespace std;

class Record {
public:
    int64_t id, manager_id; // Employee ID and their manager's ID
    string bio, name; // Fixed length string to store employee name and biography

    Record(vector<string> &fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }
	
	// Function to get the size of the record
    int get_size() {
        // sizeof(int) is for name/bio size() in serialize function
        return sizeof(id) + sizeof(manager_id) + sizeof(int) + name.size() + sizeof(int) + bio.size() + sizeof(char); 
    }

    // Function to serialize the record for writing to file
    string serialize() const {
        ostringstream oss;
        char delimiter = '$';
        oss.write(&delimiter, sizeof(char));
        oss.write(reinterpret_cast<const char *>(&id), sizeof(id));
        oss.write(reinterpret_cast<const char *>(&manager_id), sizeof(manager_id));
        int name_len = name.size();
        int bio_len = bio.size();
        oss.write(reinterpret_cast<const char *>(&name_len), sizeof(name_len));
        oss.write(name.c_str(), name.size());
        oss.write(reinterpret_cast<const char *>(&bio_len), sizeof(bio_len));
        oss.write(bio.c_str(), bio.size());
        return oss.str();
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

class Page {
public:
    vector<Record> records; // Data_Area containing the records
    vector<pair<int, int>> slot_directory; // Slot directory containing offset and size of each record
    int cur_size = sizeof(int); // Current size of the page including the overflow page pointer. if you also write the length of slot directory change it accordingly.
    int overflowPointerIndex;  // Initially set to -1, indicating the page has no overflow page. 
							   // Update it to the position of the overflow page when one is created.

    static const int PAGE_SIZE = 4096;
    
    // Constructor
    Page(): overflowPointerIndex(-1) {}

    // Function to insert a record into the page
    bool insert_record_into_page(Record r) {
        
        int record_size = r.get_size();
        int slot_size = sizeof(int) * 2;


        if (cur_size + record_size + slot_size > PAGE_SIZE)
            return false;
        
        int slot_dirSize = slot_directory.size() * slot_size;
        int offset = cur_size - sizeof(int) - slot_dirSize; // data area grows from start
        

        records.push_back(r);
        pair<int,int> newPair = {offset, record_size};
        slot_directory.push_back(newPair);

        cur_size += (record_size + slot_size);
        return true;
    }



    // Function to write the page to a binary output stream. You may use
    void write_into_data_file(fstream &out) const {
        char page_data[PAGE_SIZE] = {0}; // Buffer to hold page data
        int offset = 0;

        // Write records into page_data buffer
        for (const auto &record: records) {
            string serialized = record.serialize();
            memcpy(page_data + offset, serialized.c_str(), serialized.size());
            offset += serialized.size();
        }

        offset = PAGE_SIZE - sizeof(int);        
        memcpy(page_data + offset, &overflowPointerIndex, sizeof(int));
        
        for (const auto &slots: slot_directory) { 
            offset -= sizeof(slots);
            memcpy(page_data + offset, &slots, sizeof(slots));
        }
        
        // Write the page_data buffer to the output stream
        out.write(page_data, PAGE_SIZE);

    }

    

    // Function to read a page from a binary input stream
    bool read_from_data_file(istream &in) {
        char page_data[PAGE_SIZE] = {0}; // Buffer to hold page data
        in.read(page_data, PAGE_SIZE); // Read data from input stream

        streamsize bytes_read = in.gcount();

        if (bytes_read > 0 && bytes_read != PAGE_SIZE) {
            cerr << "Incomplete read: Expected " << PAGE_SIZE << 
            " bytes, but only read " << bytes_read << " bytes." << endl;
            return false;
        }
                
        // TODO: Process data to fill the records, slot_directory, and overflowPointerIndex
       
        // read overflow pointer
        memcpy(&overflowPointerIndex, page_data + PAGE_SIZE - sizeof(int), sizeof(int));

        
        int slotSize = sizeof(int) * 2;
        int slotOffset = slotSize + sizeof(int);
        int offset = 0;
        // *TODO: You may process page_data (4 KB page) and put the information to the records and slot_directory (main memory).
        while(page_data[offset] == '$'){
            offset += sizeof(char);

            //Load slots---------------------------
            pair<int,int> newPair;
            memcpy(&newPair, page_data + PAGE_SIZE - slotOffset, slotSize);
            
            slot_directory.push_back(newPair); 
            slotOffset += slotSize;
            
            //load record------------------------------
            vector<string> recordFields = {"1", "1", "1", "1"};
            Record newRecord = Record(recordFields);
            
            char* int_chars = new char[sizeof(int64_t)];

            //read id
            memcpy(int_chars, page_data + offset, sizeof(newRecord.id));
            newRecord.id = *reinterpret_cast<int64_t*>(int_chars);
            offset += sizeof(newRecord.id);
            
            //read manager_id
            memcpy(int_chars, page_data + offset, sizeof(newRecord.manager_id));
            newRecord.manager_id = *reinterpret_cast<int64_t*>(int_chars);
            offset += sizeof(newRecord.manager_id);
            
            //read in name
            memcpy(int_chars, page_data + offset, sizeof(int));
            int nameSize = *reinterpret_cast<int*>(int_chars);
            offset += sizeof(nameSize);
            
            string name;
            name.resize(nameSize);
            memcpy(&name[0], page_data + offset, nameSize);
            
            newRecord.name = name;
            offset += nameSize;
            
            //read in bio
            memcpy(int_chars, page_data + offset, sizeof(int));
            int bioSize = *reinterpret_cast<int*>(int_chars);
            offset += sizeof(bioSize);
            
            string bio;
            bio.resize(bioSize);
            memcpy(&bio[0], page_data + offset, bioSize);
            
            newRecord.bio = bio;
            offset += bioSize;
            
            //Add Record to vector
            records.push_back(newRecord);
        }
            
        return true;
         
    }
};

class HashIndex {
private:
    const size_t maxCacheSize = 1; // Maximum number of pages in the buffer
    const int PAGE_SIZE = 4096; // Size of each page in bytes
    vector<int> PageDirectory; // Map h(id) to a bucket location in EmployeeIndex(e.g., the jth bucket)
    // can scan to correct bucket using j*PAGE_SIZE as offset (using seek function)
    // can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int nextFreePage; // Next place to write a bucket
    string fileName;

    // Function to compute hash value for a given ID
    int compute_hash_value(int64_t id) {
        // TODO: Implement the hash function h = id mod 2^8
        return id % 65536;
    }

    // Function to add a new record to an existing page in the index file
    void addRecordToIndex(int pageIndex, Page &page, Record &record) {
        // Open index file in binary mode for updating
        
        fstream indexFile(fileName, ios::binary | ios::in | ios::out);
        

        if (!indexFile.is_open()) {
            cerr << "Error: Unable to open index file for adding record." << endl;
            return;
        }
        
		      
        while (!page.insert_record_into_page(record)) {
            if (page.overflowPointerIndex == -1) {
                // Create new overflow page
                page.overflowPointerIndex = nextFreePage++;

                // Write current page (with updated overflowPointerIndex)
                
                indexFile.seekp(pageIndex * PAGE_SIZE, ios::beg);
                page.write_into_data_file(indexFile);
                pageIndex = page.overflowPointerIndex;
                page = Page();  // Empty new page
            } else {
                // Go to the next overflow page
                pageIndex = page.overflowPointerIndex;
                
                indexFile.seekg(pageIndex * PAGE_SIZE, ios::beg);
                page.read_from_data_file(indexFile);
            }
        }

        indexFile.seekp(pageIndex * PAGE_SIZE, ios::beg);
        page.write_into_data_file(indexFile);
        
        // Close the index file
        indexFile.close();
    }


    // Function to search for a record by ID in a given page of the index file
    void searchRecordByIdInPage(int pageIndex, int64_t id) {
        // Open index file in binary mode for reading
        ifstream indexFile(fileName, ios::binary | ios::in);

        // TODO:
        //  - Search for the record by ID in the page
        //  - Check for overflow pages and report if record with given ID is not found
        while (pageIndex != -1) {
            // Seek to the appropriate position in the index file
            
            indexFile.seekg(pageIndex * PAGE_SIZE, ios::beg);

            // Read the page from the index file
            Page page;
            page.read_from_data_file(indexFile);
            for (Record &rec : page.records) {
                if (rec.id == id) {
                    cout << "Employee found:\n";
                    rec.print();
                    indexFile.close();
                    return;
                }
            }

            pageIndex = page.overflowPointerIndex;
        }
        
        cout << "Employee with ID " << id << " not found.\n";
        indexFile.close();
    
    }

public:
    HashIndex(string indexFileName): nextFreePage(0), fileName(indexFileName) {
        // Create the index file if it doesn't exist
        ofstream createFile(indexFileName, ios::binary | ios::app);
        createFile.close();

        // Initialize PageDirectory
        PageDirectory.resize(65536, -1);
    }

    // Function to create hash index from Employee CSV file
    void createFromFile(string csvFileName) {
        // Read CSV file and add records to index
        // Open the CSV file for reading
        ifstream csvFile(csvFileName);

        string line;
        // Read each line from the CSV file
        while (getline(csvFile, line)) {

            // Parse the line and create a Record object
            stringstream ss(line);
            string item;
            vector<string> fields;
            
            while (getline(ss, item, ',')) {
                fields.push_back(item);
            }
            cout << fields[0] <<  endl;
            Record record(fields);
            int hashValue = compute_hash_value(record.id);

            int pageIndex;
            Page page;

            
            if (PageDirectory[hashValue] == -1) {
                // Create a new page
                pageIndex = nextFreePage++;
                PageDirectory[hashValue] = pageIndex;

            } else {
                pageIndex = PageDirectory[hashValue];
                // Load existing page
                ifstream indexIn(fileName, ios::binary | ios::in);
                indexIn.seekg(pageIndex * PAGE_SIZE, ios::beg);
                page.read_from_data_file(indexIn);
                indexIn.close();

            }

            
            addRecordToIndex(pageIndex, page, record);

        }

        // Close the CSV file
        csvFile.close();
    }

    // Function to search for a record by ID in the hash index
    void findAndPrintEmployee(int64_t id) {

        // TODO:
        //  - Compute hash value for the given ID using compute_hash_value() function
        //  - Search for the record in the page corresponding to the hash value using searchRecordByIdInPage() function
        int hashValue = compute_hash_value(id);
        if (PageDirectory[hashValue] == -1) {
            cout << "Employee with ID " << id << " not found (no bucket).\n";
            return;
        }

        int pageIndex = PageDirectory[hashValue];
        searchRecordByIdInPage(pageIndex, id);
        
    }
};

