
import { Environment as BaseEnvironment } from './cacao-vte'
import { Transition as BaseTransition } from './cacao-transition'

import { strfind } from './str'

class Cacao
    class Environment extends BaseEnvironment
    class Variable extends BaseEnvironment.Variable
    class Template extends BaseEnvironment.Template
    class Transition extends BaseTransition

    env: new Environment()
    tmpl: {}
    transitions: {}

    constructor: ({} = configuration) ->
        @env = new Environment(if configuration['env']? then configuration['env'])
        @config = configuration
    
    addtmpl: (template) ->
        @tmpl[template.name] = template

    addtmpl: (name, template_str) ->
        template = new Environment.Template name, template_str
        addtmpl template
    
    addtransition: (name, tmpl_str1, tmpl_str2) ->
        tran = new Transition tmpl_str1, tmpl_str2, name
        @transitions[name] = tran
    
    toString(str, stack = this.env) ->
        pos_next_variable = 0
        rest_str = str
        new_str = ''
        while pos_next_variable = strfind str, '#' > -1
            pos_end_next_variable = strfind str, '}', pos_next_variable
            varname = str[pos_next_variable+1..pos_end_next_variable-2]
            rest_str = rest_str[pos_end_next_variable+1..]
            new_str += stack[varname] + rest_str

        new_str += rest_str
        return new_str


module.exports = Cacao
