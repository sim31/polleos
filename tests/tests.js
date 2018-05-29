const Eos           = require('eosjs'),
      expect        = require('chai').expect,
      binaryen      = require('binaryen');
      fs            = require('fs');
      EosEcc        = Eos.modules.ecc;
      NodeosProcess = require('./NodeosProcess'),
      Account       = require('./Account');


const addr           = "127.0.0.1:8889";
//TODO: update for dawn4.2 and 1.0. --delete-all-blocks instead of resync
const nodeosOpts     = "--config-dir=./nodeos/config --data-dir=./nodeos/data --resync-blockchain" +
  " --http-server-address=" + addr;
const nodeosProcess  = new NodeosProcess(nodeosOpts);
const contractDir    = "../build";
const wastFile       = contractDir + "/polleos.wasm";
const abiFile        = contractDir + "/polleos.abi";
const polleosAccName = "polleos";
var eos, polleos;


describe("Polleos smart contract", function () {
  this.timeout(10000);

  before(async function() {
    await nodeosProcess.launch(true, true);
    console.log("Nodeos launched");

    eos = Eos.Localnet( {
      httpEndpoint: "http://" + addr,
      keyProvider: Account.getKeyProvider(),
      binaryen
    });

    // console.log(Object.getOwnPropertyNames(eos));

    Account.init("http://" + addr);
    polleos = await Account.generateAndCreateAcc(polleosAccName);

    let wasm = fs.readFileSync(wastFile);
    let abi  = fs.readFileSync(abiFile);

    await eos.setcode(polleos.name, 0, 0, wasm)
    await eos.setabi(polleos.name, JSON.parse(abi))

    eos.contract(polleos.name).then(c => console.log(c));
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


