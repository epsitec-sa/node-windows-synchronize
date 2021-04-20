const mutexAddon = require("./build/Release/mutex");

function createMutex(name, sddlString) {
  const handle = Buffer.alloc(mutexAddon.sizeof_MutexHandle);

  const res = mutexAddon.CreateMutexNW(name, sddlString || "", handle);

  if (res !== 0) {
    throw `could not create mutex for object ${name}: ${res}`;
  }

  return handle;
}

function openMutex(name, mutexAccess) {
  const handle = Buffer.alloc(mutexAddon.sizeof_MutexHandle);

  const res = mutexAddon.OpenMutexNW(name, mutexAccess, handle);

  if (res !== 0) {
    throw `could not open mutex for object ${name}: ${res}`;
  }

  return handle;
}

function waitMutex(handle, waitTimeMs) {
  const res = mutexAddon.WaitMutex(handle, waitTimeMs);

  if (res === -1) {
    throw `mutex timeout expired`;
  } else if (res === -2) {
    // Abandoned, should try to wait again
    waitMutex(handle, waitTimeMs);
  } else if (res !== 0) {
    throw `could not wait for mutex: ${res}`;
  }
}

function releaseMutex(handle) {
  const res = mutexAddon.ReleaseMutexNW(handle);

  if (res !== 0) {
    throw `could not release mutex: ${res}`;
  }
}

function closeMutex(handle) {
  mutexAddon.CloseMutex(handle);
}

module.exports = {
  createMutex,
  openMutex,
  waitMutex,
  releaseMutex,
  closeMutex,

  mutexAccess: {
    Delete: 0x00010000,
    ReadControl: 0x00020000,
    Synchronize: 0x00100000,
    WriteDac: 0x00040000,
    WriteOwner: 0x00080000,
  },
  waitTime: {
    Infinite: 0xffffffff,
  },
};
