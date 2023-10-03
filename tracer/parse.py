import util
import datetime

if __name__ == "__main__":

    file_path = '/data/traces/python3/main.bin.0'
    lst = util.parse_trace(file_path)
    page_set = set()
    count = 0
    file = open('/data/traces/python3/cleaned.txt','w')
    
    addr = ''
    timestamp_start = 0
    for item in lst:
        # file.write(str(count) + ',' + hex(item)+"\n")
        # page_set.add(hex(item))
        # count += 1
        
        if count % 2 == 0:
            addr = hex(item)
        else:
            if timestamp_start == 0:
                timestamp_start = item
            file.write(str(item - timestamp_start) + ',' + addr + "\n")
            page_set.add(addr)
        count += 1
            
    file.close()
    
    num_pages = len(list(page_set))
    print('number of pages', num_pages, 'total overhead (KB)', 4 * num_pages)
        
    