
import { Environment } from './cacao-vte'

class Cacao
    env: new Environment()
    tmpl: {}

    constructor: ({} = configuration) ->
        @env = new Environment(if configuration['env']? then configuration['env'])
    
    addtmpl: (template) ->
        @tmpl[template.name] = template

    addtmpl: (name, template_str) ->
        template = new Environment.Template name, template_str
        addtmpl template
