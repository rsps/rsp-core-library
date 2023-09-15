#!/bin/bash
#

is_user_root() { [ "${EUID:-$(id -u)}" -eq 0 ]; }

SUDO=''
if ! is_user_root; then
    SUDO='sudo'
fi

$SUDO systemctl mask wpa_supplicant.service
$SUDO systemctl stop wpa_supplicant.service

$SUDO wpa_supplicant -B -i wlx200db0144e41 -c /etc/wpa_supplicant/wpa_supplicant.conf
