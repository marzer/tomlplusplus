#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import json
import os
import subprocess
import sys
from argparse import ArgumentParser
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FLOORS = {
    r'lines': r'TOMLPLUSPLUS_COVERAGE_MIN_LINES',
    r'functions': r'TOMLPLUSPLUS_COVERAGE_MIN_FUNCTIONS',
    r'regions': r'TOMLPLUSPLUS_COVERAGE_MIN_REGIONS',
    r'branches': r'TOMLPLUSPLUS_COVERAGE_MIN_BRANCHES',
}


def llvm_cov(*args, **kwargs):
    return subprocess.run([r'llvm-cov', *args], check=True, **kwargs)


def main():
    parser = ArgumentParser(
        description=r'Reports coverage of include/ from the tests of a tomlplusplus_COVERAGE build.'
    )
    parser.add_argument(r'build_dir', type=Path, help=r'a build directory in which ctest has been run')
    args = parser.parse_args()

    build_dir = args.build_dir.resolve()
    profiles = sorted((build_dir / r'profiles').glob(r'*.profraw'))
    if not profiles:
        sys.exit(
            rf'error: no profiles in {build_dir / "profiles"}; configure with tomlplusplus_COVERAGE=ON and run ctest'
        )

    out_dir = build_dir / r'coverage'
    out_dir.mkdir(exist_ok=True)
    profdata = out_dir / r'coverage.profdata'
    subprocess.run([r'llvm-profdata', r'merge', r'-sparse', *profiles, r'-o', profdata], check=True)

    sources = sorted(str(p) for p in (ROOT / r'include').rglob(r'*') if p.suffix in (r'.h', r'.hpp', r'.inl'))
    common = [str(build_dir / r'tests' / r'tomlplusplus_tests'), rf'-instr-profile={profdata}']
    llvm_cov(r'report', *common, *sources)
    llvm_cov(r'show', *common, r'-format=html', rf'-output-dir={out_dir / "html"}', *sources)
    with open(out_dir / r'coverage.lcov', r'w', encoding=r'utf-8') as lcov:
        llvm_cov(r'export', *common, r'-format=lcov', *sources, stdout=lcov)
    summary = llvm_cov(r'export', *common, r'-summary-only', *sources, capture_output=True, text=True).stdout
    totals = json.loads(summary)[r'data'][0][r'totals']

    floors = {metric: float(os.environ[var]) for metric, var in FLOORS.items() if os.environ.get(var)}
    failed = [m for m, floor in floors.items() if totals[m][r'percent'] < floor]

    rows = [r'| Metric | Covered | Total | Percent | Floor |', r'| :--- | ---: | ---: | ---: | ---: |']
    for metric in FLOORS:
        t = totals[metric]
        floor = rf'{floors[metric]:.2f}%' if metric in floors else r'-'
        rows.append(rf'| {metric} | {t["covered"]} | {t["count"]} | {t["percent"]:.2f}% | {floor} |')
    table = '\n'.join(rows)
    print(f'\n{table}\n\nHTML report: {out_dir / "html" / "index.html"}')
    if os.environ.get(r'GITHUB_STEP_SUMMARY'):
        with open(os.environ[r'GITHUB_STEP_SUMMARY'], r'a', encoding=r'utf-8') as step_summary:
            print(f'## Coverage of include/\n\n{table}\n', file=step_summary)

    for metric in failed:
        print(rf'error: {metric} coverage {totals[metric]["percent"]:.2f}% is below the {floors[metric]:.2f}% floor')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
