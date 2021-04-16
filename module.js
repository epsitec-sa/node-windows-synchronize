const mutexAddon = require("./build/Release/mutex");

function executeCommand(serviceName, cmd) {
  const res = addon.execute_command(serviceName, cmd);

  if (res === -1) {
    throw "could not open service manager";
  } else if (res === -2) {
    throw `could not connect to service ${serviceName}`;
  } else if (res === -3) {
    throw `sending command ${cmd} to service ${serviceName} failed`;
  } else if (res > 0) {
    throw `sending command ${cmd} to service ${serviceName} failed with error code ${res}`;
  }
}

module.exports.mutex = {
  executeCommand,
};
