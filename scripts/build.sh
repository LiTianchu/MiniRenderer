#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
BUILD_TYPE="Debug"

usage() {
	cat <<EOF
Usage: $(basename "$0") [options]

Options:
  -t, --type <Debug|Release>   CMake build type (default: Debug)

Examples:
  ./scripts/build.sh
	./scripts/build.sh -t Release
EOF
}

while [[ $# -gt 0 ]]; do
	case "$1" in
		-t|--type)
			BUILD_TYPE="$2"; shift 2 ;;
		-h|--help)
			usage; exit 0 ;;
		*)
			echo "Unknown option: $1" >&2
			usage
			exit 2
			;;
	esac
done

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
	-DCMAKE_BUILD_TYPE="$BUILD_TYPE"

cmake --build "$BUILD_DIR" -j

echo "Built: $BUILD_DIR/minirenderer"
