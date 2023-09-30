import util
from collections import Counter

if __name__ == "__main__":

    file_path = '/data/traces/python3/main.bin.0'
    lst = util.parse_trace(file_path)
    
    
    file = open('/data/traces/python3/cleaned.txt','w')
    for item in lst:
        file.write(str(item)+"\n")
    file.close()
        
    