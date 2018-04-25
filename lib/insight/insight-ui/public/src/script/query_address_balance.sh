#!/bin/sh

CLI="/opt/wificoin/wificoind/src/wificoin-cli"
ADDRESS_LOG="./wfc_all_addresses"
BALANCE_LOG="./wfc_rich_list.json"

export BLOCKCOUNT=$($CLI getblockcount)
export START

# 获取上次结束时的区块高度
if [ -f "$ADDRESS_LOG" ]; then
	grep -q "block height" $ADDRESS_LOG || {
		echo "Unavailable log, path: $ADDRESS_LOG, you can remove it for rebuild"
		exit 1
	}

	START=$(head wfc_all_addresses -n 1 | awk '{print $3}')
else
	echo 'block height 1' >> $ADDRESS_LOG
	START=1
fi

export ADDRESS_COUNT=$(wc "$ADDRESS_LOG" -l | awk '{print $1}')

echo "Scan addresses from block."
echo "Starting block height $START"

# 遍历区块
for i in $(seq $START $BLOCKCOUNT); do
	blockhash=$($CLI getblockhash $i)
	blocktxs=$($CLI getblock $blockhash | jq -r '.tx[]')

	echo -e "Block:\t$i\t$blockhash\tRemaining: $(expr $BLOCKCOUNT - $i)"
	for tx in $blocktxs; do
		#echo $tx
		vout=$($CLI getrawtransaction $tx 1 | jq '.vout')
		voutnum=$(echo $vout | grep -o "valueSat"| wc -l)

		for((n=0; n<$voutnum; n++)); do
			value=$(echo $vout | jq .[$n].value)
			[ "$value" != '0' ] && {
				addresses=$(echo $vout | jq .[$n].scriptPubKey.addresses[])			
				grep -q "$addresses" $ADDRESS_LOG || {
					echo $addresses >> $ADDRESS_LOG
					echo "Find a new address: $addresses"
					let ADDRESS_COUNT=$ADDRESS_COUNT+1
				}
			}
		done
	done
done

sed -i "1s/.*/block height $BLOCKCOUNT/g" $ADDRESS_LOG

get_address_banlance() {
	echo $($CLI getaddressbalance {\"addresses\":[$1]} | jq -r .balance)
}

:> $BALANCE_LOG
echo "Get address balance..."
echo -e "{\"runtime\":$(date +%s),\"list\":[" >> $BALANCE_LOG
cat $ADDRESS_LOG | while read line; do
	balance=$(get_address_banlance $line)
	echo -e "$ADDRESS_COUNT\t$line\t$(get_address_banlance $line)"
	[ -n $balance -a "$balance" != '0' ] && echo -e "{\"address\":$line,\"balance\":$(get_address_banlance $line)}," >> $BALANCE_LOG
	
	let ADDRESS_COUNT=$ADDRESS_COUNT-1
done
echo -e "{\"address\":\"test\",\"balance\":0}]}" >> $BALANCE_LOG
echo "Done. The query result is stored in $BALANCE_LOG"
