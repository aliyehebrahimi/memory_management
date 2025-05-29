# Hash Indexing for Employee Records

## Overview

This project implements a **hash-based indexing system** in C++ to efficiently store and retrieve employee records from external storage using fixed-size pages. It simulates a basic DBMS-style indexing system, handling variable-length records and overflow pages using limited memory.

## Features

- Reads employee data from a CSV file (`Employee.csv`)
- Stores data in binary format using 4KB page structures
- Uses **hash indexing (`id % 2^16`)** to locate records quickly
- Supports **variable-length** string fields (`name` and `bio`)
- Manages **overflow pages** when hash collisions occur
- Supports **efficient record retrieval** using serialized formats and a slot directory
- Limits main memory use to **one page and a page directory** during indexing and querying

## File Structure

- `main.cpp`: Main driver (provided separately)
- `classes.h`: Contains the core implementation:
  - `Record` class: Represents an employee record
  - `Page` class: Represents a 4KB fixed-size page
  - `HashIndex` class: Manages the indexing and searching
- `Employee.csv`: Input file containing employee records
- `EmployeeIndex.dat`: Output binary index file

## Data Model

Each employee record contains:

- `id` (int64_t): Employee ID
- `name` (string): Variable-length name (up to 200 characters)
- `bio` (string): Variable-length biography (up to 500 characters)
- `manager_id` (int64_t): ID of the employee's manager

## Page Format

- **Size:** 4096 bytes (4KB)
- **Record area:** Grows from the beginning of the page
- **Slot directory:** Grows from the end of the page
- **Overflow pointer:** Stored in the last 4 bytes of the page
- **Serialization delimiter:** `$` used to mark the beginning of each record

## Hash Function

```cpp
h(id) = id % 65536
```
- Maps an employee ID to one of 65,536 possible buckets
- `PageDirectory` stores a mapping from hash values to page indices

## How to Compile and Run Indexer
### Compile
`
g++ -std=c++17 -o indexer main.cpp
`

### Run
`
./indexer 11432121 11432119 11432159
`

This will:

Read Employee.csv
Build EmployeeIndex.dat
Search and print records for the specified IDs

## Output
The program prints details for each requested employee ID, if found, in the following format:
Employee found:
    ID: 11432121
    NAME: John Doe
    BIO: Software engineer with 10 years of experience.
    MANAGER_ID: 11432000

If not found:
Employee with ID 11432159 not found.

## Memory Limitation
To simulate external memory operations:

Only one page and a page directory are allowed in memory at any time.
Do not cache or preload the entire file or multiple pages into memory.

## References

Database Systems: The Complete Book (a.k.a. "Cow Book")
Sections 9.6.2 and 9.7.2


Course lectures on:
Fixed-length and variable-length records
Page structure
External memory hash indexing


## Author
Aliyeh Ebrahimi
Oregon State University
CS 540 – Database Management Systems



