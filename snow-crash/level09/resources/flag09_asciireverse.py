import sys

def asciireverse(string):
    hex_list=string.split(" ")
    for index, elem in enumerate(hex_list):
        val=int(elem, 16) - index
        hex_list[index] = chr(val)
    flag ="".join(hex_list)
    print(flag)

def main():
    asciireverse(sys.argv[1])
    return

if __name__ == "__main__" :
    main()
