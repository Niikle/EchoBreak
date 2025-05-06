import os
import subprocess
import sys
import ipaddress
import socket
import time

# Define the networks to scan
NETWORKS = ["172.17.212.0/24", "172.17.213.0/24"]


def ping_ip(ip):
    """Ping an IP address to see if it responds."""
    param = "-n" if os.name == "nt" else "-c"
    command = ["ping", param, "1", str(ip)]
    try:
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=1)
        return result.returncode == 0
    except:
        return False


def get_hostname(ip):
    """Try to resolve hostname from IP."""
    try:
        hostname, _, _ = socket.gethostbyaddr(str(ip))
        return hostname
    except socket.herror:
        return "Unknown"
    except Exception:
        return "Unknown"


def scan_networks():
    """Scan all IPs in defined networks and display IP + Hostname."""
    print("[*] Scanning network hosts...\n")

    all_hosts = []
    total_ips = 0
    start_time = time.time()

    # First, calculate total number of IPs
    for net in NETWORKS:
        network = ipaddress.ip_network(net, strict=False)
        total_ips += sum(1 for _ in network.hosts())

    scanned = 0

    for net in NETWORKS:
        network = ipaddress.ip_network(net, strict=False)
        for ip in network.hosts():
            ip_str = str(ip)
            is_active = ping_ip(ip)
            progress = int((scanned / total_ips) * 100)

            if is_active:
                hostname = get_hostname(ip)
                print(f"[+] [{progress}%] Host active: {ip_str} ({hostname})")
                all_hosts.append({"ip": ip_str, "hostname": hostname, "status": "active"})
            else:
                hostname = "N/A"
                print(f"[ ] [{progress}%] Host inactive: {ip_str}")
                all_hosts.append({"ip": ip_str, "hostname": hostname, "status": "inactive"})

            scanned += 1

    end_time = time.time()
    duration = round(end_time - start_time, 2)

    # Summary
    active_count = len([h for h in all_hosts if h['status'] == 'active'])
    percent_up = round((active_count / total_ips) * 100, 2)

    print("\n[*] Scan completed.")
    print(f"  └─ Total IPs scanned       : {total_ips}")
    print(f"  └─ Active hosts found      : {active_count} ({percent_up:.2f}%)")
    print(f"  └─ Total scan time         : {duration} seconds\n")

    # Optional: Show only active hosts
    show_all = input("Show all hosts? (y/n): ").strip().lower() == 'y'
    print("\n[+] Host List:\n")
    for host in all_hosts:
        if not show_all and host['status'] != 'active':
            continue
        print(f"{host['ip']} ({host['hostname']}) - {host['status'].upper()}")

    return all_hosts


def install_file():
    """Install eb.net file to /bin directory."""
    source_file = "eb.net"
    dest_path = "/bin/eb.net"

    if not os.path.exists(source_file):
        print(f"[-] File '{source_file}' not found.")
        return

    try:
        # Copy file to /bin
        with open(source_file, 'rb') as src, open(dest_path, 'wb') as dst:
            dst.write(src.read())
        # Make executable
        os.chmod(dest_path, 0o755)
        print(f"[+] Successfully installed '{source_file}' to '{dest_path}'.")
    except Exception as e:
        print(f"[-] Installation failed: {e}")


def main():
    if len(sys.argv) != 2:
        print("Usage: sudo python3 installer.py [scan|install]")
        sys.exit(1)

    command = sys.argv[1].lower()

    if command == "scan":
        scan_networks()
    elif command == "install":
        install_file()
    else:
        print("Invalid command. Use 'scan' or 'install'.")
        sys.exit(1)


if __name__ == "__main__":
    main()
