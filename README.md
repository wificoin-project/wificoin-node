Wificoin Node
============

A Wificoin full node for building applications and services with Node.js. A node is extensible and can be configured to run additional services. At the minimum a node has an interface to Wificoind for more advanced address queries. Additional services can be enabled to make a node more useful such as exposing new APIs, running a block explorer and wallet service.

## Install

```bash
git clone https://github.com/KunTengRom/wificoin-node.git
cd wificoin-node
npm install
node app.js start
or
pm2 --name "wificoin-node" start app.js -- start
```

Note: For your convenience, we distribute wificoind binaries for x86_64 Linux and x86_64 Mac OS X. 

## Prerequisites

- GNU/Linux x86_32/x86_64, or OSX 64bit *(for wificoind distributed binaries)*
- Node.js v0.10, v0.12 or v4
- ZeroMQ *(libzmq3-dev for Ubuntu/Debian or zeromq on OSX)*
- ~200GB of disk storage
- ~8GB of RAM

## Documentation

- [Upgrade Notes](docs/upgrade.md)
- [Services](docs/services.md)
- [Wificoind](docs/services/wificoind.md) - Interface to Wificoind
- [Web](docs/services/web.md) - Creates an express application over which services can expose their web/API content

## License

Code released under [the MIT license](https://github.com/KunTengRom/wificoin-node/blob/master/LICENSE).

Copyright 2013-2015 BitPay, Inc.
