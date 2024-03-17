
import { Environment as BaseEnvironment } from './cacao-vte'
import { Transition as BaseTransition } from './cacao-transition'

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


module.exports = Cacao
