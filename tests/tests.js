const Eos           = require('eosjs'),
      expect        = require('chai').expect,
      NodeosProcess = require('./NodeosProcess');

const nodeosOpts = "--config-dir=./nodeos/config --data-dir=./nodeos/data --resync-blockchain";
const nodeosProcess = new NodeosProcess(nodeosOpts);


describe("Polleos smart contract", function () {
  before(async function() {
    await nodeosProcess.launch();
    console.log("Polleos launched successfully");
  });

  after(async function() {
    await nodeosProcess.stop();
  })

  it("should pass", function () {
    console.log("Test passing");
  })

  it("should fail", function () {
    console.log("Failing");
    expect(false, "false != true").to.equal(true);
  })

})


