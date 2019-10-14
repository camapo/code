//C++ program that indicates whether user-entered year is a) a four-digit number, and b) a leap year.
#include <iostream>

int main() {
  
  int year;
  
  std::cout << "\nInput year: ";
  std::cin >> year;

	if(year < 1000 || year > 9999) {
  
  	std::cout << "Invalid entry. Please try again.\n\n"; 
	}
	else if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {

  	std::cout << year;
    std::cout << " falls on a leap year.\n\n"; 
	}
	else {
  
    std::cout << year;
  	std::cout << " does not fall on a leap year.\n\n";
	}
}
