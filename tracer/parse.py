import util
from collections import Counter

if __name__ == "__main__":

    file_path = '/data/traces/python3/main.bin.0'
    lst = util.parse_trace(file_path)
    
    
    print(len(lst))
    print(Counter(lst))