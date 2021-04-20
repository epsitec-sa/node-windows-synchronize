#include <node_api.h>
#include <napi-macros.h>

#include <windows.h>
#include <sddl.h>
#include <synchapi.h>
#include <string>

struct MutexHandle
{
  HANDLE hMutex;
};

// string name, string sddl, MutexHandle* mutexHandle -> int
NAPI_METHOD(CreateMutexNW)
{
  int result = 0;

  NAPI_ARGV(3)

  NAPI_ARGV_UTF8(objectNameA, 1000, 0)
  NAPI_ARGV_UTF8(sddlStringA, 1000, 1)
  NAPI_ARGV_BUFFER_CAST(struct MutexHandle *, mutexHandle, 2)

  LPSECURITY_ATTRIBUTES pAttributes = NULL;
  SECURITY_ATTRIBUTES attributes;

  if (strcmp(sddlStringA, "") != 0)
  {
    ZeroMemory(&attributes, sizeof(attributes));
    attributes.nLength = sizeof(attributes);
    ConvertStringSecurityDescriptorToSecurityDescriptor(
      sddlStringA,
      SDDL_REVISION_1,
      &attributes.lpSecurityDescriptor,
      NULL);
  }

  mutexHandle->hMutex = CreateMutex(pAttributes, FALSE, objectNameA);
  if (mutexHandle->hMutex == NULL)
  {
    result = GetLastError();
  }

  NAPI_RETURN_INT32(result)
}

// string name, int mutexAccess, MutexHandle* mutexHandle -> int
NAPI_METHOD(OpenMutexNW)
{
  int result = 0;

  NAPI_ARGV(3)

  NAPI_ARGV_UTF8(objectNameA, 1000, 0)
  NAPI_ARGV_INT32(mutexAccess, 1)
  NAPI_ARGV_BUFFER_CAST(struct MutexHandle *, mutexHandle, 2)

  mutexHandle->hMutex = OpenMutex(mutexAccess, FALSE, objectNameA);
  if (mutexHandle->hMutex == NULL)
  {
    result = GetLastError();
  }

  NAPI_RETURN_INT32(result)
}

// MutexHandle* mutexHandle, int waitTimeMs -> int
NAPI_METHOD(WaitMutex)
{
  int result = 0;
  DWORD res = 0;

  NAPI_ARGV(2)

  NAPI_ARGV_BUFFER_CAST(struct MutexHandle *, mutexHandle, 0)
  NAPI_ARGV_INT32(waitTimeMs, 1)

  res = WaitForSingleObject(mutexHandle->hMutex, waitTimeMs);
  if (res == WAIT_FAILED)
  {
    result = GetLastError();
  }
  else if (res == WAIT_TIMEOUT)
  {
    result = -1;
  }
  else if (res == WAIT_ABANDONED)
  {
    result = -2;
  }

  NAPI_RETURN_INT32(result)
}

// MutexHandle* mutexHandle -> int
NAPI_METHOD(ReleaseMutexNW)
{
  int result = 0;
  DWORD res = 0;

  NAPI_ARGV(1)

  NAPI_ARGV_BUFFER_CAST(struct MutexHandle *, mutexHandle, 0)

  res = ReleaseMutex(mutexHandle->hMutex);
  if (res == 0)
  {
    result = GetLastError();
  }

  NAPI_RETURN_INT32(result)
}

// MutexHandle* mutexHandle -> int
NAPI_METHOD(CloseMutex)
{
  int result = 0;

  NAPI_ARGV(1)

  NAPI_ARGV_BUFFER_CAST(struct MutexHandle *, mutexHandle, 0)

  CloseHandle(mutexHandle->hMutex);

  NAPI_RETURN_INT32(result)
}


NAPI_INIT()
{
  NAPI_EXPORT_FUNCTION(CreateMutexNW)
  NAPI_EXPORT_FUNCTION(OpenMutexNW)
  NAPI_EXPORT_FUNCTION(WaitMutex)
  NAPI_EXPORT_FUNCTION(ReleaseMutexNW)
  NAPI_EXPORT_FUNCTION(CloseMutex)

  NAPI_EXPORT_SIZEOF_STRUCT(MutexHandle)
  NAPI_EXPORT_ALIGNMENTOF(MutexHandle)
}