


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
    i = _opstr.length
    for srch_str, i in _opstr of i is [_opstr.length-findstr.length-1..0] when srch_str[i+findstr.length..i] is findstr
        return i
    return -1


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


module.exports = {
    strcountprefix,
    strfind,
    strfindr,
    strdiffsimple,
    strsplitat
}