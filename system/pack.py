from pathlib import Path


def main():
    root = "rootfs"
    files = []
    collect(Path(root), files)

    size_file = "size.bin"
    index_file = "files.index"
    package_file = "package.bin"

    offset = 0

    with open(index_file, 'wt') as idxfp, open(package_file, 'wb') as pkfp:
        for f in files:
            s_path = str(f)
            assert s_path.startswith(root)
            g_path = s_path[len(root):]
            size = f.stat().st_size

            with f.open("rb") as _fp:
                pkfp.write(_fp.read())
            idxfp.write(f"{g_path} {offset} {size}\n")
            offset += size

    with open(size_file, 'wb') as szfp:
        index_size = Path(index_file).stat().st_size
        pack_size = Path(package_file).stat().st_size
        print(f"index file size: {index_size}, package file size: {pack_size}")
        szfp.write(index_size.to_bytes(4, 'little'))
        szfp.write(pack_size.to_bytes(4, 'little'))


def collect(root: Path, files: "List[Path]") -> None:
    for child in root.iterdir():
        if child.is_file():
            files.append(child)
        else:
            collect(child, files)


if __name__ == '__main__':
    main()
