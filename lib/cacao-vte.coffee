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
import './str'

class Stack extends {}
    stacktype: 'generic'
        
    constructor: (stack_type, {} = configuration) ->
        super()
        @stacktype = stack_type
        @config = configuration
        if configuration.globals?
            @globals += configuration.globals


class Environment extends Stack
    class Variable
        Type:
            store: 'store'
            read: 'read'
            create: 'create'
        
        name: ''
        value: ''
        type: ''
        tmpl_formatter: undefined

        constructor: (@var_str) ->
            @name = ''
            @value = ''
            @type = ''
            @tmpl_formatter = undefined
            try
                Environment.Variable.fromCode @var_str, this
            catch e
                console.log e.toString()
        
        @fromCode: (var_str, variable) ->
            is_there_a_formatter = strfindr var_str, '}{'
            if is_there_a_formatter > -1
                pos_formatter_start = 2 + strfind var_str, '}{'
                tmpl_str = var_str[pos_formatter_start..var_str.length-2] # Omit closing brace
                variable.tmpl_formatter = Environment.Template.fromCode tmpl_str

            prefix_store = strcountprefix var_str, '#'
            switch prefix_store
                when 1
                    variable.type = Environment.Variable.Type.read
                when 2
                    variable.type = Environment.Variable.Type.store
                when 3
                    variable.type = Environment.Variable.Type.create

            pos_var_name = 1 + strfindr var_str, '#'
            if pos_var_name > 1
                pos_var_value = 1 + strfind var_str, '{'
                if pos_var_value > 1
                    variable.name = var_str[pos_var_name..pos_var_value-2]
                    pos_var_value_end = strfind var_str, '}', pos_var_value - 1
                    if pos_var_value_end
                        variable.value = var_str[pos_var_value..pos_var_value_end]
                    else
                        variable.value = ''
                else
                    variable.name = var_str[pos_var_name..]
        
        toEnv: (env) ->


    
    class Template
        name: ''
        tmpl_str: ''
        
        constructor: (@name, @tmpl_str) ->
        
        @fromCode: (tmpl_str) ->
            pos_tmpl_name = strfind tmpl_str, '>>'
            if pos_tmpl_name isnt -1
                pos_name_linebreak = strfind tmpl_str, '\n', pos_tmpl_name
                pos_tmpl_name += 2
                name = tmpl_str[pos_tmpl_name..pos_name_linebreak-1]
                return new Environment.Template name, tmpl_str[pos_name_linebreak+1..]
            else
                return new Environment.Template '', tmpl_str
            
        
        @fromFile: (path) ->
            fd = new File path
            return Environment.Template.fromCode fd.basename(), fd.read()

        read: () ->
            variable_list = [Environment.Variable]

            lane_list = strsplitat '\n', @tmpl_str, true
            for lane in lane_list
                pos_next_variable = strfind lane, '#'
                pos_end_next_variable = strfind lane, '}', pos_next_variable
                if pos_end_next_variable > - 1
                    if lane[pos_next_variable+1] is '{' # We have a variable with snippet formatter function
                        pos_start_formatter_snippet = pos_next_variable + 2
                        pos_end_formatter_snippet = strfind lane, '}', pos_start_formatter_snippet
                        variable = new Environment.Variable lane[pos_next_variable..pos_end_formatter_snippet]
                    else # Usual variable with only a value
                        variable = new Environment.Variable lane[pos_next_variable..pos_end_next_variable]
                    variable_list.push variable
            
            return variable_list


        readToEnv: (env) ->
            vars = this.read()
            for variable in vars
                env.variables[variable.name] = variable
                    


    constructor: ({} = configuration) ->
        super('format_env', configuration)
        @variables =  {}
        @templates = {}
    
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
