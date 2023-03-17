import json
import sys

desc = {
    "partitions": [
        {
            "type": "primary",
            "number": 1,
            "start": "",
            "end": "+25M",
        },
        {
            "type": "extend",
            "number": 4,
            "start": "",
            "end": "",
            "logics": [
                {
                    "start": "",
                    "end": "+25M"
                },
                {
                    "start": "",
                    "end": ""
                }
            ]
        }
    ]
}

def main():
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'rt') as fp:
            desc = json.load(fp)
    else:
        desc = json.load(sys.stdin)

    print(generate_ic(desc))

def generate_ic(desc: dict) -> str:
    ic = []
    for idx, part in enumerate(desc["partitions"]):
        if part["type"] == "primary":
            num = part["number"]
            start = part["start"]
            end = part["end"]
            ic.append(f"n\np\n{num}\n{start}\n{end}\n")
        else:
            if idx != len(desc["partitions"])-1:
                print("extended partition should be the last partition", file=sys.stderr)
                sys.exit(2)
            start = part["start"]
            end = part["end"]
            num = part["number"]
            ic.append(f"n\ne\n{num}\n{start}\n{end}\n")
            for j, logic in enumerate(part["logics"]):
                start = logic["start"]
                end = logic["end"]
                if end == "" and j != len(part["logics"]) - 1:
                    print("only the last logic partition can omit then 'end' attribute", file=sys.stderr)
                    sys.exit(2)
                ic.append(f"n\n{start}\n{end}\n")
    ic.append("w\n")
    return ''.join(ic)

if __name__ == '__main__':
    main()
