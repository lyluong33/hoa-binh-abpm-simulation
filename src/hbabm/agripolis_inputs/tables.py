"""Readers/writers for AgriPoliS's tab-separated, CRLF text inputs."""
from __future__ import annotations

from pathlib import Path


def read_lines(path: Path) -> list[str]:
    with open(path, encoding="latin-1", newline="") as handle:
        return handle.read().replace("\r\n", "\n").split("\n")


def write_lines(path: Path, lines: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="latin-1", newline="") as handle:
        handle.write("\r\n".join(lines) + "\r\n")


def row(*cells) -> str:
    return "\t" + "\t".join(str(c) for c in cells)


def fmt(value: float, digits: int = 2) -> str:
    return f"{value:.{digits}f}".rstrip("0").rstrip(".") if value != int(value) else str(int(value))
