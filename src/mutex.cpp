#include <node_api.h>
#include <napi-macros.h>

#include <windows.h>
#include <sddl.h>
#include <string>

struct MutexHandle
{
  HANDLE hMutex;
};

// string name, string sddl, MutexHandle* mutexHandle -> int
NAPI_METHOD(CreateMutex)
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
NAPI_METHOD(OpenMutex)
{
  int result = 0;

  NAPI_ARGV(3)

  NAPI_ARGV_UTF8(objectNameA, 1000, 0)
  NAPI_ARGV_INT32(mutexAccess, 1)
  NAPI_ARGV_BUFFER_CAST(struct MutexHandle *, mutexHandle, 2)

  mutexHandle->hHandle = OpenMutex(mutexAccess, FALSE, objectNameA);
  if (mutexHandle->hHandle == NULL)
  {
    result = GetLastError();
  }

  NAPI_RETURN_INT32(result)
}

// SharedMemoryHandle* memoryHandle, byte* data, int dataSize -> int
NAPI_METHOD(WriteSharedData)
{
  int result = 0;

  NAPI_ARGV(3)

  NAPI_ARGV_BUFFER_CAST(struct SharedMemoryHandle *, memoryHandle, 0)
  NAPI_ARGV_BUFFER_CAST(char *, data, 1)
  NAPI_ARGV_INT32(dataSize, 2)

  if (dataSize > memoryHandle->size)
  {
    result = 1;
    NAPI_RETURN_INT32(result)
  }

  RtlMoveMemory((PVOID)memoryHandle->pBuf, data, dataSize);

  NAPI_RETURN_INT32(result)
}

// SharedMemoryHandle* memoryHandle, byte* data, int dataSize -> int
NAPI_METHOD(ReadSharedData)
{
  int result = 0;

  NAPI_ARGV(3)

  NAPI_ARGV_BUFFER_CAST(struct SharedMemoryHandle *, memoryHandle, 0)
  NAPI_ARGV_BUFFER_CAST(char *, data, 1)
  NAPI_ARGV_INT32(dataSize, 2)

  if (dataSize > memoryHandle->size)
  {
    result = 1;
    NAPI_RETURN_INT32(result)
  }

  RtlMoveMemory(data, (PVOID)memoryHandle->pBuf, memoryHandle->size);

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
  NAPI_EXPORT_FUNCTION(CreateMutex)
  NAPI_EXPORT_FUNCTION(OpenMutex)
  NAPI_EXPORT_FUNCTION(WriteSharedData)
  NAPI_EXPORT_FUNCTION(ReadSharedData)
  NAPI_EXPORT_FUNCTION(CloseMutex)

  NAPI_EXPORT_SIZEOF_STRUCT(MutexHandle)
  NAPI_EXPORT_ALIGNMENTOF(MutexHandle)
}