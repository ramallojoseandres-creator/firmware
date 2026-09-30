#!/bin/bash
set -e
echo ">> Renombrando a RamaSoft..."
grep -rl --include=\*.cpp --include=\*.h "Bruce" src/ 2>/dev/null | while read f; do
  case "$f" in
    *credit*|*Credit*|*about*|*About*) continue ;;
  esac
  sed -i 's/Bruce/RamaSoft/g' "$f"
done
echo ">> Listo."