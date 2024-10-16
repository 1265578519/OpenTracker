
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


-------
Closed mode
While personally I like my tracker to be open, I can see that there's people that want to control what torrents to track – or not to track. If you've compiled opentracker with one of the accesslist-options (see Build instructions above), you can control which torrents are tracked by providing a file that contains a list of human readable info_hashes. An example whitelist file would look like

0123456789abcdef0123456789abcdef01234567
890123456789abcdef0123456789abcdef012345
To make opentracker reload it's white/blacklist, send a SIGHUP unix signal.


-------
Statistics
Given its very network centric approach, talking to opentracker via http comes very naturally. Besides the /announce and /scrape paths, there is a third path you can access the tracker by: /stats. This request takes parameters, for a quick overview just inquire /stats?mode=everything`.

Statistics have grown over time and are currently not very tidied up. Most modes were written to dump legacy-SNMP-style blocks that can easily be monitored by MRTG. These modes are: peer, conn, scrp, udp4, tcp4, busy, torr, fscr, completed, syncs. I'm not going to explain these here.

The statedump mode dumps non-recreatable states of the tracker so you can later reconstruct an opentracker session with the -l option. This is beta and wildly undocumented.

You can inquire opentracker's version (i.e. CVS versions of all its objects) using the version mode.


下面中文的，方便讲解使用
Centos 6.9 x64位安装说明
``` markdown
yum -y install unzip wget gcc zlib-devel make
wget https://github.com/1265578519/OpenTracker/archive/master.zip -O /root/OpenTracker.zip
unzip OpenTracker.zip
mv OpenTracker-master /home
cd /home
cd OpenTracker-master
cd libowfat
make
cd /home/OpenTracker-master
cd opentracker
make

```

运行程序，并且监听tcp和udp端口的8080，并且自动后台工作
``` markdown
./opentracker -f opentracker.conf.sample -p 8080 -P 8080 &
```

推荐使用8080端口，基本上免备案不会扫描，而且CloudFlare也支持使用CDN隐藏IP来使用。

多端口可以加多个即可
``` markdown
./opentracker -f opentracker.conf.sample -p 8080 -P 8080 -p 6961 -P 6961 -p 2710 -P 2710 &
```

添加开机启动，执行一次即可，否则会启动多个进程。。。可以搭配下方的计划任务自动重启进程，如果centos 7未有killall命令可以用 yum -y install psmisc 安装，centos7要赋予开机启动文件执行权限 chmod +x /etc/rc.d/rc.local;ll /etc/rc.d/rc.local
``` markdown
echo "cd /home/OpenTracker-master;cd opentracker;./opentracker -f opentracker.conf.sample -p 8080 -P 8080 -p 6961 -P 6961 -p 2710 -P 2710 &" >> /etc/rc.d/rc.local
```

输入下方命令可以查看是否在工作中
``` markdown
top -b -n 1 |grep opentracker
```

查看进程当前并发连接数
``` markdown
netstat -apn|grep opentracker |wc -l
```

查看系统当前网络情况
``` markdown
netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'
```

通过浏览器访问程序的统计功能
``` markdown
http://ip:8080/stats
http://ip:8080/stats?mode=everything
http://ip:8080/stats?mode=top100
http://ip:8080/scrape
```

服务器里面可以
``` markdown
curl http://localhost:8080/stats
```

运行crontab -e
可以根据需要使用计划任务自动重启进程，下方意思，每天0点、4点、8点、12点、16点、20点整，自动重启进程。
``` markdown
0 0,4,8,12,16,20 * * * killall -9 opentracker;sleep 3;cd /home/OpenTracker-master;cd opentracker;./opentracker -f opentracker.conf.sample -p 8080 -P 8080 -p 6961 -P 6961 -p 2710 -P 2710 &
```

方法二
使用计划任务自动监控进程是否存在然后重启恢复，不进行定时重启
``` markdown
* * * * * /sbin/pidof opentracker||{ cd /home/OpenTracker-master;cd opentracker;./opentracker -f opentracker.conf.sample -p 8080 -P 8080 -p 6961 -P 6961 -p 2710 -P 2710 &}
```

方式三，推荐替换二，可以代替之前的pidof判断oom杀进程，毕竟触发oom的之前几分钟因为内存不足都无法新建tcp连接，ssh都上不去（控制512M内存的75%是125000，1G内存的75%是250000，避免超过80%）
```markdown
* * * * * [ $(awk '/MemFree/ {free=$2} /Buffers/ {buffers=$2} /^Cached/ {cached=$2} END {print free + buffers + cached}' /proc/meminfo) -lt 125000 ] && { killall -9 opentracker;sleep 3;cd /home/OpenTracker-master;cd opentracker;./opentracker -f opentracker.conf.sample -p 8080 -P 8080 -p 6961 -P 6961 -p 2710 -P 2710 &}
```


软件的自带帮助说明
``` markdown
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
```

间隔可以在编译前进行修改，默认我改成了2小时，以便降低服务器宽带开销
``` markdown
trackerlogic.h:#define OT_CLIENT_TIMEOUT_SEND (60*15)#(60*30)，客户端最小间隔请求时间，客户端可能不会遵守
trackerlogic.h:#define OT_CLIENT_REQUEST_INTERVAL (60*30)#(60*120)，客户端默认间隔请求时间
trackerlogic.h:#define OT_CLIENT_REQUEST_VARIATION (60*6)，服务端下发随机客户端间隔请求时间调整，提高性能，默认允许误差随机6分钟内，保持默认无修改
trackerlogic.h:#define OT_PEER_TIMEOUT 45#180，服务端删除peer时间，单位分钟
```


utorrent中制作种子过程tracker写
http://服务器ip:8080/announce

udp://服务器ip:8080/announce


反映：http://bbs.itzmx.com/thread-18214-1-1.html
-------
程序来自官网
https://erdgeist.org/arts/software/opentracker/
mail:erdgeist@erdgeist.org

推荐Tracker服务器购买优惠注册地址：
https://www.vultr.com/?ref=6813695
https://justhost.ru/services/vps/tariffs/13?ref=75001

注：2024年2月27日更新一次版本代码改动
centos6仅支持libowfat 0.31，centos7仅支持libowfat 0.32，请使用对应系统的版本编译opentracker，上方提供的安装代码当前版本为libowfat 0.32
centos6在使用 cd libowfat 进入目录的上方插入并且运行这段代码
```
rm -rf libowfat;mv libowfat-0.31 libowfat
```

有关Linux内核优化参数
只需要调整打开文件数，和禁用系统防火墙避免lsmod加载nf模块引起高CPU占用和触发丢包，其它的参数就微乎其微了
1.打开文件数
```
ulimit -n 1048576
echo "* soft nofile 1048576" >> /etc/security/limits.conf
echo "* hard nofile 1048576" >> /etc/security/limits.conf
```
2.ksoftirqd内核进程占用高CPU的优化，禁用防火墙和确认lsmod中不存在加载nf驱动模块
centos7关闭防火墙并且禁用nf驱动模块，对于其它系统的关闭方法请使用搜索引擎
```
yum -y install iptables-services;service ip6tables stop;chkconfig ip6tables off;service iptables stop;chkconfig iptables off;systemctl stop firewalld;systemctl disable firewalld
```
关闭防火墙后，通过 lsmod 指令确认驱动模块加载情况，不能有包含"nf"开头的模块
3.如果使用ipv6网络，需要设置最大连接数，默认值4096，改为最大值不限制
```
echo "net.ipv6.route.max_size = 2147483647" >> /etc/sysctl.conf
```
然后执行 /sbin/sysctl -p 让参数生效
4.给予更多的socket端口号，默认值4096，1G内存设置为100000，这个对访问影响不大一般来说可以保持默认值
```
echo "net.ipv4.tcp_max_orphans = 100000" >> /etc/sysctl.conf
```
然后执行 /sbin/sysctl -p 让参数生效
以上优化中打开文件和防火墙关闭禁用nf驱动模块最为重要，对于防火墙给予了lsmod查看办法，下面这是确认打开文件设置成功的查看办法
根据软件的pid进行排序显示
```
yum -y install lsof
lsof -n | awk '{ print $2; }' | uniq -c | sort -rn | head
```
查看单个pid进程的限制打开文件大小最大数量，可以查看修改系统打开文件数量后是否生效了
```
cat /proc/28492/limits | grep files
```
虽然实际上ulimit限制显示是1024，但是Linux应该是用4字节作为计算，也就是4124达到阈值，修改打开文件后就可以突破4124，修改成功后两个数值都会显示为1048576

如果你用的是其他操作系统，Debian系统设置limits打开文件
https://bbs.itzmx.com/thread-110212-1-1.html

一个可选内存优化，禁用AnonHugePages释放更多可用内存给予进程
https://bbs.itzmx.com/thread-110231-1-1.html