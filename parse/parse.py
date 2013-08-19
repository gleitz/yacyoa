import json
pages = []
page_map = {}

WIN_MAP = {'*':'#2ca02c',
           '-':'#d62728'}

PEOPLE_MAP = {'b':'#e377c2',
              'j':'#ff7f0e',
              'l':'#8c564b',
              'm':'#7f7f7f',
              'k':'#17becf'}

with open('graph.txt', 'r') as f:
    for line in f.readlines():
        line = line.strip()
        if '/' not in line:
            for symbol in WIN_MAP.keys():
                if symbol in line:
                    page = int(line.replace(symbol, ''))
                    color = WIN_MAP[symbol]
                    pages.append({'id': page,
                                  'color': color,
                                  'children': []})
        else:
            current_page, children = line.split('/')
            children = map(int, children.split('+'))
            color = 'black'
            page = False
            for symbol in PEOPLE_MAP.keys():
                if symbol in current_page:
                    page = int(current_page.replace(symbol, ''))
                    color = PEOPLE_MAP[symbol]
            if not page:
                page = int(current_page)
            pages.append({'id': page,
                          'color': color,
                          'children': children})

for page in pages:
    page_map[page['id']] = page

for page in pages:
    if len(page['children']) == 1:
        new_children = page_map[page['children'][0]]['children']
        page_map[page['id']]['children'] = new_children

print json.dumps(page_map.values())
