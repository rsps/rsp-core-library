#!/bin/bash
#

is_user_root() { [ "${EUID:-$(id -u)}" -eq 0 ]; }

SUDO=''
if ! is_user_root; then
    SUDO='sudo'
fi

$SUDO killall wpa_supplicant

$SUDO systemctl unmask wpa_supplicant.service
$SUDO systemctl start wpa_supplicant.service
$SUDO systemctl restart NetworkManager.service

