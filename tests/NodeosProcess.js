const exec = require('child_process').exec;

class NodeosProcess {

  constructor(optionsStr, executable="nodeos") {
    this.cmd    = executable + " " +  optionsStr;
    this.isRunning = false;
    this.process = null;
  }

  launch(logerr = false, logstdout = false) {
    return new Promise( (resolve, reject) => {
      if (this.isRunning)
        reject("This process is already running");

      this.process = exec(this.cmd, (error, stdout, stderr) => {
        this.isRunning = false;

        if (error) {
          console.log("nodeos exited with error: ", error);
        }
        else {
          console.log("nodeos exited cleanly");
        }

        if (logerr)
          console.log("stderr: ", stderr);
        if (logstdout)
          console.log("stdout: ", stdout);
      });

      this.isRunning = true;

      setTimeout(() => {
        if (!this.isRunning)
          reject("nodeos failed to launch");
        else
          resolve();

      }, 3000)
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