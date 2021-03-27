import subprocess
# Get 0.0.0 version from latest Git tag
tagcmd = "git describe --tags"
version = subprocess.check_output(tagcmd, shell=True).decode().strip()
print(f'\'-D FIRMWAREVERSION="{version}"\'')
