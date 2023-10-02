import util

if __name__ == "__main__":

    file_path = '/data/traces/python3/main.bin.0'
    lst = util.parse_trace(file_path)
    page_set = set()
    count = 0
    file = open('/data/traces/python3/cleaned.txt','w')
    for item in lst:
        file.write(str(count) + ',' + hex(item)+"\n")
        page_set.add(hex(item))
        count += 1
    file.close()
    
    num_pages = len(list(page_set))
    print('number of pages', num_pages, 'total overhead (KB)', 4 * num_pages)
        
    