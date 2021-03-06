#!/bin/sh
# $1: AT cmd, $2: waiting time, $3: forcely use the bulk node.
# environment variable: unit - modem unit.
# echo "This is a script to execute the AT command."


if [ -z "$unit" ] || [ "$unit" -eq "0" ]; then
	prefix="usb_modem_"
else
	prefix="usb_modem${unit}_"
fi

modem_type=`nvram get ${prefix}act_type`
act_node1="${prefix}act_int"
act_node2="${prefix}act_bulk"
modem_vid=`nvram get ${prefix}act_vid`
modem_model=`nvram get modem_model`

atcmd=`nvram get modem_atcmd`

stop_lock=`nvram get stop_atlock`
if [ -n "$stop_lock" ] && [ "$stop_lock" -eq "1" ]; then
	at_lock=""
else
	at_lock="flock -x /tmp/at_cmd_lock"
fi

if [ "$modem_type" == "" -o "$modem_type" == "ecm" -o "$modem_type" == "rndis" -o "$modem_type" == "asix" -o "$modem_type" == "ncm" ] && [ "$modem_model" == "2" -a "$modem_type" != "ncm" ]; then
	exit 0
fi

#at_ret="/tmp/at_ret"

if [ -n "$3" -a "$3" == "bulk" ]; then
	act_node=$act_node2
else
	act_node=$act_node1
fi

modem_act_node=`nvram get $act_node`
if [ -n "$3" -a "$3" == "bulk" -a "$modem_act_node" == "" ]; then
	act_node=$act_node1
	modem_act_node=`nvram get $act_node`
fi

if [ "$modem_act_node" == "" ]; then
	/usr/sbin/find_modem_node.sh

	modem_act_node=`nvram get $act_node`
	if [ "$modem_act_node" == "" ]; then
		echo "Can't get $act_node!"
		exit 1
	fi
fi

if [ -n "$2" ]; then
	waited_sec=$2
else
	waited_sec=3
fi

if [ -z "$1" ]; then
	ATCMD=$1
else
	ATCMD=AT$1
fi

if [ -n "$atcmd" ] && [ "$atcmd" -eq "1" ]; then
	open_tty "$ATCMD"
	ret=$?
else
	#$at_lock chat -t $waited_sec -e '' "$ATCMD" OK >> /dev/$modem_act_node < /dev/$modem_act_node 2>$at_ret
	$at_lock chat -t $waited_sec -e '' "$ATCMD" OK >> /dev/$modem_act_node < /dev/$modem_act_node
	ret=$?
	#cat $at_ret
fi
exit $ret

