#!/usr/bin/env python3
import argparse
import os
import re
import sys
import xml.etree.ElementTree as ET
from collections import defaultdict


def read_tag_text(root, tag):
    el = root.find(".//" + tag)
    if el is None or el.text is None:
        return None
    return el.text.strip()


def parse_int(s):
    if s is None:
        return None
    # " 240 " みたいなのを許容
    m = re.search(r"-?\d+", s)
    return int(m.group(0)) if m else None


def write_ptHat_summary(run_counter, outfile):

    with open(outfile, "w") as f:
        f.write("<ptHat>\n")

        items = sorted(run_counter.items())  # [((pmin, pmax), nrun), ...]
        for i, ((pmin, pmax), nrun) in enumerate(items):
            if i == 0:
                f.write(f"  <Item>{pmin}</Item>\n")
            f.write(f"  <Item>{pmax}</Item>\n")
            f.write(f"  <RunNum>{nrun}</RunNum>\n")

        f.write("</ptHat>\n")


def main():
    ap = argparse.ArgumentParser(
        description=(
            "Scan jet_RunX.xml, read pTHatMin/Max, create symlinks "
            "JetscapeHadronList<min>_<max>_RunN.out -> jet_X_final_state_hadrons.dat, "
            "and write ptHat summary XML."
        )
    )
    ap.add_argument(
        "folder",
        help="Folder containing jet_Run*.xml (and usually the .dat files)"
    )
    ap.add_argument(
        "--dry-run",
        action="store_true",
        help="Do not create links; just print what would happen"
    )
    ap.add_argument(
        "--force",
        action="store_true",
        help="Overwrite existing symlinks/files at link path"
    )
    ap.add_argument(
        "--pattern",
        default=r"^jet_Run(\d+)\.xml$",
        help="Regex for XML filenames (default: ^jet_Run(\\d+)\\.xml$)"
    )
    ap.add_argument(
        "--link-pattern",
        default="JetscapeHadronListBin{pmin}_{pmax}_Run{run_idx}.out",
        help="Pattern for symlink names (default: JetscapeHadronListBin{pmin}_{pmax}_Run{run_idx}.out)"
    )
    ap.add_argument(
        "--summary-filename",
        default="ptHat_summary.xml",
        help="Filename for ptHat summary XML (default: ptHat_summary.xml)"
    )

    args = ap.parse_args()

    folder = os.path.abspath(args.folder)
    if not os.path.isdir(folder):
        print(f"ERROR: not a directory: {folder}", file=sys.stderr)
        sys.exit(1)

    rx = re.compile(args.pattern)

    # (min,max) -> next run index
    run_counter = defaultdict(int)
    # (min,max) -> list of (xmlfile, target_dat, linkname, linkpath)
    planned = defaultdict(list)

    xml_files = sorted([name for name in os.listdir(folder) if rx.match(name)])

    if not xml_files:
        print("No matching XML files found.", file=sys.stderr)
        sys.exit(1)
    else:
        print(f"Found {len(xml_files)} XML files to process.")

    for xml_name in xml_files:
        print(f"Processing {xml_name}...")
        xml_path = os.path.join(folder, xml_name)
        try:
            tree = ET.parse(xml_path)
            root = tree.getroot()
        except Exception as e:
            print(f"WARNING: failed to parse {xml_name}: {e}", file=sys.stderr)
            continue

        pmin = parse_int(read_tag_text(root, "pTHatMin"))
        pmax = parse_int(read_tag_text(root, "pTHatMax"))
        outfn = read_tag_text(root, "outputFilename")

        print(f"  pTHatMin={pmin}, pTHatMax={pmax}, outputFilename={outfn}")

        if pmin is None or pmax is None:
            print(f"WARNING: {xml_name}: missing pTHatMin/pTHatMax; skipped", file=sys.stderr)
            continue
        if outfn is None:
            print(f"WARNING: {xml_name}: missing outputFilename; skipped", file=sys.stderr)
            continue

        # outputFilename例: /mnt/tmp//jet_130  -> basename: jet_130
        base = os.path.basename(outfn.rstrip("/"))
        # 想定される hadron 出力: jet_130_final_state_hadrons.dat
        target_dat = os.path.join(folder, f"{base}_final_state_hadrons.dat")

        key = (pmin, pmax)
        run_idx = run_counter[key]
        run_counter[key] += 1

        link_name = args.link_pattern.format(pmin=pmin, pmax=pmax, run_idx=run_idx)
        link_path = os.path.join(folder, link_name)

        print(f"  Target data file: {target_dat}")
        print(f"  Target link: {link_path}")

        planned[key].append((xml_name, target_dat, link_name, link_path))

    # 実行
    created = 0
    for (pmin, pmax), items in planned.items():
        for (xml_name, target_dat, link_name, link_path) in items:
            if not os.path.exists(target_dat):
                print(
                    f"WARNING: target not found, skip link: {link_name} -> "
                    f"{os.path.basename(target_dat)} (from {xml_name})",
                    file=sys.stderr
                )
                continue

            # 既存処理（壊れたリンクも検出）
            if os.path.lexists(link_path):
                if args.force:
                    if not args.dry_run:
                        os.remove(link_path)
                else:
                    print(f"WARNING: link exists, skip (use --force): {link_name}", file=sys.stderr)
                    continue

            rel_target = os.path.relpath(target_dat, start=os.path.dirname(link_path))
            if args.dry_run:
                print(f"DRY-RUN: ln -s {rel_target} {link_name}   (from {xml_name})")
            else:
                print(f"Creating symlink: {link_name} -> {os.path.basename(target_dat)} (from {xml_name})")
                os.symlink(rel_target, link_path)
                created += 1
                print(f"  Created")

    # 集計出力（標準出力）
    print("\n=== Summary (pTHatMin, pTHatMax) -> #Runs ===")
    for (pmin, pmax) in sorted(run_counter.keys()):
        print(f"{pmin:>6} {pmax:>6}  ->  {run_counter[(pmin, pmax)]}")

    # Summary をファイルに書き出し
    summary_path = os.path.join(folder, args.summary_filename)
    write_ptHat_summary(run_counter, summary_path)
    print(f"\nptHat summary written to: {summary_path}")

    if args.dry_run:
        print("\n(DRY-RUN) No links created.")
    else:
        print(f"\nCreated {created} symlink(s).")


if __name__ == "__main__":
    main()