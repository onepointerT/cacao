
import { Environment, Stack } from './cacao-vte'

class Transition
    class Equivalence
        tmpl_before: undefined
        tmpl_refactoring: undefined

        constructor: (tmpl_before_str, tmpl_after_str) ->
            @tmpl_before = Environment.Template.fromCode tmpl_before
            @tmpl_refactoring = Environment.Template.fromCode tmpl_after
        
        transformation: (stack = new Stack()) ->
            return @tmpl_before.transformTo stack, @tmpl_refactoring
    

    name: ''
    equiv: undefined

    constructor: (tmpl_before_str, tmpl_after_str, name = '') ->
        equivalence = new Equivalence tmpl_before_str, tmpl_after_str
        @equiv = equivalence
        @name = name
    
    transform: (str) ->
        stack = new Stack()
        transformation_tmpl = new Environment.Template equiv.transformation stack
        return transformation_tmpl.transform str, stack
