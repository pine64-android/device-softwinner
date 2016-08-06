#!/sbin/busybox sh

BLOCK_PATH=/dev/block
BUSYBOX_PATH=/sbin/busybox

${BUSYBOX_PATH} mkdir -m 755 ${BLOCK_PATH}/by-name
for line in `$BUSYBOX_PATH cat /proc/cmdline`
do
	if [ ${line%%=*} = 'partitions' ] ; then
		partitions=${line##*=}
		part=" "
		while [ "$part" != "$partitions" ]
		do
			part=${partitions%%:*}
			$BUSYBOX_PATH ln -s "${BLOCK_PATH}/${part#*@}" "${BLOCK_PATH}/by-name/${part%@*}"
            if [ ${part%@*} = "misc" ] ; then
                $BUSYBOX_PATH chown 0.1000 "${BLOCK_PATH}/${part#*@}"
                $BUSYBOX_PATH chmod 0660 "${BLOCK_PATH}/${part#*@}"
            fi
			partitions=${partitions#*:}
		done
	fi
done
