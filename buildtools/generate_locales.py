#!/usr/bin/env python3

import argparse
import os

def ensure_dir(filepath: str):
    path = os.path.dirname(filepath)
    if len(path) == 0:
        return
    os.makedirs(path, exist_ok = True)

def silence(command: str):
    return command

def gen_pot(input_files: str, pot: str, keyword: str):
    ensure_dir(pot)
    command = f"xgettext {input_files} -o {pot} -k{keyword}"
    os.system(silence(command))

def new_po(pot: str, po: str, lang: str):
    ensure_dir(po)
    command = f"msginit -o {po} -i {pot} -l {lang} --no-translator"
    os.system(silence(command))

def merge_po(pot: str, po: str):
    ensure_dir(po)
    command = f"msgmerge {po} {pot} -o {po}" 
    os.system(silence(command))

def gen_po(pot: str, po: str, lang: str):
    if os.path.exists(po):
        merge_po(pot, po)
    else:
        new_po(pot, po, lang)

def gen_mo(po, mo):
    ensure_dir(mo)
    command = f"msgfmt {po} -o {mo}"
    os.system(silence(command))

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--outdir', default = "locale")
parser.add_argument('-n', '--domainname', required = True)
parser.add_argument('-l', '--locales',    required = True)
parser.add_argument('-i', '--inputfiles', required = True)
parser.add_argument('-c', '--categories', default = "LC_MESSAGES")

args = parser.parse_args()

outdir      = args.outdir.strip()
domain_name = args.domainname.strip()
srclist_str = args.inputfiles.strip()
locale_list = args.locales.strip().strip(",").split(",")
categories  = args.categories.strip().strip(",").split(",")

keywords = {
    "LC_MESSAGES": "_",
    "LC_TIME":     "_t",
    "LC_COLLATE":  "_c",
    "LC_CTYPE":    "_ct",
}

for k,v in keywords.items():
    if not k in categories:
        continue

    pot_file = f"{outdir}/{domain_name}-{k}.pot"
    gen_pot(srclist_str, pot_file, v)

    for locale in locale_list:
        po_file = f"{outdir}/{locale}/{k}/{domain_name}.po"
        mo_file = f"{outdir}/{locale}/{k}/{domain_name}.mo"

        gen_po(pot_file, po_file, locale)
        gen_mo(po_file, mo_file)

