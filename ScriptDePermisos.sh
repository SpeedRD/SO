SCRIPT:

#!/bin/bash

# PASO 1: Crear grupos de usuarios en Linux
sudo groupadd ufvauditor
sudo groupadd ufvauditores

# PASO 2: Crear usuarios y asignarlos a los grupos
sudo useradd -m -G ufvauditor userSU001
sudo useradd -m -G ufvauditor userSU002
sudo useradd -m -G ufvauditor userSU003
sudo useradd -m -G ufvauditor userSU004

sudo useradd -m userfp
sudo useradd -m usermonitor

sudo usermod -aG ufvauditores userfp
sudo usermod -aG ufvauditores usermonitor

# PASO 3: Establecer permisos en la estructura de archivos
cd /ruta/a/tus/ficheros

touch SU001 SU002 SU003 SU004
chmod 640 SU001 SU002 SU003 SU004

# Establecer permisos específicos para los usuarios de las sucursales
sudo chown userSU001:ufvauditor SU001
sudo chmod 640 SU001

sudo chown userSU002:ufvauditor SU002
sudo chmod 640 SU002

sudo chown userSU003:ufvauditor SU003
sudo chmod 640 SU003

sudo chown userSU004:ufvauditor SU004
sudo chmod 640 SU004

# Establecer permisos para el grupo ufvauditores en todos los ficheros y directorios
sudo chgrp ufvauditores -R /ruta/a/tus/ficheros
sudo chmod 750 -R /ruta/a/tus/ficheros

# Asignar permisos especiales a FileProcessor y Monitor para userfp
sudo chown userfp:ufvauditores FileProcessor Monitor
sudo chmod 750 FileProcessor Monitor

# Asignar permisos especiales a Monitor para usermonitor
sudo chown usermonitor:ufvauditores Monitor
sudo chmod 750 Monitor
