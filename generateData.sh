#!/bin/bash

# Función para mostrar el uso del script
usage() {
    echo "Uso: $0 [-l | --lines] <No. Líneas> [-s | --sucursal] <Sucursal> [-t | --tipo] <Tipo de Operación> [-u | --usuarios] <No. Usuarios>"
    exit 1
}

# Analizar los argumentos largos y cortos
TEMP=`getopt -o l:s:t:u: --long lines:,sucursal:,tipo:,usuarios: -- "$@"`
eval set -- "$TEMP"

# Inicializar variables
l=""
s=""
t=""
u=""

# Extraer opciones y sus argumentos en bucle.
while true; do
    case "$1" in
        -l|--lines)
            l=$2; shift 2;;
        -s|--sucursal)
            s=$2; shift 2;;
        -t|--tipo)
            t=$2; shift 2;;
        -u|--usuarios)
            u=$2; shift 2;;
        --)
            shift; break;;
        *)
            usage;;
    esac
done

# Verificar que se hayan proporcionado todos los argumentos
if [ -z "$l" ] || [ -z "$s" ] || [ -z "$t" ] || [ -z "$u" ]; then
    usage
fi

# Crear archivo de datos
filename="${s}_${t}_$(date +%d%m%Y)_1.data"
if [ -f "$filename" ]; then
    i=1
    while [ -f "${s}_${t}_$(date +%d%m%Y)_${i}.data" ]; do
        let i++
    done
    filename="${s}_${t}_$(date +%d%m%Y)_${i}.data"
fi

echo "IdOperacion;FECHA_INICIO;FECHA_FIN;IdUsuario;IdTipoOperacion;NoOperacion;Importe;Estado" > $filename

for (( i=1; i<=$l; i++ )); do
    usuario=$(printf "USER%03d" $((RANDOM % u + 1)))
    importe=$(($RANDOM % 101 - 50))
    estado=$(($RANDOM % 2 == 0 ? "Correcto" : "Error"))
    echo "OPE00$i;02/02/2022 15:25;02/02/2022 15:50;$usuario;$t;$i;$importe€;$estado" >> $filename
done

echo "Datos generados en el archivo: $filename"
