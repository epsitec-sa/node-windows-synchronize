const mutexAddon = require("./build/Release/mutex");

function createMutex(name, sddlString) {
  const handle = Buffer.alloc(mutexAddon.sizeof_MutexHandle);

  const res = mutexAddon.CreateMutex(name, sddlString || "", handle);

  if (res !== 0) {
    throw `could not create mutex for object ${name}: ${res}`;
  }

  return handle;
}

function openMutex(name, mutexAccess) {
  const handle = Buffer.alloc(mutexAddon.sizeof_MutexHandle);

  const res = mutexAddon.OpenMutex(name, mutexAccess, handle);

  if (res !== 0) {
    throw `could not open mutex for object ${name}: ${res}`;
  }

  return handle;
}

function writeSharedData(handle, data, encoding) {
  let buf = null;

  if (isBuffer(data)) {
    buf = data;
  } else if (data && typeof data === "string") {
    buf = Buffer.from(data, encoding || "utf8");
  } else {
    buf = Buffer.from(data);
  }

  const res = sharedMemoryAddon.WriteSharedData(handle, buf, buf.byteLength);

  if (res === 1) {
    throw `data size (${data.length()}) exceeded maximum shared memory size`;
  }
}

function readSharedData(handle, encoding, bufferSize) {
  const dataSize = bufferSize || sharedMemoryAddon.GetSharedMemorySize(handle);
  const buf = Buffer.alloc(dataSize);

  const res = sharedMemoryAddon.ReadSharedData(handle, buf, dataSize);

  if (res === 1) {
    throw `data size (${data.length()}) exceeded maximum shared memory size`;
  }

  if (encoding) {
    // is a string
    return buf.toString(encoding).replace(/\0/g, ""); // remove trailing \0 characters
  }

  return buf;
}

function closeMutex(handle) {
  mutexAddon.CloseMutex(handle);
}

module.exports = {
  createMutex,
  openMutex,
  writeSharedData,
  readSharedData,
  closeMutex,

  mutexAccess: {
    Delete: 0x00010000,
    ReadControl: 0x00020000,
    Synchronize: 0x00100000,
    WriteDac: 0x00040000,
    WriteOwner: 0x00080000,
  },
};
