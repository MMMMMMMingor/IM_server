#include "../src/message/addressbook.pb.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person *person) {
  person->set_id(1);

  person->set_name("mingor");

  person->set_email("mmmmingor@gmail.com");

  tutorial::Person::PhoneNumber *phone_number = person->add_phones();
  phone_number->set_number("8888888");
  phone_number->set_type(tutorial::Person::MOBILE);
}

int main(int argc, char *argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
    return -1;
  }

  tutorial::AddressBook address_book;

  {
    // Read the existing address book.
    fstream input(argv[1], ios::in | ios::binary);
    if (!input) {
      cout << argv[1] << ": File not found.  Creating a new file." << endl;
    } else if (!address_book.ParseFromIstream(&input)) {
      cerr << "Failed to parse address book." << endl;
      return -1;
    }
  }

  // Add an address.
  PromptForAddress(address_book.add_people());

  {
    // Write the new address book back to disk.
    fstream output(argv[1], ios::out | ios::trunc | ios::binary);
    if (!address_book.SerializeToOstream(&output)) {
      cerr << "Failed to write address book." << endl;
      return -1;
    }
  }

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  std::cout << "finished" << std::endl;

  return 0;
}