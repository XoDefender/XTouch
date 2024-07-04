#!/bin/bash

# Check if script is run as root
if [ "$(id -u)" -ne 0 ]; then
  echo "This script must be run with sudo or as root"
  exit 1
fi

sudo mysql << EOF
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '030748qwe';
CREATE DATABASE IF NOT EXISTS xtouch;
USE xtouch;
source ./xtouch.sq;
EOF

# Check if MySQL commands executed successfully
if [ $? -eq 0 ]; then
  echo "MySQL commands executed successfully"
else
  echo "Error: MySQL commands execution failed"
fi