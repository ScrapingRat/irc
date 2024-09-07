#!/bin/bash

# Replace with your server's IP address and port
SERVER_IP="127.0.0.1"
SERVER_PORT="6667"

# Number of times to connect and send EOF
NUM_CONNECTIONS=15

for ((i=1; i<=NUM_CONNECTIONS; i++))
do
	sleep 0.5
	echo "Connection $i"
	{
		exec {fd}<>/dev/tcp/$SERVER_IP/$SERVER_PORT
		if [[ $? -ne 0 ]]; then
			echo "Failed to connect to $SERVER_IP:$SERVER_PORT"
			exit 1
		fi
		echo "PING " >&$fd
		while read -r -u $fd response; do
			echo "Client $i received response: $response"
		done
		exec {fd}>&-
	} &
done

# Wait for all background jobs to finish
wait