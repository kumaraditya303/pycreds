#include <pybind11/pybind11.h>
#define UNICODE
#include <string>
#include <windows.h>
#include <wincred.h>

std::unique_ptr<wchar_t[]> utf8ToWideChar(std::string utf8)
{
  int wide_char_length =
      MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
  if (wide_char_length == 0)
  {
    return NULL;
  }

  auto result = std::make_unique<wchar_t[]>(wide_char_length);
  if (MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, result.get(),
                          wide_char_length) == 0)
  {
    return NULL;
  }
  return std::move(result);
}

std::string wideCharToAnsi(LPWSTR wide_char)
{
  if (wide_char == NULL)
  {
    return std::string();
  }

  int ansi_length =
      WideCharToMultiByte(CP_ACP, 0, wide_char, -1, NULL, 0, NULL, NULL);
  if (ansi_length == 0)
  {
    return std::string();
  }
  auto buffer = std::make_unique<char[]>(ansi_length);
  if (WideCharToMultiByte(CP_ACP, 0, wide_char, -1, buffer.get(), ansi_length, NULL,
                          NULL) == 0)
  {
    return std::string();
  }
  auto result = std::string(buffer.get());
  return result;
}

std::string wideCharToUtf8(LPWSTR wide_char)
{
  std::string result;
  if (wide_char == NULL)
  {
    return result;
  }

  int utf8_length =
      WideCharToMultiByte(CP_UTF8, 0, wide_char, -1, NULL, 0, NULL, NULL);
  if (utf8_length == 0)
  {
    return result;
  }

  auto buffer = std::make_unique<char[]>(utf8_length);
  if (WideCharToMultiByte(CP_UTF8, 0, wide_char, -1, buffer.get(), utf8_length, NULL,
                          NULL) == 0)
  {
    return result;
  }

  result = std::string(buffer.get());

  return result;
}

std::string getErrorMessage(DWORD errorCode)
{
  LPWSTR errBuffer;
  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL, errorCode, 0, (LPWSTR)&errBuffer, 0, NULL);
  std::string errMsg = wideCharToAnsi(errBuffer);
  LocalFree(errBuffer);
  return errMsg;
}
namespace py = pybind11;

bool setPassword(std::string &service, std::string &account,
                 std::string &password)
{
  auto target_name = utf8ToWideChar(service + '/' + account);
  auto user_name = utf8ToWideChar(account);
  CREDENTIAL cred = {0};
  cred.Type = CRED_TYPE_GENERIC;
  cred.TargetName = target_name.get();
  cred.UserName = user_name.get();
  cred.CredentialBlobSize = (DWORD)password.size();
  cred.CredentialBlob = (LPBYTE)(password.data());
  cred.Persist = CRED_PERSIST_ENTERPRISE;
  bool result = ::CredWrite(&cred, 0);
  if (!result)
  {
    throw std::invalid_argument(getErrorMessage(::GetLastError()).c_str());
  }
  return true;
}

std::string getPassword(std::string &service, std::string &account)
{
  auto target_name = utf8ToWideChar(service + '/' + account);
  CREDENTIAL *cred;
  bool result = ::CredRead(target_name.get(), CRED_TYPE_GENERIC, 0, &cred);

  if (!result)
  {
    throw std::invalid_argument(getErrorMessage(::GetLastError()).c_str());
  }
  auto password = std::string(
      reinterpret_cast<char *>(cred->CredentialBlob), cred->CredentialBlobSize);
  ::CredFree(cred);
  return password;
}

bool deletePassword(std::string &service, std::string &account)
{
  auto target_name = utf8ToWideChar(service + "/" + account);
  bool result = ::CredDelete(target_name.get(), CRED_TYPE_GENERIC, 0);
  if (!result)
  {
    throw std::invalid_argument(getErrorMessage(::GetLastError()).c_str());
  }
  return true;
}

py::object findPassword(std::string &service)
{
  auto filter = utf8ToWideChar(service + "*");
  DWORD count;
  CREDENTIAL **creds;
  bool result = ::CredEnumerate(filter.get(), 0, &count, &creds);

  if (!result)
  {
    return py::none();
  }
  auto password = std::string(reinterpret_cast<char *>(creds[0]->CredentialBlob),
                              creds[0]->CredentialBlobSize);
  ::CredFree(creds);
  return py::str(password);
}

py::object findCredentials(std::string &service)
{
  auto filter = utf8ToWideChar(service + "*");
  DWORD count;
  CREDENTIAL **creds;
  bool result = ::CredEnumerate(filter.get(), 0, &count, &creds);
  if (!result)
  {
    return py::list();
  }
  py::list credentials;
  for (DWORD i = 0; i < count; ++i)
  {
    CREDENTIAL *cred = creds[i];
    if (cred->UserName == NULL || cred->CredentialBlobSize == NULL)
    {
      continue;
    }
    py::dict credential;
    auto username = wideCharToUtf8(cred->UserName);
    auto password = std::string(reinterpret_cast<char *>(cred->CredentialBlob),
                                cred->CredentialBlobSize);
    credential["account"] = username;
    credential["password"] = password;
    credentials.append(credential);
  }
  ::CredFree(creds);
  return credentials;
}
