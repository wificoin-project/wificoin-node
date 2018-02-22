# Setting up Development Environment

## Install Node.js

Install Node.js by your favorite method, or use Node Version Manager by following directions at https://github.com/creationix/nvm

```bash
nvm install v4
```

## Fork and Download Repositories

To develop wificoin-node:

```bash
cd ~
git clone git@github.com:<yourusername>/wificoin-node.git
git clone git@github.com:<yourusername>/wificoin-lib.git
```

To develop wificoin or to compile from source:

```bash
git clone git@github.com:<yourusername>/wificoin.git
git fetch origin <branchname>:<branchname>
git checkout <branchname>
```
**Note**: See wificoin documentation for building wificoin on your platform.


## Install Development Dependencies

For Ubuntu:
```bash
sudo apt-get install libzmq3-dev
sudo apt-get install build-essential
```
**Note**: Make sure that libzmq-dev is not installed, it should be removed when installing libzmq3-dev.


For Mac OS X:
```bash
brew install zeromq
```

## Install and Symlink

```bash
cd wificoin-lib
npm install
cd ../wificoin-node
npm install
```
**Note**: If you get a message about not being able to download wificoin distribution, you'll need to compile wificoind from source, and setup your configuration to use that version.


We now will setup symlinks in `wificoin-node` *(repeat this for any other modules you're planning on developing)*:
```bash
cd node_modules
rm -rf wificoin-lib
ln -s ~/wificoin-lib
rm -rf wificoind-rpc
ln -s ~/wificoind-rpc
```

And if you're compiling or developing wificoin:
```bash
cd ../bin
ln -sf ~/wificoin/src/wificoind
```

## Run Tests

If you do not already have mocha installed:
```bash
npm install mocha -g
```

To run all test suites:
```bash
cd wificoin-node
npm run regtest
npm run test
```

To run a specific unit test in watch mode:
```bash
mocha -w -R spec test/services/wificoind.unit.js
```

To run a specific regtest:
```bash
mocha -R spec regtest/wificoind.js
```

## Running a Development Node

To test running the node, you can setup a configuration that will specify development versions of all of the services:

```bash
cd ~
mkdir devnode
cd devnode
mkdir node_modules
touch wificoin-node.json
touch package.json
```

Edit `wificoin-node.json` with something similar to:
```json
{
  "network": "livenet",
  "port": 3001,
  "services": [
    "wificoind",
    "web",
    "insight-api",
    "insight-ui",
    "<additional_service>"
  ],
  "servicesConfig": {
    "wificoind": {
      "spawn": {
        "datadir": "/home/<youruser>/.wificoin",
        "exec": "/home/<youruser>/wificoin/src/wificoind"
      }
    }
  }
}
```

**Note**: To install services [insight-api](https://github.com/bitpay/insight-api) and [insight-ui](https://github.com/bitpay/insight-ui) you'll need to clone the repositories locally.

Setup symlinks for all of the services and dependencies:

```bash
cd node_modules
ln -s ~/wificoin-lib
ln -s ~/wificoin-node
ln -s ~/insight-api
ln -s ~/insight-ui
```

Make sure that the `<datadir>/wificoin.conf` has the necessary settings, for example:
```
server=1
whitelist=127.0.0.1
txindex=1
addressindex=1
timestampindex=1
spentindex=1
zmqpubrawtx=tcp://127.0.0.1:28332
zmqpubhashblock=tcp://127.0.0.1:28332
rpcallowip=127.0.0.1
rpcuser=wificoin
rpcpassword=local321
```

From within the `devnode` directory with the configuration file, start the node:
```bash
../wificoin-node/bin/wificoin-node start
```