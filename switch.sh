#!/bin/bash
#
# stty 115200 baud on the specified serial interface and
# pass the command
#
set -o errexit

die() {
	echo "$@" >&2
	exit 1
}

usage() {
	cat <<EOF
Select the UART switch to SOC or USB debug
Usage:

	$0 [-t <tty>] <command>

where command may be 0, 1 or 2

        0 - None
        1 - SOC On
        2 - USB Debug On

-t is switch device serial console, usually /dev/cu.usbmodem<something>

EOF
}

while getopts "t:h" OPTION; do
    case $OPTION in
		t)
			SW_TTY=${OPTARG}
			;;
		*)
			usage
			exit 2
			;;
	esac
done
shift $((OPTIND-1))

if [ "$#" = 0 ]; then
	die "No command given, try running with -h"
fi

cmd="$1"

case "$cmd" in
	[012]) true ;;
	*) die "Unknown command: '$cmd', try running with -h" ;;
esac

if [ "$SW_TTY" = "" ]; then
	cnt=0
	for D in /dev/cu.usbmodem*; do
		((cnt++))
		SW_TTY="$D"
	done
	if [[ "$cnt" != 1 ]]; then
		die 'Cannot guess devices TTY, please supply it with -t TTY parameter'
	fi
fi

# setting baud speed to 115200 and passing the command
# opening tty on file descriptor 3 to avoid closing tty after baud set operation
exec 3>$SW_TTY
stty -f $SW_TTY 115200
echo "$cmd" >&3
