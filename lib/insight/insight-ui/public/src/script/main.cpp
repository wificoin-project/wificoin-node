#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <unordered_map>

#include "wfcclient.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

#include "threadpool.h"

using namespace std;
using namespace boost;
using namespace jsonrpc;

#define WFC_RPC_SERVER "http://test:admin@192.168.2.119:9665"

unordered_map<string, double> addressList;
boost::mutex addressMutex;

static unsigned long block_height = 248290;
static bool stoped = false;

bool findAddress(const std::string &address)
{
    const auto &it = addressList.find(address);
    if (it != addressList.end()) {
        return true;
    }
    return false;
}

void parse_block_round(const unsigned long &blk_index, bool &error)
{
    error = false;
    HttpClient httpclient(WFC_RPC_SERVER);
    wfcClient client(httpclient, JSONRPC_CLIENT_V1);
    try {
        std::string hash = client.getblockhash(blk_index);
        Json::Value result = client.getblock(hash, 1);
        if (!result.isMember("tx") || !result["tx"].isArray()) {
            cout << ("block index: " + std::to_string(blk_index) + " hash: " + hash + " Bad block");
            return;
        }

        // cout <<result.toStyledString()<< endl;

        const Json::Value &tx = result["tx"];
        int tx_i, tx_size = tx.size();
        cout << "blk_index: " << blk_index << "\t" << tx_size << endl;
        for (tx_i = 0; tx_i < tx_size; tx_i++) {
            std::string tx_hash = tx[tx_i].asString();
            Json::Value txinfo = client.getrawtransaction(tx_hash, true);

//            cout << tx_hash << "\t" << txinfo.toStyledString() << endl;
#if 1
            if (txinfo.isMember("vout") && txinfo["vout"].isArray()) {
                const Json::Value &vout = txinfo["vout"];
                int vout_j, vout_size = vout.size();
                for (vout_j = 0; vout_j < vout_size; vout_j++) {
                    if (!vout[vout_j].isMember("scriptPubKey") || !vout[vout_j]["scriptPubKey"].isObject() ||
                        !vout[vout_j]["scriptPubKey"].isMember("addresses") ||
                        !vout[vout_j]["scriptPubKey"]["addresses"].isArray()) {
                        // cerr << "Error transaction:" <<  vout[vout_j].toStyledString() << endl;
                        continue;
                    }
                    const Json::Value &addr = vout[vout_j]["scriptPubKey"]["addresses"];
                    for (int idx = 0; idx < addr.size(); idx++) {
                        std::string address = addr[idx].asString();
                        cout << address << endl;
                        if (!findAddress(address)) {
                            mutex::scoped_lock lock(addressMutex);
                            addressList.insert(std::pair<string, double>(address, 0));
                        }
                    }
                }
            }
#endif
        }

    } catch (...) {
        return;
    }
    error = true;
}

void thread_detect_address()
{
    HttpClient httpclient(WFC_RPC_SERVER);
    wfcClient client(httpclient, JSONRPC_CLIENT_V1);
    while (!stoped) {
        boost::this_thread::sleep(boost::posix_time::seconds(3));
        unsigned long now_block_height = block_height;
        try {
            now_block_height = static_cast<unsigned long>(client.getblockcount());
            cout << now_block_height << endl;
        } catch (JsonRpcException &e) {
            cout << "getblockcount: " << e.GetMessage() << endl;
            continue;
        }

        tools::threadpool &tpool = tools::threadpool::getInstance();
        unsigned long threads = tpool.get_max_concurrency();
        unsigned long rounds_size = now_block_height - block_height;
        unsigned long blk_index;

        if (threads > 1) {
            for (blk_index = 0; blk_index < rounds_size; blk_index += threads) {
                if (stoped) {
                    return;
                }
                std::deque<bool> error(threads);
                unsigned long blk_height[threads];
                unsigned long thread_size = std::min(threads, rounds_size - blk_index);
                tools::threadpool::waiter waiter;
                for (int j = 0; j < thread_size; ++j) {
                    blk_height[j] = block_height + blk_index + j;
                    tpool.submit(&waiter,
                                 boost::bind(&parse_block_round, std::cref(blk_height[j]), std::ref(error[j])));
                }
                waiter.wait();
                for (int j = 0; j < thread_size; ++j) {
                    if (!error[j]) {
                        cerr << "parse_block_round failed: block: " << blk_height[j] << endl;
                    }
                }
            }

        } else {
            for (blk_index = 0; blk_index < rounds_size; blk_index += threads) {
                if (stoped) {
                    return;
                }

                bool ret;
                parse_block_round(blk_index, ret);
                if (!ret) {
                    cerr << "parse_block_round failed: block: " << blk_index << endl;
                }
            }
        }

        block_height = now_block_height;
        cout << __FUNCTION__ << endl;
    }
}

void thread_detect_address_balance()
{
    HttpClient httpclient(WFC_RPC_SERVER);
    wfcClient client(httpclient, JSONRPC_CLIENT_V1);
    while (!stoped) {
        boost::this_thread::sleep(boost::posix_time::seconds(10));
        mutex::scoped_lock lock(addressMutex);
        Json::Value data;
        data["runtime"] = Json::UInt64(time(NULL));
        Json::Value amounts;
        for (auto &address : addressList) {
            Json::Value item;
            address.second = 0;
            try {
                Json::Value result = client.getbalance(address.first);
                cout << __FUNCTION__ << ":" << address.first << "\t" << result.toStyledString() << endl;
            } catch (JsonRpcException &e) {
                cout << "getbalance: " << address.first << "failed: " << e.GetMessage() << endl;
            }

            item["address"] = address.first;
            item["balance"] = address.second;
            amounts.append(item);
        }
        data["list"] = amounts;
        cout << __FUNCTION__ << endl;
        cout << data.toStyledString() << endl;

        ofstream ofs;
        ofs.open("wfc_rich_list.json");
        ofs << data.toStyledString() << endl;
        ofs.close();
    }
}

int main(int argc, char *argv[])
{
    boost::thread t1(thread_detect_address);
    boost::thread t2(thread_detect_address_balance);

    t1.join();
    t2.join();

    cout << "Hello World!" << endl;
    return 0;
}
