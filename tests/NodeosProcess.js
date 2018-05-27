const exec = require('child_process').exec;

class NodeosProcess {

  constructor(optionsStr, executable="nodeos") {
    this.cmd    = executable + " " +  optionsStr;
    this.isRunning = false;
    this.process = null;
  }

  launch() {
    return new Promise( (resolve, reject) => {
      if (this.isRunning)
        reject("This process is already running");

      this.process = exec(this.cmd, (error, stdout, stderr) => {
        this.isRunning = false;

        if (error)
          console.log("nodeos exited with error: ", error);
        else
          console.log("nodeos exited cleanly");
      });

      this.isRunning = true;

      setTimeout(() => {
        if (!this.isRunning)
          reject("nodeos failed to launch");
        else
          resolve();

      }, 1000)
    })
  }

  stop() {
    return new Promise( (resolve, reject) => {
      if (!this.isRunning)
        reject("nodeos already stopped");

      this.process.on("exit", () => {
        resolve();
      })
      this.process.on("error", (err) => {
        reject("Error when trying to stop nodeos: ", err);
      })
      this.process.kill();
    })
  }
}

module.exports = NodeosProcess;