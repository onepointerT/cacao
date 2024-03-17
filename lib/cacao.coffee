
import { Environment as BaseEnvironment } from './cacao-vte'

class Cacao
    class Environment extends BaseEnvironment
    class Variable extends BaseEnvironment.Variable
    class Template extends BaseEnvironment.Template

    env: new Environment()
    tmpl: {}

    constructor: ({} = configuration) ->
        @env = new Environment(if configuration['env']? then configuration['env'])
        @config = configuration
    
    addtmpl: (template) ->
        @tmpl[template.name] = template

    addtmpl: (name, template_str) ->
        template = new Environment.Template name, template_str
        addtmpl template


module.exports = Cacao
