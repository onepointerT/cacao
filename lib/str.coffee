




class Range
    start: 0
    end: 0
    content: ''

    constructor: (range_start, range_end, range_content = '') ->
        @start = range_start
        @end = range_end
        @content = range_content


strcountprefix: (str, findstr, pos = 0, pos_until = 0) ->
    if pos > 0
        _opstr = str[pos..]
    else if pos_until > 0 and pos > 0
        _opstr = str[pos..pos_until]
    else if pos_until > 0
        _opstr = str[..pos_until]
    else
        _opstr = str
    count = 0
    for srch_str, i in _opstr
        if srch_str[i+findstr.length] is findstr
            count += 1
        else
            return count
    return count


strfind: (str, findstr, pos = 0, pos_until = 0) ->
    if pos > 0
        _opstr = str[pos..]
    else if pos_until > 0 and pos > 0
        _opstr = str[pos..pos_until]
    else if pos_until > 0
        _opstr = str[..pos_until]
    else
        _opstr = str
    i = 0
    for srch_str, i in _opstr of i is [0.._opstr.length-findstr.length-1] when srch_str[i..i+findstr.length] is findstr
        return i
    return -1


strfindr: (str, findstr, pos = 0, pos_until = 0) ->
    if pos > 0
        _opstr = str[pos..]
    else if pos_until > 0 and pos > 0
        _opstr = str[pos..pos_until]
    else if pos_until > 0
        _opstr = str[..pos_until]
    else
        _opstr = str
    i = _opstr.length-1
    for srch_str, i in _opstr of i is [_opstr.length-findstr.length-1..0] when srch_str[i+findstr.length..i] is findstr
        return i
    return -1



strfind_delimiterInTmpl: (tmpl_str, var_range_current, var_range_next) ->
    pos_tmpl_var_current = var_range_current.start
    pos_tmpl_var_next = strfind tmpl_str, var_range_next.content, pos_tmpl_var_current + var_range_current.content.length
    
    # Now diff the content string of tmpl_str from begin of var_range_current until the next variable
    # The result will be, thus inner contains the delimiter and
    diff = [before, after, inner] = strdiffsimple tmpl_str[pos_tmpl_var_current..pos_tmpl_var_next], var_range_current

    return new Range var_range_current.end + 1, var_range_current.end + 1 + inner.length, inner
    
    
strfind_untilDelimiter: (str, from_range, delim, rstart = from_range.end + 1, end = str.length - delim.length - 1) ->
    pos_delim = strfindr str, delim, from_range, rstart
    return new Range from_range.start, pos_delim, str[..pos_delim]


# Returns [before, after, inner]
strdiffsimple: (str1, str2) ->
    diff = [before, after, inner] = ['', '', '']
    
    if strfind(str1, str2) > -1
        inner = str2
        outer = str1
    else if strfind(str2, str1) > -1
        inner = str1
        outer = str2
    else return diff

    pos_inner = strfind(outer, inner)
    pos_end_inner = pos_inner + inner.length

    before = outer[..pos_inner]
    after = outer[pos_end_inner+1..]
    
    return diff


strreplace: (str, searchstr, replacestr) ->
    return String(str).replace(searchstr, replacestr)


strsplitat: (delimiter, str, include_delimiter = false) ->
    tokenlist = []
    current_first_pos = 0
    for char, idx in str of idx is [0..str.length-1]
        if str[idx..idx+delimiter.length] is delimiter
            if include_delimiter
                tokenlist.push str[current_first_pos..idx-1]
            else
                tokenlist.push str[current_first_pos..idx-1]
            current_first_pos = idx+delimiter.length+1
            idx = current_first_pos-1
    return tokenlist


genuuidv4: () ->
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'
        .replace(/[xy]/g, (c) =>
            r = Math.random() * 16 | 0
            v = c == 'x' ? r : (r & 0x3 | 0x8)
            return v.toString(16)
        )

genid: () ->
    return strreplace(genuuidv4(), '-', '')


module.exports = {
    Range,
    strcountprefix,
    strfind,
    strfindr,
    strfind_delimiterInTmpl,
    strfind_untilDelimiter,
    strdiffsimple,
    strreplace,
    strsplitat,
    genid
}