# Variable environment templates work like this:
# It stores values with ##var_name{#{value}} and reads them with #var_name
# And dynamically finds into variables with ###var_name{} on template parsing
# For example:
# ##start{<}###starttag{} ###properties{}##end{>}
# ###body{}
# #start{}/#starttag{}#end{}
# can define the syntax template how html basically works
# and evolves to
# <###starttag{} ###properties{}>
# ##body{}
# </#starttag>

import './path'

class Stack extends {}
    stacktype: 'generic'
        
    constructor: (stack_type, {} = configuration) ->
        super()
        @stacktype = stack_type
        if configuration.globals?
            @globals += configuration.globals


class Environment extends Stack
    class Variable
            constructor: (@name, @value) ->
    
    class Template
        constructor: (@name, @tmpl_str) ->
        
        @fromFile: (path) ->
            fd = new File path
            return new Environment.Template fd.basename(), fd.read()


    constructor: ({} = configuration) ->
        super('format_env', configuration)
        @variables =  {}
    
    addvar: (variable) ->
        variables[variable.name] = variable.value

    addvar: (name, value) ->
        variable = new Environment.Variable name, value
        variables[name] = variable
    
    get: (name) ->
        if not variables[name]?
            return undefined
        return variables[name]

    getValue: (name) ->
        if not variables[name]?
            return undefined
        return variables[name].value

module.exports = Environment