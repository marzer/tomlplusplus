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
BADGE_COLOURS = ((90, r'#4c1'), (80, r'#97ca00'), (70, r'#a4a61d'), (60, r'#dfb317'), (50, r'#fe7d37'), (0, r'#e05d44'))


def llvm_cov(*args, **kwargs):
    return subprocess.run([r'llvm-cov', *args], check=True, **kwargs)


def badge(percent):
    label = r'coverage'
    value = rf'{percent:.2f}%'
    colour = next(colour for floor, colour in BADGE_COLOURS if percent >= floor)
    label_width = 7 * len(label) + 10
    value_width = 7 * len(value) + 10
    return (
        rf'<svg xmlns="http://www.w3.org/2000/svg" width="{label_width + value_width}" height="20" role="img"'
        rf' aria-label="{label}: {value}"><title>{label}: {value}</title><g shape-rendering="crispEdges">'
        rf'<rect width="{label_width}" height="20" fill="#555"/>'
        rf'<rect x="{label_width}" width="{value_width}" height="20" fill="{colour}"/></g>'
        r'<g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif"'
        r' text-rendering="geometricPrecision" font-size="110">'
        rf'<text x="{5 * label_width}" y="140" transform="scale(.1)"'
        rf' textLength="{10 * (label_width - 10)}">{label}</text>'
        rf'<text x="{10 * label_width + 5 * value_width}" y="140" transform="scale(.1)"'
        rf' textLength="{10 * (value_width - 10)}">{value}</text></g></svg>'
    )


def file_table(files):
    rows = [
        r'| File | Missed lines | Lines | Functions | Regions | Branches |',
        r'| :--- | ---: | ---: | ---: | ---: | ---: |',
    ]
    missed = []
    for f in files:
        s = f[r'summary']
        count = s[r'lines'][r'count'] - s[r'lines'][r'covered']
        if count:
            missed.append((count, Path(f[r'filename']).resolve().relative_to(ROOT / r'include').as_posix(), s))
    for count, name, s in sorted(missed, key=lambda row: (-row[0], row[1])):
        percents = r' | '.join(rf'{s[m][r"percent"]:.2f}%' for m in FLOORS)
        rows.append(rf'| {name} | {count} | {percents} |')
    return '\n'.join(rows)


def main():
    parser = ArgumentParser(
        description=r'Reports coverage of include/ from the tests of one or more tomlplusplus_COVERAGE builds, merged.'
    )
    parser.add_argument(r'build_dirs', type=Path, nargs=r'+', help=r'build directories in which ctest has been run')
    args = parser.parse_args()

    build_dirs = [d.resolve() for d in args.build_dirs]
    profiles = []
    for build_dir in build_dirs:
        found = sorted((build_dir / r'profiles').glob(r'*.profraw'))
        if not found:
            sys.exit(
                rf'error: no profiles in {build_dir / "profiles"}; configure with tomlplusplus_COVERAGE=ON and run ctest'
            )
        profiles += found

    out_dir = build_dirs[0] / r'coverage'
    out_dir.mkdir(exist_ok=True)
    profdata = out_dir / r'coverage.profdata'
    subprocess.run([r'llvm-profdata', r'merge', r'-sparse', *profiles, r'-o', profdata], check=True)

    sources = sorted(str(p) for p in (ROOT / r'include').rglob(r'*') if p.suffix in (r'.h', r'.hpp', r'.inl'))
    binaries = [str(d / r'tests' / r'tomlplusplus_tests') for d in build_dirs]
    common = [binaries[0], *(arg for b in binaries[1:] for arg in (r'-object', b)), rf'-instr-profile={profdata}']
    llvm_cov(r'report', *common, *sources)
    llvm_cov(r'show', *common, r'-format=html', rf'-output-dir={out_dir / "html"}', *sources)
    with open(out_dir / r'coverage.lcov', r'w', encoding=r'utf-8') as lcov:
        llvm_cov(r'export', *common, r'-format=lcov', *sources, stdout=lcov)
    summary = json.loads(
        llvm_cov(r'export', *common, r'-summary-only', *sources, capture_output=True, text=True).stdout
    )[r'data'][0]
    totals = summary[r'totals']
    (out_dir / r'html' / r'badge.svg').write_text(badge(totals[r'lines'][r'percent']), encoding=r'utf-8')

    floors = {metric: float(os.environ[var]) for metric, var in FLOORS.items() if os.environ.get(var)}
    failed = [m for m, floor in floors.items() if totals[m][r'percent'] < floor]

    rows = [r'| Metric | Covered | Total | Percent | Floor |', r'| :--- | ---: | ---: | ---: | ---: |']
    for metric in FLOORS:
        t = totals[metric]
        floor = rf'{floors[metric]:.2f}%' if metric in floors else r'-'
        rows.append(rf'| {metric} | {t["covered"]} | {t["count"]} | {t["percent"]:.2f}% | {floor} |')
    table = '\n'.join(rows)
    files = file_table(summary[r'files'])
    print(f'\n{table}\n\n{files}\n\nHTML report: {out_dir / "html" / "index.html"}')
    if os.environ.get(r'GITHUB_STEP_SUMMARY'):
        with open(os.environ[r'GITHUB_STEP_SUMMARY'], r'a', encoding=r'utf-8') as step_summary:
            print(f'## Coverage of include/\n\n{table}\n\n### Files with missed lines\n\n{files}\n', file=step_summary)

    for metric in failed:
        print(rf'error: {metric} coverage {totals[metric]["percent"]:.2f}% is below the {floors[metric]:.2f}% floor')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
