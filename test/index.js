/*global describe,it*/

var assert = require("assert");
const { mutexAccess } = require("../");
var lib = require("../");

describe("CreateMutex", function () {
  it("should create mutex", function () {
    const handle = lib.createMutex("Local\\TestMutex");

    assert.ok(handle);

    lib.closeMutex(handle);
  });
});

describe("OpenMutex", function () {
  it("should create and open mutex", function () {
    const cHandle = lib.createMutex("Local\\TestMutex");
    const oHandle = lib.openMutex(
      "Local\\TestMutex",
      lib.mutexAccess.Synchronize
    );

    assert.ok(cHandle);
    assert.ok(oHandle);

    lib.closeMutex(oHandle);
    lib.closeMutex(cHandle);
  });
});

describe("WaitReleaseMutex", function () {
  it("should create and wait/release mutex", function () {
    const handle = lib.createMutex("Local\\TestMutex");

    assert.ok(handle);

    lib.waitMutex(handle, lib.waitTime.Infinite);
    lib.releaseMutex(handle);

    lib.closeMutex(handle);
  });
});

describe("MultipleWaitReleaseMutex", function () {
  it("should create, open and wait/release mutex by multiple instances", function () {
    const cHandle = lib.createMutex("Local\\TestMutex");
    const oHandle = lib.openMutex(
      "Local\\TestMutex",
      lib.mutexAccess.Synchronize
    );

    assert.ok(cHandle);
    assert.ok(oHandle);

    lib.waitMutex(oHandle, lib.waitTime.Infinite);
    lib.releaseMutex(oHandle);

    lib.waitMutex(cHandle, lib.waitTime.Infinite);
    lib.releaseMutex(cHandle);

    lib.closeMutex(oHandle);
    lib.closeMutex(cHandle);
  });
});
