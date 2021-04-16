#include <node_api.h>

#include <windows.h>
#include <string>
#include <psapi.h>

// string serviceName, int cmd -> int
napi_value ExecuteCommand(napi_env env, napi_callback_info info)
{
  napi_status status;

  size_t argc = 2;
  napi_value argv[2];

  napi_value napiResult;
  int result = 1;

  char serviceNameA[1000];
  size_t serviceNameARead = 0;
  int cmd = 0;

  SC_HANDLE managerHandle;
  SC_HANDLE serviceHandle;

  SERVICE_STATUS   controlParms;
  DWORD retStatus;


  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
  }

  status = napi_get_value_string_utf8(env, argv[0], serviceNameA, 1000, &serviceNameARead);

  if (status != napi_ok) {
    printf("---%d---", (int)status);
    napi_throw_error(env, NULL, "Invalid serviceName was passed as argument");
  }

  status = napi_get_value_int32(env, argv[1], &cmd);

  if (status != napi_ok) {
    printf("---%d---", (int)status);
    napi_throw_error(env, NULL, "Invalid cmd was passed as argument");
  }

  managerHandle = OpenSCManager(NULL, NULL, GENERIC_READ);
  if (managerHandle != NULL)
  {
      serviceHandle = OpenService(managerHandle, serviceNameA, SERVICE_USER_DEFINED_CONTROL | SERVICE_QUERY_STATUS);

      if (serviceHandle != NULL)
      {
          retStatus = ControlService(serviceHandle, cmd, &controlParms);

          if (retStatus)
          {
              // Get the return code from the service
              result = (int)controlParms.dwWin32ExitCode;
          }
          else
          {
              result = -3;
          }

          CloseServiceHandle(serviceHandle);
      }
      else
      {
        result = -2;
      }

      CloseServiceHandle(managerHandle);
  }
  else
  {
      result = -1;
  }

  
  status = napi_create_int32(env, result, &napiResult);

  if (status != napi_ok) {
    printf("---%d---", (int)status);
    napi_throw_error(env, NULL, "Unable to create return value");
  }

  return napiResult;
}


napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, NULL, 0, ExecuteCommand, NULL, &fn);
  if (status != napi_ok) {
    printf("---%d---", (int)status);
    napi_throw_error(env, NULL, "Unable to wrap ExecuteCommand native function");
  }

  status = napi_set_named_property(env, exports, "execute_command", fn);
  if (status != napi_ok) {
    printf("---%d---", (int)status);
    napi_throw_error(env, NULL, "Unable to populate execute_command exports");
  }

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)