#!/usr/bin/env python3
"""Build and run the portable C host-test suites with GCC or Clang."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys


REPOSITORY_ROOT = Path(__file__).resolve().parents[1]
COMMAND_ROOT = Path("projects/06_综合软件实践/嵌入式命令处理框架")
TASK_ROOT = Path("projects/06_综合软件实践/轻量任务状态管理")
CONSOLE_ROOT = Path("projects/06_综合软件实践/固件控制台集成")


TEST_SUITES = (
    (
        "command-framework",
        (
            COMMAND_ROOT / "practice/src/ring_buffer.c",
            COMMAND_ROOT / "practice/src/device.c",
            COMMAND_ROOT / "practice/src/command.c",
            COMMAND_ROOT / "tests/test_command_framework.c",
        ),
        (COMMAND_ROOT / "practice/include",),
    ),
    (
        "ring-buffer",
        (
            COMMAND_ROOT / "practice/src/ring_buffer.c",
            COMMAND_ROOT / "tests/test_ring_buffer.c",
        ),
        (COMMAND_ROOT / "practice/include",),
    ),
    (
        "task-manager",
        (
            TASK_ROOT / "practice/src/task_manager.c",
            TASK_ROOT / "tests/test_task_manager.c",
        ),
        (TASK_ROOT / "practice/include",),
    ),
    (
        "firmware-console",
        (
            COMMAND_ROOT / "practice/src/ring_buffer.c",
            COMMAND_ROOT / "practice/src/device.c",
            COMMAND_ROOT / "practice/src/command.c",
            TASK_ROOT / "practice/src/task_manager.c",
            CONSOLE_ROOT / "practice/src/firmware_console.c",
            CONSOLE_ROOT / "tests/test_firmware_console.c",
        ),
        (
            COMMAND_ROOT / "practice/include",
            TASK_ROOT / "practice/include",
            CONSOLE_ROOT / "practice/include",
        ),
    ),
)


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--compiler",
        default=os.environ.get("CC", "gcc"),
        help="C compiler executable (default: CC or gcc)",
    )
    parser.add_argument(
        "--sanitizers",
        action="store_true",
        help="enable AddressSanitizer and UndefinedBehaviorSanitizer",
    )
    return parser.parse_args()


def run(command: list[str]) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=REPOSITORY_ROOT, check=True)


def main() -> int:
    args = parse_arguments()
    compiler = shutil.which(args.compiler)
    if compiler is None:
        print(f"compiler not found: {args.compiler}", file=sys.stderr)
        return 2

    compiler_name = Path(compiler).stem.replace("+", "p")
    build_directory = REPOSITORY_ROOT / ".build" / f"host-tests-{compiler_name}"
    build_directory.mkdir(parents=True, exist_ok=True)

    common_flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-pedantic"]
    sanitizer_flags: list[str] = []
    if args.sanitizers:
        sanitizer_flags = [
            "-fsanitize=address,undefined",
            "-fno-omit-frame-pointer",
        ]

    executable_suffix = ".exe" if os.name == "nt" else ""
    for name, sources, include_directories in TEST_SUITES:
        executable = build_directory / f"{name}-test{executable_suffix}"
        compile_command = [compiler, *common_flags, *sanitizer_flags]
        compile_command.extend(str(path) for path in sources)
        for include_directory in include_directories:
            compile_command.extend(("-I", str(include_directory)))
        compile_command.extend(("-o", str(executable)))

        run(compile_command)
        run([str(executable)])

    mode = "sanitizers" if args.sanitizers else "strict warnings"
    print(f"host tests passed: {len(TEST_SUITES)} suites ({mode})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
