[OpenTracker](https://erdgeist.org/arts/software/opentracker/)
=======

-------
That should leave you with an exectuable called opentracker and one debug version opentracker.debug.

Some variables in opentracker's Makefile control features and behaviour of opentracker. Here they are:

-DWANT_V6 makes opentracker an IPv6-only tracker. More in the v6-section below.
opentracker can deliver gzip compressed full scrapes. Enable this with -DWANT_COMPRESSION_GZIP option.
Normally opentracker tracks any torrent announced to it. You can change that behaviour by enabling ONE of -DWANT_ACCESSLIST_BLACK or  -DWANT_ACCESSLIST_WHITE. Note, that you have to provide a whitelist file in order to make opentracker do anything in the latter case. More in the closed mode section below.
opentracker can run in a cluster. Enable this behaviour by enabling -DWANT_SYNC_LIVE. Note, that you have to configure your cluster before you can use opentracker when this option is on.
Some statistics opentracker can provide are sensitive. You can restrict access to these statistics by enabling -DWANT_RESTRICT_STATS. See section statistics for more details.
Fullscrapes is bittorrent's way to query a tracker for all tracked torrents. Since it's in the standard, it is enabled by default. Disable it by commenting out -DWANT_FULLSCRAPE.
By default opentracker will only allow the connecting endpoint's IP address to be announced. Bittorrent standard allows clients to provide an IP address in its query string. You can make opentracker use this IP address by enabling -DWANT_IP_FROM_QUERY_STRING.
Some experimental or older, deprecated features can be enabled by the -DWANT_LOG_NETWORKS, -DWANT_SYNC_SCRAPE or -DWANT_IP_FROM_PROXY switch.
Currently there is some packages for some linux distributions and OpenBSD around, but some of them patch Makefile and default config to make opentracker closed by default. I explicitly don't endorse those packages and will not give support for problems stemming from these missconfigurations.


-------
Invocation
opentracker can be run by just typing ./opentracker. This will make opentracker bind to 0.0.0.0:6969 and happily serve all torrents presented to it. If ran as root, opentracker will immediately chroot to . (or any directory given with the -d option) and drop all priviliges after binding to whatever tcp or udp ports it is requested.

When options were few, opentracker used to accept all of them from command line. While this still is possible for most options, using them is quite unhandy: an example invocation would look like ./opentracker -i 23.23.23.7 -p 80 -P 80 -p 6969 -i 23.23.23.8 -p 80 -r http://www.mytorrentsite.com/ -d /usr/local/etc/opentracker -w mytorrents.list -A 127.0.0.1.

opentracker now uses a config file that you can provide with the -f switch.


-------
Config file
opentracker's config file is very straight forward and a very well documented example config can be found in the file opentracker.conf.sample.


``` markdown
yum -y install unzip wget
wget https://github.com/1265578519/OpenTracker/archive/master.zip -O /root/OpenTracker.zip
unzip OpenTracker.zip
cd OpenTracker-master
cd libowfat
make
cd /root/OpenTracker-master
cd opentracker
make

```

``` markdown
./opentracker
```

``` markdown
netstat -apn | grep opentracker
```

Usage: ./opentracker [-i ip] [-p port] [-P port] [-r redirect] [-d dir] [-u user] [-A ip] [-f config] [-s livesyncport]
	-f config include and execute the config file
	-i ip     specify ip to bind to (default: *, you may specify more than one)
	-p port   specify tcp port to bind to (default: 6969, you may specify more than one)
	-P port   specify udp port to bind to (default: 6969, you may specify more than one)
	-r redirecturlspecify url where / should be redirected to (default none)
	-d dir    specify directory to try to chroot to (default: ".")
	-u user   specify user under whose priviliges opentracker should run (default: "nobody")
	-A ip     bless an ip address as admin address (e.g. to allow syncs from this address)

Example:   ./opentracker -i 127.0.0.1 -p 6969 -P 6969 -f ./opentracker.conf -i 10.1.1.23 -p 2710 -p 80

