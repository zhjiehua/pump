#!/usr/bin/env bash
# Generate compile_commands.json at repo root for clangd, then drop stale index.
# Does not rebuild binaries (safe while debugging).
# Usage: sync_clangd_db.sh [hmi|weiduodianzi|all] [--keep-index]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TARGET="${1:-hmi}"
KEEP_INDEX=0
if [[ "${2:-}" == "--keep-index" ]]; then
    KEEP_INDEX=1
fi

QMAKE="${QMAKE:-qmake}"
MAKEFILES=()
LABEL=""

ensure_qmake() {
    local src_dir="$1"
    local build_dir="$2"
    local pro="$3"
    mkdir -p "${build_dir}"
    if [[ ! -f "${build_dir}/Makefile" || "${pro}" -nt "${build_dir}/Makefile" ]]; then
        (cd "${build_dir}" && "${QMAKE}" "${pro}")
    fi
    if [[ -f "${build_dir}/Makefile" ]]; then
        MAKEFILES+=("${build_dir}/Makefile")
    fi
}

case "${TARGET}" in
    hmi)
        LABEL="HMI"
        ensure_qmake \
            "${ROOT}/hmi" \
            "${ROOT}/hmi/build" \
            "${ROOT}/hmi/hmi.pro"
        ;;
    weiduodianzi)
        LABEL="weiduodianzi"
        ensure_qmake \
            "${ROOT}/pump/weiduodianzi" \
            "${ROOT}/pump/weiduodianzi/build_desktop" \
            "${ROOT}/pump/weiduodianzi/weiduodianzi_desktop.pro"
        ;;
    all)
        LABEL="HMI+weiduodianzi"
        ensure_qmake \
            "${ROOT}/hmi" \
            "${ROOT}/hmi/build" \
            "${ROOT}/hmi/hmi.pro"
        ensure_qmake \
            "${ROOT}/pump/weiduodianzi" \
            "${ROOT}/pump/weiduodianzi/build_desktop" \
            "${ROOT}/pump/weiduodianzi/weiduodianzi_desktop.pro"
        ;;
    *)
        echo "Usage: $0 {hmi|weiduodianzi|all} [--keep-index]" >&2
        exit 1
        ;;
esac

if [[ ${#MAKEFILES[@]} -eq 0 ]]; then
    echo "No qmake Makefile found for ${TARGET}" >&2
    echo "Set QMAKE (now: ${QMAKE}) and run the matching QMake task first." >&2
    exit 1
fi

python3 "${ROOT}/scripts/gen_compile_commands.py" \
    "${MAKEFILES[@]}" \
    -o "${ROOT}/compile_commands.json"

echo "${LABEL}" > "${ROOT}/.clangd-target"
touch "${ROOT}/compile_commands.json"

if [[ "${KEEP_INDEX}" -eq 0 ]]; then
    for dir in \
        "${ROOT}/.cache/clangd/index" \
        "${ROOT}/.clangd/index" \
        "${ROOT}/hmi/.cache/clangd/index"; do
        if [[ -d "${dir}" ]]; then
            rm -rf "${dir}"
            echo "cleared ${dir}"
        fi
    done
fi

echo "clangd compile_commands -> ${LABEL}"
echo "Next: Command Palette -> clangd: Restart language server"
echo "Wait until status bar finishes indexing before expecting cross-file jumps."
