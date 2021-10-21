#include <pybind11/pybind11.h>
#define SECRET_API_SUBJECT_TO_CHANGE
#include <libsecret/secret.h>
#include <string>
namespace py = pybind11;

static const SecretSchema schema = {
    "org.freedesktop.Secret.Generic",
    SECRET_SCHEMA_NONE,
    {{"service", SECRET_SCHEMA_ATTRIBUTE_STRING},
     {"account", SECRET_SCHEMA_ATTRIBUTE_STRING}}};

bool setPassword(std::string &service, std::string &account,
                 std::string &password) {
  GError *error = NULL;
  secret_password_store_sync(&schema, SECRET_COLLECTION_DEFAULT,
                             (service + "/" + account).c_str(),
                             password.c_str(), NULL, &error, "service",
                             service.c_str(), "account", account.c_str(), NULL);
  if (error != NULL) {
    std::string err = std::string(error->message);
    g_error_free(error);
    throw std::invalid_argument(err.c_str());
  }
  return true;
}

std::string getPassword(std::string &service, std::string &account)

{
  GError *error = NULL;
  gchar *raw_password = secret_password_lookup_sync(
      &schema, NULL, &error, "service", service.c_str(), "account",
      account.c_str(), NULL);
  if (error != NULL && error->message != NULL) {
    std::string err = std::string(error->message);
    g_error_free(error);
    throw std::invalid_argument(err.c_str());
  }
  if (raw_password == NULL) {
    throw std::invalid_argument("password not found");
  }
  std::string password = std::string(raw_password);
  secret_password_free(raw_password);
  return password;
}

bool deletePassword(std::string &service, std::string &account)

{
  GError *error = NULL;
  gboolean result = secret_password_clear_sync(&schema, NULL, &error, "service",
                                               service.c_str(), "account",
                                               account.c_str(), NULL);
  if (error != NULL) {
    std::string err = std::string(error->message);
    g_error_free(error);
    throw std::invalid_argument(err.c_str());
  }
  if (!result) {
    throw std::invalid_argument("password not found");
  }
  return true;
}

py::object findPassword(std::string &service) {
  GError *error = NULL;

  gchar *raw_password = secret_password_lookup_sync(
      &schema, NULL, &error, "service", service.c_str(), NULL);
  if (error != NULL || raw_password == NULL) {
    return py::none();
  }
  std::string password = std::string(raw_password);
  secret_password_free(raw_password);
  return py::str(password);
}

py::object findCredentials(std::string &service) {
  GError *error = NULL;

  GHashTable *attributes = g_hash_table_new(NULL, NULL);
  g_hash_table_replace(attributes, (gpointer) "service",
                       (gpointer)service.c_str());

  GList *items = secret_service_search_sync(
      NULL, &schema, attributes,
      static_cast<SecretSearchFlags>(SECRET_SEARCH_ALL | SECRET_SEARCH_UNLOCK |
                                     SECRET_SEARCH_LOAD_SECRETS),
      NULL, &error);

  g_hash_table_destroy(attributes);
  if (error != NULL) {
    std::string err = std::string(error->message);
    g_error_free(error);
    throw std::invalid_argument(err.c_str());
  }
  GList *current = items;
  py::list credentials;
  for (current = items; current != NULL; current = current->next) {
    SecretItem *item = reinterpret_cast<SecretItem *>(current->data);

    GHashTable *itemAttrs = secret_item_get_attributes(item);
    char *account = strdup(
        reinterpret_cast<char *>(g_hash_table_lookup(itemAttrs, "account")));

    SecretValue *secret = secret_item_get_secret(item);
    char *password = strdup(secret_value_get_text(secret));

    if (account == NULL || password == NULL) {
      if (account)
        free(account);

      if (password)
        free(password);

      continue;
    }
    py::dict credential;
    credential["account"] = account;
    credential["password"] = password;
    credentials.append(credential);
    free(account);
    free(password);
  }
  return credentials;
}

PYBIND11_MODULE(_pycreds, m) {
  m.def("set_password", &setPassword, "Function to set password.");
  m.def("get_password", &getPassword, "Function to get password.");
  m.def("find_password", &findPassword, "Function to find password.");
  m.def("delete_password", &deletePassword, "Function to delete password.");
  m.def("find_credentials", &findCredentials, "Function to find credentials.");
}
