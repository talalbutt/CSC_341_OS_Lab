/*
 * fsys.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */
#include "fsys.hpp"

// Constructor
FSYS::FSYS() {
  fileTable = FAT();
  load();
}

// Print method
void FSYS::print() {
  cout << this->toString() << endl;
}

// Represent the FAT table as a string
string FSYS::toString() {

}

// Save all file records out to the actual (non-virtual disk)
bool FSYS::save() {

}

// Load files from the physical disk into the virtual disk
bool FSYS::load() {

}

// Index the FAT using string keys (filenames)
FileBuffer FSYS::operator[](const string& key) {
  FAT_Record record;
  for (int i = 0; i < fileTable.size(); i++) {
    if (my_strcasecmp(key, fileTable[i].fileName)) {
      record = fileTable[i];
      break;
    }
  }

  FileBuffer buffer;
  for (int i = 0; i < record.size; i++) {
    buffer.push_back(disk[record.location + i]);
  }

  cout << "FileBuffer size = " << buffer.buffer->size() << endl;

  return buffer;
}
