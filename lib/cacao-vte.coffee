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
        
        @findVariable: (str, start = 0, end = str.length - 1) ->
            pos_var_start = strfind str, '#', start, end
            if pos_var_start > -1
                has_tmpl = strfind str, '}{', pos_var_start, end
                if has_tmpl > -1
                    pos_var_end = 1 + strfind str, '}', has_tmpl+2, end
                else
                    pos_var_end = 1 + strfind str, '}', pos_var_start, end
                if pos_var_end is 0
                    throw new Exception "There is no variable inside '#{str}'.", mark("Cacao.Environment.VariableException", new Error().lineNumber)
                return new Range pos_var_start, pos_var_end, str[pos_var_start..pos_var_end]
            return undefined
        
        @findVariables: (str, start = 0, end = str.length - 1) ->
            rangelist = [Range]
            pos_start_next_variable = start 
            while range = Environment.Variable.findVariable str, pos_start_next_variable, end isnt undefined
                rangelist.push range
                pos_start_next_variable = variable.end + 1

            varenv = {}
            for range in rangelist
                variable = new Environment.Variable range.content
                varenv[variable.name] = variable
            
            return [varenv, rangelist]


    
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

        @findContentUntil: (str, var_range_current, var_range_next) ->
            # Find the content between the two variables current and next
            pos_variable_end = 1 + var_range_current.end
            pos_next_variable = strfind @tmpl_str, '#', pos_variable_end + 1
            content_between = str[pos_var_end..pos_next_variable]

            # Find out their delimiter
            delim_range = strfind_delimiterInTmpl @tmpl_str, var_range_current, var_range_next

            # Find out with what the var_range_current is delimited
            start = if var_range_current.start <= 0 then 0 else var_range_current.start - 1
            if var_range_current.content.length > 0 # We have content, that delimits the following content
                delim_range_before = new Range var_range_current.end, var_range_current.end, var_range_current.content
            else if var_range_current.start is 0
                delim_range_before = new Range 0, 0, ''
            else if @tmpl_str[start..var_range_current.start] isnt '}' # We have a delimiter before in out template
                # Find out the end of the precending variable
                pos_var_end_before = strfindr @tmpl_str, '}', strfindr @tmpl_str, '#', var_range_current.start - 1, var_range_current.start - 1
                delim_between = @tmpl_str[pos_var_end_before+1..var_range_current.start-1]
                delim_range_before = new Range var_range_current.end, var_range_current.end, delim_between
            else # If there is a delimiting string before, use it. Else find out the template content of the variable
                pos_var_start_before = strfindr @tmpl_str, '#', 0, var_range_current.start - 1
                pos_var_content_start = strfind @tmpl_str, '{', pos_var_start_before
                pos_var_end_before = strfindr @tmpl_str, '}', 0, var_range_current.start - 1
                var_range_current_new = new Range pos_var_start_before, pos_var_content_end,
                                                  @tmpl_str[pos_var_start_before..pos_var_end_before]

                delim_range_before = Environment.Template.findContentUntil str, var_range_current_new, var_range_current

            # Now the left and the right delimiter is set. Find its position in text
            pos_delim_left = strfind str, delim_range_before, 0, (strfindr str, delim_range.content)
            pos_delim_right = strfindr str, delim_range.content, 0, pos_delim_left

            # Return everything from the left to the right delimiter
            return new Range pos_delim_left + 1, pos_delim_right - 1, str[pos_delim_left+1..pos_delim_right-1]

        @templateFormatter: (stack, current_var, var_value) ->
            tmpl = current_var.tmpl_formatter
            return tmpl.transform var_value, stack
        
        transformTo: (stack, tmpl2) ->
            return this.transform tmpl2.tmpl_str, stack

        transform: (str, stack = new Stack()) ->
            vars = [varenv, rangelist] = Environment.Variable.findVariables @tmpl_str

            result = ''
            pos_in_str = 0
            for range, idx in rangelist
                current_range = Environment.Template.findContentUntilDelimiter str, ''
                # Every variable counts where it is written, so process it now
                variable = varenv[idx]
                switch variable.type
                    # Variables like ##start{<}
                    when Environment.Variable.Type.store
                        stack[variable.name] = variable.value

                        if variable.tmpl_formatter?
                            result += '###' + variable.name + '{}' + this.templateFormatter stack, variable, variable.value
                        else
                            result += '###' + variable.name + '{}'

                    # Variables like #starttag{}
                    when Environment.Variable.Type.read
                        if variable.tmpl_formatter?
                            result += this.templateFormatter stack, variable, stack[variable.name]
                        else
                            result += stack[variable.name]

                    # Variables like ###properties{}
                    when Environment.Variable.Type.create
                        if variable.value.length > 0
                            stack[variable.name] = variable.value
                        
                        # Now everything else
                        if idx < rangelist.length - 1
                            content_range = Environment.Template.findContentUntil str, range, rangelist[idx+1]
                        else
                            content_range = str[range.end+1..]

                        if variable.tmpl_formatter?
                            result += this.templateFormatter stack, variable, variable.value + content_range
                        else
                            result += variable.value + content_range
                    else
                        throw new Exception "Could not transform string. Unknown variable type '#{variable.type}'.", mark("Environment.TemplateException", new Error().lineNumber)
            return result

    
        readToEnv: (env) ->
            vars = this.read()
            for variable in vars
                env.addvar variable
    

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


module.exports = {
    Environment,
    Stack
}
