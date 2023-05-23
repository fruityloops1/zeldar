import argparse

symbol_matchers = ["", "_ZN", "_ZNK", "_ZTVN"]

def convert_map_to_sym(map_file_path, sym_file_path, symbol_filter=None, start_name=None):
    if start_name == None:
        start_name = "__main_start"
    sym_entries = []

    with open(map_file_path, 'r') as map_file:
        lines = map_file.readlines()

        start_index = None
        for i, line in enumerate(lines):
            if 'Address         Publics by Value' in line:
                start_index = i + 2  # shitty shitty shitty shitty
                break

        if start_index is None:
            print("Error: Symbol table not found in the map file.")
            return

        # Parse symbol entries
        for line in lines[start_index:]:
            line = line.strip()
            if line == '':
                break

            address, symbol = line.split(None, 1)
            address = address.replace('00000000:00000071', '0x')

            for matcher in symbol_matchers:
                filt = f'{matcher}{str(len(symbol_filter))}{symbol_filter}'
                if symbol.startswith(filt):
                    sym_entry = f'{symbol} = {start_name} + {address};'
                    if not symbol.endswith("_0"): # get rid of exported nn defs
                        sym_entries.append(sym_entry)

    with open(sym_file_path, 'w') as sym_file:
        sym_file.write('\n'.join(sym_entries))
        sym_file.write('\n')

    print(f"Symbol file saved as {sym_file_path}.")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert a map file to a sym file.')
    parser.add_argument('input', help='path to the input map file')
    parser.add_argument('output', help='path to the output sym file')
    parser.add_argument('-f', '--filter', dest='symbol_filter', help='filter symbols by prefix')
    parser.add_argument('-s', '--start', dest='start_name', help='use something other than __main_start')

    args = parser.parse_args()
    convert_map_to_sym(args.input, args.output, args.symbol_filter, args.start_name)
