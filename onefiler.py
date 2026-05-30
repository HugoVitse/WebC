import argparse
from pathlib import Path

macroExclude = ["pragma", 'include "']


def getFiles(incdir: str):
    dossier = Path(incdir)
    fichiers = [str(f.resolve()) for f in dossier.iterdir() if f.is_file()]
    return fichiers


def writeFile(file, content):
    with open(file, "w", encoding="utf-8") as _file:
        _file.write(content)


def readFile(file):
    content = ""
    with open(file, "r", encoding="utf-8") as _file:
        for line in _file:
            lineStripped = line.rstrip()
            isIn = False
            for exclude in macroExclude:
                if exclude in lineStripped:
                    isIn = True
                    break
            if not isIn:
                content += f"{lineStripped}\n"

    return content


def makeFile(files, types):
    content = "#pragma once\n"
    typeContent = readFile(types)
    content += typeContent
    for file in files:
        contentFile = readFile(file)
        if contentFile != typeContent:
            content += contentFile
    return content


def main():
    parser = argparse.ArgumentParser(description="Produces a single header file")

    parser.add_argument("incdir", help="include directory")
    parser.add_argument("output", help="output header file")
    parser.add_argument("types", help="types header file")

    args = parser.parse_args()

    print(f"include directory:  {args.incdir}")
    print(f"output file:   {args.output}")

    files = getFiles(args.incdir)
    file = makeFile(files, args.types)
    writeFile(args.output, file)


main()
