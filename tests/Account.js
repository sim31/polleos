const Eos    = require('eosjs');
const ecc    = Eos.modules.ecc;
const assert = require('chai').assert;

let eos;
let keyProvider = ['5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'];

class Account {

  constructor(privateKey, publicKey, name) {
    this.privateKey = privateKey;
    this.publicKey  = publicKey;
    this.name       = name;

    keyProvider.push(this.privateKey);
  }

  create(creator = "eosio") {
    return eos.newaccount(creator, this.name, this.publicKey, this.publicKey);
  }

  static init(httpEndpoint) {
    eos = Eos.Localnet( {httpEndpoint, keyProvider} );
  }

  static async generateAccount(name) {
    assert(name.length > 0, "Account name generation not implemented");
    let privKey = await ecc.unsafeRandomKey();
    let pubKey  = ecc.privateToPublic(privKey);
    return new Account(privKey, pubKey, name);
  }

  static async generateAndCreateAcc(name) {
    let acc = await this.generateAccount(name);
    await acc.create();
    return acc;
  }

  static getKeyProvider() {
    return keyProvider;
  }
}

module.exports = Account;
