import subprocess
import platform

# Linuxの場合
def get_ip_address_linux():
    try:
        result = subprocess.run(
            ["ip", "-4", "addr", "show"],
            capture_output=True,
            text=True,
            check=True
        )
        output = result.stdout
        for line in output.split('\n'):
            if 'inet ' in line and '127.0.0.1' not in line:
                ip_address = line.split()[1].split('/')[0]
                return ip_address
    except subprocess.CalledProcessError as e:
        return f"Error retrieving IP address: {e}"

# macOSの場合
def get_ip_address_mac():
    try:
        result = subprocess.run(
            ["ipconfig", "getifaddr", "en0"],
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        return f"Error retrieving IP address: {e}"

# ipアドレスを取得
def get_ip_address():
    os_name = platform.system()
    if os_name == "Linux":
        return get_ip_address_linux()
    elif os_name == "Darwin":
        return get_ip_address_mac()
    else:
        return "Unsupported OS"
