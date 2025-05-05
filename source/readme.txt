to install ebinst:
1. mv EchoBreak/source/ebinst /usr/local/bin
2. chmod +x /usr/local/bin/ebinstl

to run ebinst:
# Scan all devices on both networks
ebinstl scan all

# Scan only devices whose hostname starts with SM
ebinstl scan 114

# Install eb on all discovered hosts
ebinstl install all

# Install eb only on hosts with hostname starting with SM
ebinstl install 114
