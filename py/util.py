
from uuid import uuid4


class Range:
    start = 0
    end = 0
    content = str()

    def __init__(self, range_start, range_end, range_content=str()):
        self.start = range_start
        self.end = range_end
        self.content = range_content



def strdiffsimple(str1: str, str2: str) -> [str, str, str]:
    diff = [before, after, inner] = ['', '', '']

    if str1.find(str2) > -1: # str2 is in str1
        inner = str2
        outer = str1
    elif str2.find(str1) > -1: # str1 is in str2
        inner = str1
        outer = str2
    else:
        return diff

    pos_inner = outer.find(inner)
    pos_end_inner = pos_inner + len(inner)

    before = outer[:pos_inner]
    after = outer[pos_end_inner+1:]

    return [before, after, inner]


def strfind_delimiterInTmpl(tmpl_str: str, var_range_current: Range, var_range_next: Range) -> Range:
    pos_tmpl_var_current = var_range_current.start
    pos_tmpl_var_next = tmpl_str.find(var_range_next.content, pos_tmpl_var_current + len(var_range_current.content))

    # Now diff the content string of tmpl_str from begin of var_range_current until the next variable
    # The result will be, thus inner contains the delimiter and
    diff = [before, after, inner] = strdiffsimple(tmpl_str[pos_tmpl_var_current:pos_tmpl_var_next-1], var_range_current.content)

    return Range(var_range_current.end+1, var_range_current.end+1+len(inner), inner)


def strfind_untilDelimiter(tmpl_str, from_range: Range, delim: str, start=-1, end=-1) -> Range:
    # Set default depending parameters
    if start is -1:
        start = from_range.end + 1
    elif end is -1:
        end = len(tmpl_str) - len(delim) - 1

    pos_delim = tmpl_str.find(delim, start, end)
    return Range(0, pos_delim - 1, tmpl_str[:pos_delim-1])


def genuuid4():
    return str(uuid4())


def genid():
    return genuuid4().replace('-', '')


