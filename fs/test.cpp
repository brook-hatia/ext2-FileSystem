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

    friend std::ostream &operator<<(std::ostream &os, const Employee &emp)
    {
        os << emp.Name << endl;
        os << emp.Employee_ID << endl;
        os << emp.Salary << endl;
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Employee &emp)
    {
        is >> emp.Name;
        is >> emp.Employee_ID;
        is >> emp.Salary;
        return is;
    }
};

int main()
{
    Employee Emp_1;
    Emp_1.Name = "John";
    Emp_1.Employee_ID = 2121;
    Emp_1.Salary = 11000;

    Employee Emp_2;
    Emp_2.Name = "Smith";
    Emp_2.Employee_ID = 112434;
    Emp_2.Salary = 125000;

    // Writing this data to Employee.txt
    std::ofstream file1;
    file1.open("Employee.txt", std::ios::app);
    file1 << Emp_1;
    file1.close();

    // Reading data from Employee.txt
    std::ifstream file2;
    file2.open("Employee.txt", std::ios::in);
    file2 >> Emp_1;
    cout << Emp_1.Name << endl;
    cout << Emp_1.Employee_ID << endl;
    cout << Emp_1.Salary << endl;

    file2 >> Emp_2;
    cout << Emp_2.Name << endl;
    cout << Emp_2.Employee_ID << endl;
    cout << Emp_2.Salary << endl;
    file2.close();
    return 0;
}