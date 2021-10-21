#include <Security/Security.h>
#include <pybind11/pybind11.h>
#include <string>
const std::string CFStringToStdString(CFStringRef cfstring) {
  const char *cstr = CFStringGetCStringPtr(cfstring, kCFStringEncodingUTF8);

  if (cstr != NULL) {
    return std::string(cstr);
  }

  CFIndex length = CFStringGetLength(cfstring);
  CFIndex cstrPtrLen = length * 2 + 1;
  char *cstrPtr = static_cast<char *>(malloc(cstrPtrLen));

  Boolean result =
      CFStringGetCString(cfstring, cstrPtr, cstrPtrLen, kCFStringEncodingUTF8);

  std::string stdstring;
  if (result) {
    stdstring = std::string(cstrPtr);
  }

  free(cstrPtr);

  return stdstring;
}

const std::string errorStatusToString(OSStatus status) {
  std::string errorStr;
  CFStringRef errorMessageString = SecCopyErrorMessageString(status, NULL);

  const char *errorCStringPtr =
      CFStringGetCStringPtr(errorMessageString, kCFStringEncodingUTF8);
  if (errorCStringPtr) {
    errorStr = std::string(errorCStringPtr);
  } else {
    errorStr = std::string("An unknown error occurred.");
  }

  CFRelease(errorMessageString);
  return errorStr;
}
namespace py = pybind11;

bool setPassword(std::string &service, std::string &account,
                 std::string &password) {
  OSStatus status = SecKeychainAddGenericPassword(
      NULL, service.length(), service.data(), account.length(), account.data(),
      password.length(), password.data(), NULL);
  if (status != errSecSuccess) {
    throw std::invalid_argument(errorStatusToString(status));
  }
  return true;
}

std::string getPassword(std::string &service, std::string &account) {
  void *data;
  UInt32 length;
  OSStatus status = SecKeychainFindGenericPassword(
      NULL, service.length(), service.data(), account.length(), account.data(),
      &length, &data, NULL);
  if (status != errSecSuccess) {
    throw std::invalid_argument(errorStatusToString(status));
  }
  std::string password =
      std::string(reinterpret_cast<const char *>(data), length);
  SecKeychainItemFreeContent(NULL, data);
  return password;
}

bool deletePassword(std::string &service, std::string &account)

{
  SecKeychainItemRef item;
  OSStatus status = SecKeychainFindGenericPassword(
      NULL, service.length(), service.data(), account.length(), account.data(),
      NULL, NULL, &item);
  if (status != errSecSuccess) {
    throw std::invalid_argument(errorStatusToString(status));
  }
  status = SecKeychainItemDelete(item);
  CFRelease(item);
  if (status != errSecSuccess) {
    throw std::invalid_argument(errorStatusToString(status));
  }
  return true;
}

py::object findPassword(std::string &service) {
  SecKeychainItemRef item;
  void *data;
  UInt32 length;

  OSStatus status = SecKeychainFindGenericPassword(
      NULL, service.length(), service.data(), 0, NULL, &length, &data, &item);
  if (status != errSecSuccess) {
    return py::none();
  }
  std::string password =
      std::string(reinterpret_cast<const char *>(data), length);
  SecKeychainItemFreeContent(NULL, data);
  CFRelease(item);
  return py::str(password);
}

py::dict getCredentialsForItem(CFDictionaryRef item) {
  CFStringRef service =
      (CFStringRef)CFDictionaryGetValue(item, kSecAttrService);
  CFStringRef account =
      (CFStringRef)CFDictionaryGetValue(item, kSecAttrAccount);

  CFMutableDictionaryRef query =
      CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,
                                &kCFTypeDictionaryValueCallBacks);

  CFDictionaryAddValue(query, kSecAttrService, service);
  CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
  CFDictionaryAddValue(query, kSecMatchLimit, kSecMatchLimitOne);
  CFDictionaryAddValue(query, kSecReturnAttributes, kCFBooleanTrue);
  CFDictionaryAddValue(query, kSecReturnData, kCFBooleanTrue);
  CFDictionaryAddValue(query, kSecAttrAccount, account);

  py::dict cred;
  CFTypeRef result = NULL;
  OSStatus status = SecItemCopyMatching((CFDictionaryRef)query, &result);

  CFRelease(query);

  if (status == errSecSuccess) {
    CFDataRef passwordData = (CFDataRef)CFDictionaryGetValue(
        (CFDictionaryRef)result, CFSTR("v_Data"));
    CFStringRef password = CFStringCreateFromExternalRepresentation(
        NULL, passwordData, kCFStringEncodingUTF8);

    cred["account"] = CFStringToStdString(account);
    cred["password"] = CFStringToStdString(password);

    CFRelease(password);
  }

  if (result != NULL) {
    CFRelease(result);
  }

  return cred;
}

py::object findCredentials(std::string &service)

{

  CFStringRef serviceStr =
      CFStringCreateWithCString(NULL, service.c_str(), kCFStringEncodingUTF8);

  CFMutableDictionaryRef query =
      CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,
                                &kCFTypeDictionaryValueCallBacks);
  CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
  CFDictionaryAddValue(query, kSecAttrService, serviceStr);
  CFDictionaryAddValue(query, kSecMatchLimit, kSecMatchLimitAll);
  CFDictionaryAddValue(query, kSecReturnRef, kCFBooleanTrue);
  CFDictionaryAddValue(query, kSecReturnAttributes, kCFBooleanTrue);

  CFTypeRef result = NULL;
  OSStatus status = SecItemCopyMatching((CFDictionaryRef)query, &result);

  CFRelease(serviceStr);
  CFRelease(query);
  if (status == errSecSuccess) {
    CFArrayRef resultArray = (CFArrayRef)result;
    int resultCount = CFArrayGetCount(resultArray);
    py::list credentials;
    for (int idx = 0; idx < resultCount; idx++) {
      CFDictionaryRef item =
          (CFDictionaryRef)CFArrayGetValueAtIndex(resultArray, idx);

      py::dict credential = getCredentialsForItem(item);
      credentials.append(credential);
    }
    return credentials;
  }
  return py::list();
}

PYBIND11_MODULE(_pycreds, m) {

  m.def("set_password", &setPassword, "Function to set password.");
  m.def("get_password", &getPassword, "Function to get password.");
  m.def("find_password", &findPassword, "Function to find password.");
  m.def("delete_password", &deletePassword, "Function to delete password.");
  m.def("find_credentials", &findCredentials, "Function to find credentials.");
}
