#!/bin/sh

while true; do
  request_string="GET /announce?info_hash=012345678901234567\
$(printf %02X $(( $RANDOM & 0xff )) )\
&ip=$(( $RANDOM & 0xff )).17.13.15&port=$(( $RANDOM & 0xff )) HTTP/1.0"

#  echo $request_string
#  echo
  printf "%s\n\n" "$request_string" | nc 84.200.61.9 6969 | hexdump -C

  request_string="GET /announce?info_hash=012345678901234567\
$(printf %02X $(( $RANDOM & 0xff )) )\
&ip=2001:1608:6:27::$(( $RANDOM & 0xff ))&port=$(( $RANDOM & 0xff )) HTTP/1.0"
  printf "%s\n\n" "$request_string" | nc 2001:1608:6:27::9 6969 | hexdump -C
  printf "%s\n\n" "$request_string"

  request_string="GET /scrape?info_hash=012345678901234567\
$(printf %02X $(( $RANDOM & 0xff )) ) HTTP/1.0"
  printf "%s\n\n" "$request_string" | nc 2001:1608:6:27::9 6969 | hexdump -C

done
