#ifdef _MSC_VER
#include "pycreds_win.hpp"
#elif defined(__linux__)
#include "pycreds_posix.hpp"
#elif defined(__APPLE__)
#include "pycreds_darwin.hpp"
#endif

PYBIND11_MODULE(_pycreds, m)
{
    m.def("set_password", &setPassword, "Function to set password.");
    m.def("get_password", &getPassword, "Function to get password.");
    m.def("find_password", &findPassword, "Function to find password.");
    m.def("delete_password", &deletePassword, "Function to delete password.");
    m.def("find_credentials", &findCredentials, "Function to find credentials.");
}
