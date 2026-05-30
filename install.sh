#!/usr/bin/env bash
set -euo pipefail

URL="https://github.com/HugoVitse/webc/releases/latest/download/webc.zip"
ZIP_NAME="webc.zip"
TARGET_DIR=""

if [ -t 1 ]; then
	RED="\033[31m"
	GREEN="\033[32m"
	YELLOW="\033[33m"
	BLUE="\033[34m"
	BOLD="\033[1m"
	RESET="\033[0m"
else
	RED=""
	GREEN=""
	YELLOW=""
	BLUE=""
	BOLD=""
	RESET=""
fi

log() {
	printf "%b\n" "$1"
}

pad() {
	printf "%b\n" "${BOLD}------------------------------------------------------------${RESET}"
}

banner() {
	log "${BOLD}  __   __   __              ______     ${RESET}"
	log "${BOLD} |  | |  | |  |  ___  __   /  _____\   ${RESET}"
	log "${BOLD} |  | |  | |  | / _ \| |___|  |        ${RESET}"
	log "${BOLD} |  |_|  |_|  | | \_ | |_| |  |_____    ${RESET}"
	log "${BOLD}  \__________/  \___|\____/\_______/   ${RESET}"
}

die() {
	log "${RED}Error:${RESET} $1"
	exit 1
}

usage() {
	log "Usage: $0 [-d|--dir <folder>]"
}

while [[ $# -gt 0 ]]; do
	case "$1" in
		-d|--dir)
			[[ $# -ge 2 ]] || die "Missing value for $1"
			TARGET_DIR="$2"
			shift 2
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			die "Unknown option: $1"
			;;
	esac
done

if ! command -v unzip >/dev/null 2>&1; then
	die "unzip is required. Please install it and retry."
fi

pad
banner
pad
log ""
log "${YELLOW}::${RESET} Installing release bundle"
pad
log "${BLUE}[1/2] Fetching:${RESET} $URL"

if command -v curl >/dev/null 2>&1; then
	curl -fL --retry 3 -o "$ZIP_NAME" "$URL"
elif command -v wget >/dev/null 2>&1; then
	wget -O "$ZIP_NAME" "$URL"
else
	die "curl or wget is required to download the release archive."
fi

log "${GREEN}Progress:${RESET} [#####-----] 50%"

log "${BLUE}Extracting:${RESET} $ZIP_NAME"
unzip -o "$ZIP_NAME"


if [[ -n "$TARGET_DIR" ]]; then
	if [[ -e "$TARGET_DIR" ]]; then
		die "Target directory already exists: $TARGET_DIR"
	fi
	mkdir -p "$TARGET_DIR"
	shopt -s dotglob nullglob
	mv myApp/* "$TARGET_DIR"/
	shopt -u dotglob nullglob
	rm -rf myApp
	rm "$ZIP_NAME"
fi

log "${GREEN}[2/2] Extracted.${RESET}"
log "${GREEN}Progress:${RESET} [##########] 100%"
log ""
pad
log "${GREEN}All set!${RESET}"
log "Next: cd myApp && make -b"
