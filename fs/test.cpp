// Class to define the properties
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

class Employee
{
public:
    std::string Name;
    int Employee_ID;
    int Salary;
};

int main()
{
    Employee Emp_1;
    Emp_1.Name = "John";
    Emp_1.Employee_ID = 2121;
    Emp_1.Salary = 11000;

    Employee Emp_1;
    Emp_1.Name = "Smith";
    Emp_1.Employee_ID = 112434;
    Emp_1.Salary = 125000;

    // Writing this data to Employee.txt
    std::ofstream file1;
    file1.open("Employee.txt", std::ios::app);
    file1.write((char *)&Emp_1, sizeof(Emp_1));
    file1.close();

    // Reading data from EMployee.txt
    std::ifstream file2;
    file2.open("Employee.txt", std::ios::in);
    file2.seekg(0);
    file2.read((char *)&Emp_1, sizeof(Emp_1));
    cout << Emp_1.Name << endl;
    cout << Emp_1.Employee_ID << endl;
    cout << Emp_1.Salary << endl;
    file2.close();
    return 0;
}