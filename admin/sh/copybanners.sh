#!/usr/bin/expect -f

spawn rsync -vaz /srv/web/backoffice/admin/media/banners/ developer@10.15.0.20:/srv/web/banners/system_com/
expect "Enter passphrase for key '/home/developer/.ssh/id_dsa': "
send "84756dev65748\r"
expect "total size is "

spawn rsync -vaz /srv/web/backoffice/admin/media/banners/ developer@10.15.0.20:/srv/web/banners/system_lga/
expect "Enter passphrase for key '/home/developer/.ssh/id_dsa': "
send "84756dev65748\r"
expect "total size is "
