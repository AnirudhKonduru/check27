#!/bin/bash
echo "Thank you for installing check27!"
echo "Setting up..."
sudo rm -r /usr/share/check27
sudo mkdir /usr/share/check27
sudo cp -r ./progs/* /usr/share/check27/
tree /usr/share/check27
make check27
sudo cp check27 /usr/bin
