Build the dependencies
=================
1. libjson-rpc-cpp
------------------------

- [libjson-rpc-cpp](https://github.com/cinemast/libjson-rpc-cpp/releases/tag/v0.7.0)  v0.7.0 is recommend.

**Debian (stretch) and Ubuntu (15.10 or later)**

```sh
sudo apt-get install libjsonrpccpp-dev libjsonrpccpp-tools
```


2. boost only depend on libboost-thread and libboost-system
----------------------------
- [boost](https://www.boost.org/)
 

 **Debian (stretch) and Ubuntu (15.10 or later)**
```sh
sudo apt-get install libboost-thread-dev libboost-system-dev
```


Build
-----
```sh
git clone https://github.com/wificoin-project/wificoin-node.git
mkdir -p wificoin-node/tree/master/lib/insight/insight-ui/public/src/script/build
cd wificoin-node/tree/master/lib/insight/insight-ui/public/src/script/build
cmake .. && make
```

## License
This framework is licensed under [MIT](http://en.wikipedia.org/wiki/MIT_License).
All of this libraries dependencies are licensed under MIT compatible licenses.
