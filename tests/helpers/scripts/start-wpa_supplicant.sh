#!/bin/bash
#

is_user_root() { [ "${EUID:-$(id -u)}" -eq 0 ]; }

SUDO=''
if ! is_user_root; then
    SUDO='sudo'
fi

INTERFACE=`nmcli --get-values GENERAL.DEVICE,GENERAL.TYPE device show | awk '/^wifi$/{print dev; next};{dev=$0};'`

$SUDO systemctl mask wpa_supplicant.service
$SUDO systemctl stop wpa_supplicant.service

$SUDO wpa_supplicant -B -i $INTERFACE -c /etc/wpa_supplicant/wpa_supplicant.conf

