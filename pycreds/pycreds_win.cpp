#include <pybind11/pybind11.h>
#define UNICODE
#include <string>
#include <windows.h>
#include <wincred.h>

LPWSTR utf8ToWideChar(std::string utf8) {
  int wide_char_length =
      MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
  if (wide_char_length == 0) {
    return NULL;
  }

  LPWSTR result = new WCHAR[wide_char_length];
  if (MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, result,
                          wide_char_length) == 0) {
    delete[] result;
    return NULL;
  }

  return result;
}

std::string wideCharToAnsi(LPWSTR wide_char) {
  if (wide_char == NULL) {
    return std::string();
  }

  int ansi_length =
      WideCharToMultiByte(CP_ACP, 0, wide_char, -1, NULL, 0, NULL, NULL);
  if (ansi_length == 0) {
    return std::string();
  }

  char *buffer = new char[ansi_length];
  if (WideCharToMultiByte(CP_ACP, 0, wide_char, -1, buffer, ansi_length, NULL,
                          NULL) == 0) {
    delete[] buffer;
    return std::string();
  }

  std::string result = std::string(buffer);
  delete[] buffer;
  return result;
}

std::string wideCharToUtf8(LPWSTR wide_char) {
  if (wide_char == NULL) {
    return std::string();
  }

  int utf8_length =
      WideCharToMultiByte(CP_UTF8, 0, wide_char, -1, NULL, 0, NULL, NULL);
  if (utf8_length == 0) {
    return std::string();
  }

  char *buffer = new char[utf8_length];
  if (WideCharToMultiByte(CP_UTF8, 0, wide_char, -1, buffer, utf8_length, NULL,
                          NULL) == 0) {
    delete[] buffer;
    return std::string();
  }

  std::string result = std::string(buffer);
  delete[] buffer;
  return result;
}

std::string getErrorMessage(DWORD errorCode) {
  LPWSTR errBuffer;
  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL, errorCode, 0, (LPWSTR)&errBuffer, 0, NULL);
  std::string errMsg = wideCharToAnsi(errBuffer);
  LocalFree(errBuffer);
  return errMsg;
}
namespace py = pybind11;

bool setPassword(std::string &service, std::string &account,
                 std::string &password) {
  LPWSTR target_name = utf8ToWideChar(service + '/' + account);
  LPWSTR user_name = utf8ToWideChar(account);
  CREDENTIAL cred = {0};
  cred.Type = CRED_TYPE_GENERIC;
  cred.TargetName = target_name;
  cred.UserName = user_name;
  cred.CredentialBlobSize = password.size();
  cred.CredentialBlob = (LPBYTE)(password.data());
  cred.Persist = CRED_PERSIST_ENTERPRISE;
  bool result = ::CredWrite(&cred, 0);
  delete[] target_name;
  if (!result) {
    throw std::invalid_argument(getErrorMessage(::GetLastError()).c_str());
  }
  return true;
}

std::string getPassword(std::string &service, std::string &account) {
  LPWSTR target_name = utf8ToWideChar(service + '/' + account);
  CREDENTIAL *cred;
  bool result = ::CredRead(target_name, CRED_TYPE_GENERIC, 0, &cred);
  delete[] target_name;
  if (!result) {
    throw std::invalid_argument(getErrorMessage(::GetLastError()).c_str());
  }
  std::string password = std::string(
      reinterpret_cast<char *>(cred->CredentialBlob), cred->CredentialBlobSize);
  ::CredFree(cred);
  return password;
}

bool deletePassword(std::string &service, std::string &account) {
  LPWSTR target_name = utf8ToWideChar(service + "/" + account);
  bool result = ::CredDelete(target_name, CRED_TYPE_GENERIC, 0);
  delete[] target_name;
  if (!result) {
    throw std::invalid_argument(getErrorMessage(::GetLastError()).c_str());
  }
  return true;
}

py::object findPassword(std::string &service) {
  LPWSTR filter = utf8ToWideChar(service + "*");
  DWORD count;
  CREDENTIAL **creds;
  bool result = ::CredEnumerate(filter, 0, &count, &creds);
  delete[] filter;
  if (!result) {
    return py::none();
  }
  std::string password =
      std::string(reinterpret_cast<char *>(creds[0]->CredentialBlob),
                  creds[0]->CredentialBlobSize);
  ::CredFree(creds);
  return py::str(password);
}

py::object findCredentials(std::string &service) {
  LPWSTR filter = utf8ToWideChar(service + "*");
  DWORD count;
  CREDENTIAL **creds;
  bool result = ::CredEnumerate(filter, 0, &count, &creds);
  delete[] filter;
  if (!result) {
    return py::list();
  }
  py::list credentials;
  for (DWORD i = 0; i < count; ++i) {
    CREDENTIAL *cred = creds[i];
    if (cred->UserName == NULL || cred->CredentialBlobSize == NULL) {
      continue;
    }
    py::dict credential;
    std::string username = wideCharToUtf8(cred->UserName);
    std::string password(reinterpret_cast<char *>(cred->CredentialBlob),
                         cred->CredentialBlobSize);
    credential["account"] = username;
    credential["password"] = password;
    credentials.append(credential);
  }
  ::CredFree(creds);
  return credentials;
}

PYBIND11_MODULE(_pycreds, m) {

  m.def("set_password", &setPassword, "Function to set password.");
  m.def("get_password", &getPassword, "Function to get password.");
  m.def("find_password", &findPassword, "Function to find password.");
  m.def("delete_password", &deletePassword, "Function to delete password.");
  m.def("find_credentials", &findCredentials, "Function to find credentials.");
}
